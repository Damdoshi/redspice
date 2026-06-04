#!/usr/bin/env python3
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

components = [
    "Memory8",
    "62256", "6264", "28C256", "28C64", "6522",
]
creatable = ["62256", "6264", "28C256", "28C64", "6522"]

create_cpp = ROOT / "src" / "Create.cpp"
circuit_cpp = ROOT / "src" / "Circuit.cpp"


def add_once_after(text: str, anchor: str, addition: str) -> str:
    if addition.strip() in text:
        return text
    if anchor not in text:
        raise SystemExit(f"Anchor not found: {anchor!r}")
    return text.replace(anchor, anchor + addition, 1)


def register_includes() -> None:
    text = create_cpp.read_text()
    missing = [c for c in components if f'"{c}.hpp"' not in text]
    if not missing:
        return
    addition = "".join(f'#include\t\t"{c}.hpp"\n' for c in missing)
    if '#include\t\t"2716.hpp"\n' in text:
        text = add_once_after(text, '#include\t\t"2716.hpp"\n', addition)
    else:
        lines = text.splitlines(True)
        pos = 0
        for i, line in enumerate(lines):
            if line.startswith('#include'):
                pos = i + 1
        lines.insert(pos, addition)
        text = "".join(lines)
    create_cpp.write_text(text)


def register_create_chain() -> None:
    text = create_cpp.read_text()
    calls = []
    for c in creatable:
        klass = f'CMP_{c}'
        if f'{klass}::Create' not in text:
            calls.append(
                f'  if (hbs::{klass}::TypeMatches(type))\n'
                f'    return (hbs::{klass}::Create(timer, type, name, value, position));\n'
            )
    if not calls:
        return
    addition = "".join(calls)
    marker = '  if (type == "terminal")\n'
    if marker not in text:
        marker = '  if (type == "wire" || type == "track")\n'
    if marker not in text:
        raise SystemExit("Could not find a safe creation insertion point in src/Create.cpp")
    text = text.replace(marker, addition + "\n" + marker, 1)
    create_cpp.write_text(text)


def register_creatable_types() -> None:
    text = circuit_cpp.read_text()
    missing = [c for c in creatable if f'"{c}"' not in text]
    if not missing:
        return
    addition = (
        '  const char *memory_io_types[] = {\n'
        '    "62256", "6264", "28C256", "28C64", "6522"\n'
        '  };\n\n'
        '  creatable_types.insert(creatable_types.end(), memory_io_types, memory_io_types + sizeof(memory_io_types) / sizeof(memory_io_types[0]));\n'
    )
    anchor = '  creatable_types.assign(types, types + sizeof(types) / sizeof(types[0]));\n'
    text = add_once_after(text, anchor, addition)
    circuit_cpp.write_text(text)


def main() -> None:
    register_includes()
    register_create_chain()
    register_creatable_types()
    print("Registered memory and VIA components in src/Create.cpp and src/Circuit.cpp")


if __name__ == "__main__":
    main()
