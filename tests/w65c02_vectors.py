#!/usr/bin/env python3
"""Encode SingleStepTests JSON as an endian-independent stream for w65c02.cpp.

Usage: w65c02_vectors.py DIRECTORY OUTPUT [--limit N]
The $5C reference is excluded: WDC specifies eight cycles; SST models four.
--wdc-bus applies the documented dummy-read corrections in docs/W65C02S.md.
"""
import argparse
import json
from pathlib import Path
import struct

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('directory', type=Path)
parser.add_argument('output', type=Path)
parser.add_argument('--limit', type=int)
parser.add_argument('--wdc-bus', action='store_true')
args = parser.parse_args()

def regs(r):
    return struct.pack('<H5B', *(r[k] for k in ('pc', 's', 'a', 'x', 'y', 'p')))

def memory(r):
    return struct.pack('<H', len(r)) + b''.join(struct.pack('<HB', a, v) for a, v in r)

count = 0
adjusted = 0
with args.output.open('wb') as out:
    for op in range(256):
        if op in (0x5c, 0xcb, 0xdb):
            continue
        source = args.directory / f'{op:02x}.json'
        if not source.exists():
            continue
        tests = json.loads(source.read_text())
        for t in tests[:args.limit]:
            cycles = [c[:] for c in t['cycles']]
            if args.wdc_bus:
                before = [c[:] for c in cycles]
                r = t['initial']
                ram = dict(r['ram'])
                def read(address):
                    address &= 65535
                    return [address, ram.get(address, 0), 'read']
                # WDC reads the last instruction byte while indexing zero page.
                if (op & 31) in (0x01, 0x15, 0x16) or op in (0x34,0x54,0x74,0x94,0x96,0xb4,0xb6,0xd4,0xf4):
                    cycles[2] = read(r['pc'] + 1)
                if op in (0x99,0x9d,0x9e,0xde,0xfe):
                    base = ram[(r['pc']+1)&65535] | ram[(r['pc']+2)&65535] << 8
                    ea = (base + r['y' if op == 0x99 else 'x']) & 65535
                    if base & 0xff00 == ea & 0xff00:
                        cycles[3] = read(ea)
                if op == 0x91:
                    cycles[4] = read((ram[(r['pc']+1)&65535] + 1) & 255)
                if op == 0x6c:
                    cycles[3:6] = [read(r['pc']+2), cycles[3], cycles[4]]
                if op == 0x7c:
                    cycles[3] = read(r['pc']+2)
                if op in (0x10,0x30,0x50,0x70,0x80,0x90,0xb0,0xd0,0xf0) and len(cycles)==4:
                    cycles[3] = read(r['pc']+2)
                if op in (0xdc,0xfc):
                    cycles[3] = read(ram[(r['pc']+1)&65535] | ram[(r['pc']+2)&65535] << 8)
                adjusted += cycles != before
            name = t['name'].encode('ascii')
            out.write(bytes([len(name)]) + name + regs(t['initial']) + memory(t['initial']['ram']))
            out.write(bytes([len(cycles)]))
            for a, v, kind in cycles:
                out.write(struct.pack('<HBB', a, v, kind == 'read'))
            out.write(regs(t['final']) + memory(t['final']['ram']))
            count += 1
print(f'Encoded {count} tests ({adjusted} with WDC dummy-read corrections) into {args.output}')
