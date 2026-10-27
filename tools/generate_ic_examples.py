#!/usr/bin/env python3
"""Generate portable, nested full-custom NMOS examples for the unified editor."""
from pathlib import Path
import json
import shlex

root = Path(__file__).resolve().parents[1]
destination = root / 'res' / '10_integrated'
destination.mkdir(parents=True, exist_ok=True)

def quote(value):
    return json.dumps(value, ensure_ascii=False)

class Design:
    def __init__(self, title):
        self.title, self.items, self.metadata, self.pins = title, [], [], []

    def add(self, kind, name='', x=0, y=0, w=1, h=1, layer='metal1'):
        number = len(self.items) + 1
        self.items.append(f'item {number} {kind} {layer} {x} {y} {w} {h} 0 0 10000 0 0 0 {quote(name)} ""')
        return number

    def shape(self, layer, x, y, w=1, h=1):
        self.add('shape', x=x, y=y, w=w, h=h, layer=layer)

    def wire(self, layer, a, b):
        assert a[0] == b[0] or a[1] == b[1]
        self.shape(layer, min(a[0], b[0]), min(a[1], b[1]), abs(a[0]-b[0])+1, abs(a[1]-b[1])+1)

    def instance(self, name, source, x=0, y=0):
        number = self.add('instance', name, x, y)
        self.metadata.append(f'instance {number} 0 {quote(source)}')

    def contact(self, kind, name, x, y, pin):
        number = self.add(kind, name, x, y)
        self.pins.append(f'pin {number} {pin}')

    def save(self, name, package='', total=0):
        text = ['BCAD_IC 2', 'tech nmos', 'title '+quote(self.title), f'package {quote(package)} {total} 8']
        text += self.items + self.metadata + self.pins + ['end '+str(len(self.items)), '']
        (destination/name).write_text('\n'.join(text))

# Reuse the actual NMOS layout shipped with the first editor. Only its four
# external contacts become internal named terminals; no transistor is replaced.
cell = Design('Cellule inverseur NMOS - sans contact externe ni boitier')
template = root / 'silicon' / 'examples' / 'nmos-inverter.rsi'
for line in template.read_text().splitlines():
    row = shlex.split(line)
    if not row or row[0] != 'item':
        continue
    if row[2] in ('input', 'output', 'inout', 'supply', 'ground'):
        row[2] = 'terminal'
    row[1] = str(len(cell.items)+1)
    cell.items.append(' '.join(row[:14])+f' {quote(row[14])} {quote(row[15])}')
cell.save('cell-inverter.rsi')

two = Design('Deux inverseurs : une puce composee de cellules reutilisables')
two.instance('first', 'cell-inverter.rsi')
two.instance('second', 'cell-inverter.rsi', 30, 0)
for a,b in [((0,1),(0,-12)),((0,-12),(35,-12)),((35,-12),(35,-3))]:
    two.wire('metal2', a, b)
for x,y in [(0,1),(35,-3),(10,1),(40,1)]:
    two.shape('via', x, y)
two.wire('metal1', (0,-7), (30,-7))
two.wire('metal2', (10,1), (40,1))
for kind,name,x,y,pin in [('input','A',5,-3,1),('supply','VCC',0,-7,2),('output','Y',30,1,3),('ground','GND',10,1,4)]:
    two.contact(kind,name,x,y,pin)
two.save('buffer2.rsi', 'DIP', 4)

four = Design('Quatre inverseurs : cellules dans des cellules, boitier DIP8')
four.instance('left', 'buffer2.rsi')
four.instance('right', 'buffer2.rsi', 60, 0)
for a,b in [((30,1),(30,6)),((30,6),(65,6)),((65,6),(65,-3)),((0,-7),(60,-7)),((10,1),(10,12)),((10,12),(70,12)),((70,12),(70,1))]:
    four.wire('metal1', a, b)
for kind,name,x,y,pin in [('input','A',5,-3,1),('supply','VCC',0,-7,8),('output','Y',90,1,3),('ground','GND',10,1,4)]:
    four.contact(kind,name,x,y,pin)
four.save('buffer4.rsi', 'DIP', 8)

(destination/'demo.rs').write_text('''# Deux instances du meme CI, chacune avec son etat interne.
.chipsets:
input a(0) [-12,-5]
input b(1) [-12,8]
true vcc [-12,-10]
false gnd [-12,14]
silicon first(buffer4.rsi) [0,-5]
silicon second(buffer4.rsi) [0,8]
output y_a [15,-3]
output y_b [15,10]
.links:
a:1 first:1
b:1 second:1
vcc:1 first:8
vcc:1 second:8
gnd:1 first:4
gnd:1 second:4
first:3 y_a:1
second:3 y_b:1
''')
(destination/'demo.bcadtest').write_text('''tick
expect y_a 0
expect y_b 1
set a 1
set b 0
tick
expect y_a 1
expect y_b 0
''')
print('Generated cell-inverter.rsi, buffer2.rsi, buffer4.rsi and demo.rs')
