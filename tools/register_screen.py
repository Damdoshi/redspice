#!/usr/bin/env python3
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def replace_once(path, old, new):
    p = ROOT / path
    s = p.read_text()
    if new in s:
        return
    if old not in s:
        raise SystemExit(f"Cannot patch {path}: anchor not found")
    p.write_text(s.replace(old, new, 1))


def add_once(path, anchor, text):
    p = ROOT / path
    s = p.read_text()
    if text.strip() in s:
        return
    if anchor not in s:
        raise SystemExit(f"Cannot patch {path}: anchor not found")
    p.write_text(s.replace(anchor, anchor + text, 1))


# Creation.
add_once("src/Create.cpp", '#include\t\t"Terminal.hpp"\n', '#include\t\t"VideoScreen.hpp"\n')
replace_once(
    "src/Create.cpp",
    '  if (type == "output")\n    return (new hbs::Output(timer, name, position));\n',
    '  if (type == "output")\n    return (new hbs::Output(timer, name, position));\n  if (type == "screen")\n    return (new hbs::VideoScreen(timer, name, value, position));\n'
)

# Treat screen as a simulated sink like output/terminal.
p = ROOT / "src/Circuit.cpp"
s = p.read_text()
old = 'type == "output" || type == "terminal"'
new = 'type == "output" || type == "terminal" || type == "screen"'
if old in s:
    s = s.replace(old, new)
p.write_text(s)

# Make saved files stable if some older tree maps type names explicitly.
add_once(
    "src/Circuit.cpp",
    '  if (type == "Terminal")\n    return ("terminal");\n',
    '  if (type == "screen")\n    return ("screen");\n'
)

# User creation list.
p = ROOT / "src/Circuit.cpp"
s = p.read_text()
if '"screen"' not in s.split('creatable_types = {', 1)[-1].split('};', 1)[0]:
    s = s.replace('"input", "output", "clock", "true", "false", "terminal",', '"input", "output", "screen", "clock", "true", "false", "terminal",', 1)
    p.write_text(s)

print("screen component registered")
