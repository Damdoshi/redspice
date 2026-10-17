#!/usr/bin/env python3
"""Compile and run sanitised tests using a test-only LibLapin surface."""
import concurrent.futures
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
         '-Itests/support', '-Iinclude', '-Iinclude/stdcmp', '-Iinclude/vircmp', '-pthread']
sources = sorted(p for folder in ('src', 'screen', 'stdcmp', 'vircmp')
                 for p in Path(folder).glob('*.cpp'))
sources += [Path('main.cpp'), Path('tests/reliability.cpp')]
headers = [p for folder in ('include', 'tests/support') for p in Path(folder).rglob('*') if p.is_file()]
flag_file = build / 'flags.txt'
flag_text = '\n'.join([cxx, *flags])
if not flag_file.exists() or flag_file.read_text() != flag_text:
    flag_file.write_text(flag_text)
    for obj in build.glob('*.o'):
        obj.unlink()
stamp = max(p.stat().st_mtime for p in headers)

def compile_one(source):
    obj = build / (str(source).replace('/', '_') + '.o')
    if not obj.exists() or obj.stat().st_size == 0 or obj.stat().st_mtime < max(stamp, source.stat().st_mtime):
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
              if source not in (Path('main.cpp'), Path('tests/reliability.cpp'))]
    for name, entry, extra in [
            ('bcad', 'main.cpp', []),
            ('reliability', 'tests/reliability.cpp',
             ['-Wl,--wrap=write,--wrap=fsync,--wrap=close,--wrap=rename,--wrap=link'])]:
        obj = objects[sources.index(Path(entry))]
        subprocess.run([cxx, *flags, '-no-pie', *common, str(obj), '-ldl', *extra,
                        '-o', str(build / name)], check=True)
        executable = build / name
        executable.chmod(executable.stat().st_mode | 0o111)
    leaks = os.environ.get('BCAD_DETECT_LEAKS', '1')
    environment = dict(os.environ, ASAN_OPTIONS=f'detect_leaks={leaks}:halt_on_error=1',
                       UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1')
    subprocess.run([str(build / 'reliability'), str(root), str(build / 'bcad')],
                   check=True, env=environment, timeout=120)
except (RuntimeError, subprocess.SubprocessError) as error:
    print(error, file=sys.stderr)
    sys.exit(1)
