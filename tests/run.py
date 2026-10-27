#!/usr/bin/env python3
"""Compile and run sanitised tests using a test-only LibLapin surface."""
import concurrent.futures
import gzip
import os
from pathlib import Path
import subprocess
import sys

root = Path(__file__).resolve().parents[1]
os.chdir(root)
build = root / '.test-build'
build.mkdir(exist_ok=True)
cxx = os.environ.get('CXX', 'g++')
flags = ['-std=gnu++23', '-O1', '-g', '-Wall', '-Wextra',
         '-Wno-missing-field-initializers', '-fno-omit-frame-pointer',
         '-fsanitize=address,undefined,float-cast-overflow', '-D_GLIBCXX_ASSERTIONS',
         '-Isilicon/include', '-Itests/support', '-Iinclude', '-Iinclude/stdcmp', '-Iinclude/vircmp', '-pthread']
sources = sorted(p for folder in ('src', 'screen', 'stdcmp', 'vircmp')
                 for p in Path(folder).glob('*.cpp'))
entries = [Path('main.cpp'), Path('tests/reliability.cpp'), Path('tests/w65c02.cpp'),
           Path('tests/prototyping.cpp'), Path('tests/routing.cpp'), Path('tests/integrated.cpp'), Path('tests/ux.cpp')]
sources += entries
headers = [p for folder in ('include', 'tests/support','silicon/include') for p in Path(folder).rglob('*') if p.is_file()]
flag_file = build / 'flags.txt'
flag_text = '\n'.join([cxx, *flags])
if not flag_file.exists() or flag_file.read_text() != flag_text:
    flag_file.write_text(flag_text)
    for obj in build.glob('*.o'):
        obj.unlink()
stamp = max(p.stat().st_mtime for p in headers)
silicon_dependencies = [Path('silicon/include/Silicon.hpp'),
                        Path('silicon/src/Document.cpp'), Path('silicon/src/Extract.cpp'),
                        Path('silicon/src/Simulator.cpp'), Path('silicon/src/Hierarchy.cpp'),
                        Path('silicon/include/Editor.hpp'), Path('silicon/src/Editor.cpp'), Path('silicon/src/Image.cpp'), Path('silicon/src/Examples.cpp'), Path('silicon/src/Main.cpp')]
silicon_stamp = max(p.stat().st_mtime for p in silicon_dependencies)

def compile_one(source):
    obj = build / (str(source).replace('/', '_') + '.o')
    dependency_stamp = silicon_stamp if source.name in ('SiliconEngine.cpp', 'SiliconEditor.cpp', 'SiliconChip.cpp', 'Create.cpp', 'Integrated.cpp', 'IntegratedWorkspace.cpp', 'Workspace.cpp') else 0
    if not obj.exists() or obj.stat().st_size == 0 or obj.stat().st_mtime < max(stamp, dependency_stamp, source.stat().st_mtime):
        pending = obj.with_suffix('.pending')
        result = subprocess.run([cxx, *flags, '-c', str(source), '-o', str(pending)],
                                text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        if result.returncode:
            raise RuntimeError(str(source) + '\n' + result.stdout)
        if not pending.exists() or pending.stat().st_size == 0:
            raise RuntimeError('Compiler produced an empty object: ' + str(source))
        pending.replace(obj)
    return obj

try:
    with concurrent.futures.ThreadPoolExecutor(max_workers=int(os.environ.get('JOBS', '3'))) as pool:
        objects = []
        for index, obj in enumerate(pool.map(compile_one, sources), 1):
            objects.append(obj)
            if index % 25 == 0:
                print(f'Compiled {index}/{len(sources)} translation units', flush=True)
    common = [str(obj) for source, obj in zip(sources, objects)
              if source not in entries]
    for name, entry, extra in [
            ('bcad', 'main.cpp', []),
            ('reliability', 'tests/reliability.cpp',
             ['-Wl,--wrap=write,--wrap=fsync,--wrap=close,--wrap=rename,--wrap=link']),
            ('prototyping', 'tests/prototyping.cpp', []),
            ('routing', 'tests/routing.cpp', []),
            ('integrated', 'tests/integrated.cpp', []),
            ('ux', 'tests/ux.cpp', [])]:
        obj = objects[sources.index(Path(entry))]
        subprocess.run([cxx, *flags, '-no-pie', *common, str(obj), '-ldl', *extra,
                        '-o', str(build / name)], check=True)
        executable = build / name
        executable.chmod(executable.stat().st_mode | 0o111)
    leaks = os.environ.get('BCAD_DETECT_LEAKS', '1')
    environment = dict(os.environ, ASAN_OPTIONS=f'detect_leaks={leaks}:halt_on_error=1',
                       UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1')
    subprocess.run([cxx, *flags, '-no-pie',
                    str(objects[sources.index(Path('src/W65C02.cpp'))]),
                    str(objects[sources.index(Path('tests/w65c02.cpp'))]),
                    '-o', str(build / 'w65c02')], check=True)
    vectors = build / 'w65c02-vectors.bin'
    vectors.write_bytes(gzip.decompress((root / 'tests/data/w65c02-vectors.bin.gz').read_bytes()))
    subprocess.run([str(build / 'w65c02')], check=True, env=environment, timeout=120)
    subprocess.run([str(build / 'w65c02'), '--corpus', str(vectors)],
                   check=True, env=environment, timeout=120)
    subprocess.run([str(build / 'prototyping')], check=True, env=environment, timeout=120)
    subprocess.run([str(build / 'routing')], check=True, env=environment, timeout=120)
    subprocess.run([str(build / 'integrated')], check=True, env=environment, timeout=120)
    subprocess.run([str(build / 'ux')], check=True, env=environment, timeout=120)
    subprocess.run([str(build / 'reliability'), str(root), str(build / 'bcad')],
                   check=True, env=environment, timeout=120)
    for scenario, circuit in [
            ('silicon/examples/bcad-nmos.bcadtest', 'silicon/examples/bcad-nmos.rs'),
            ('res/10_integrated/demo.bcadtest', 'res/10_integrated/demo.rs')]:
        subprocess.run([str(build / 'bcad'), '--test', scenario, circuit],
                       check=True, env=environment, timeout=120)
except (RuntimeError, subprocess.SubprocessError) as error:
    print(error, file=sys.stderr)
    sys.exit(1)
