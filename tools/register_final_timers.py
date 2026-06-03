#!/usr/bin/env python3
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

components = [
    "4046", "4047", "4060", "4098",
    "4521", "4536", "4538", "4541",
]

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
    if '#include\t\t"4040.hpp"\n' in text:
        text = add_once_after(text, '#include\t\t"4040.hpp"\n', addition)
    else:
        marker = '#include'
        lines = text.splitlines(True)
        pos = 0
        for i, line in enumerate(lines):
            if line.startswith(marker):
                pos = i + 1
        lines.insert(pos, addition)
        text = "".join(lines)
    create_cpp.write_text(text)


def register_create_chain() -> None:
    text = create_cpp.read_text()
    missing = [c for c in components if f'CMP_{c}::Create' not in text]
    if not missing:
        return
    addition = "".join(
        f'  if ((tmp = hbs::CMP_{c}::Create(timer, type, name, value, position)) != NULL)\n'
        f'    return (tmp);\n'
        for c in missing
    )
    anchor = (
        '  if ((tmp = hbs::CMP_4040::Create(timer, type, name, value, position)) != NULL)\n'
        '    return (tmp);\n'
    )
    if anchor in text:
        text = add_once_after(text, anchor, addition)
    else:
        legacy = '  if (type == "4040")\n    return (new hbs::CMP_4040(timer, name, position));\n'
        if legacy not in text:
            raise SystemExit("Could not find a 4040 creation anchor in src/Create.cpp")
        legacy_addition = "".join(
            f'  if (type == "{c}")\n    return (new hbs::CMP_{c}(timer, name, position));\n'
            for c in missing
        )
        text = add_once_after(text, legacy, legacy_addition)
    create_cpp.write_text(text)


def register_creatable_types() -> None:
    text = circuit_cpp.read_text()
    if all(f'"{c}"' in text for c in components):
        return
    addition = (
        '  const char *final_timer_types[] = {\n'
        '    "4046", "4047", "4060", "4098",\n'
        '    "4521", "4536", "4538", "4541"\n'
        '  };\n\n'
        '  creatable_types.insert(creatable_types.end(), final_timer_types, final_timer_types + sizeof(final_timer_types) / sizeof(final_timer_types[0]));\n'
    )
    anchor = '  creatable_types.assign(types, types + sizeof(types) / sizeof(types[0]));\n'
    text = add_once_after(text, anchor, addition)
    circuit_cpp.write_text(text)


def main() -> None:
    register_includes()
    register_create_chain()
    register_creatable_types()
    print("Registered final timer/oscillator components in src/Create.cpp and src/Circuit.cpp")


if __name__ == "__main__":
    main()
