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
    if '"6502"' in text:
        path.write_text(text)
        return
    if '"2716", "wire"' in text:
        text = text.replace('"2716", "wire"', '"2716", "6502", "wire"', 1)
    elif '"2716"' in text:
        text = text.replace('"2716"', '"2716", "6502"', 1)
    else:
        raise SystemExit('Could not find a safe place to insert "6502" in creatable_types.')
    path.write_text(text)


register_create()
register_circuit()
print('6502 registered in src/Create.cpp and src/Circuit.cpp')
