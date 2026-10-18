#!/usr/bin/env python3
"""Run the CPU tests without LibLapin. Optional: --klaus DIRECTORY."""
import argparse
import gzip
import os
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--klaus', type=Path, help='Directory containing the two Klaus bin_files images')
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
build = root / '.test-build'
build.mkdir(exist_ok=True)
executable = build / 'w65c02-standalone'
subprocess.run([os.environ.get('CXX', 'g++'), '-std=c++17', '-O2', '-Wall', '-Wextra',
                '-Wpedantic', '-I'+str(root/'include'), str(root/'src/W65C02.cpp'),
                str(root/'tests/w65c02.cpp'), '-o', str(executable)], check=True)
vectors = build / 'w65c02-vectors.bin'
vectors.write_bytes(gzip.decompress((root/'tests/data/w65c02-vectors.bin.gz').read_bytes()))
subprocess.run([str(executable)], check=True)
subprocess.run([str(executable), '--corpus', str(vectors)], check=True)
if args.klaus:
    for filename, success in [('6502_functional_test.bin', '3469'),
                              ('65C02_extended_opcodes_test.bin', '24f1')]:
        subprocess.run([str(executable), '--klaus', str(args.klaus/filename), success],
                       check=True, timeout=120)
