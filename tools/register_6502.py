#!/usr/bin/env python3
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def insert_after(text, anchor, payload):
    if payload.strip() in text:
        return text
    if anchor not in text:
        raise SystemExit(f"Anchor not found: {anchor!r}")
    return text.replace(anchor, anchor + payload, 1)


def register_create():
    path = ROOT / "src" / "Create.cpp"
    text = path.read_text()
    text = insert_after(text, '#include\t\t"2716.hpp"\n', '#include\t\t"6502.hpp"\n')
    call = (
        '  if (hbs::CMP_6502::TypeMatches(type))\n'
        '    return (hbs::CMP_6502::Create(timer, type, name, value, position));\n'
    )
    if call not in text:
        marker = '  if (type == "2716")\n    return (new hbs::CMP_2716(timer, name, value, position));\n'
        if marker in text:
            text = text.replace(marker, marker + '\n' + call, 1)
        else:
            marker = '  if (type == "terminal")\n'
            text = text.replace(marker, call + '\n' + marker, 1)
    path.write_text(text)


def register_circuit():
    path = ROOT / "src" / "Circuit.cpp"
    text = path.read_text()
    for model in ('W65C02S', '6502'):
        if f'"{model}"' not in text:
            if '"2716"' not in text:
                raise SystemExit('Could not find a safe place to register the CPU.')
            text = text.replace('"2716"', f'"2716", "{model}"', 1)
    path.write_text(text)


register_create()
register_circuit()
print('W65C02S and legacy 6502 alias registered in src/Create.cpp and src/Circuit.cpp')
