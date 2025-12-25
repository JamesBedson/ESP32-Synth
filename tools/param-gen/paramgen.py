#!/usr/bin/env python3

import tomllib, pathlib, sys, re

CONFIG = pathlib.Path(".paramgen.toml")

CPP_TYPES = { "float": "float", "int": "int", "bool": "bool" }
TS_TYPES  = { "float": "number", "int": "number", "bool": "boolean" }

def pascal_case(name: str) -> str:
    parts = re.split(r'[_\-\s]+', name)
    return ''.join(p.capitalize() for p in parts if p)

def usage():
    print("Usage:")
    print("  paramgen setup")
    print("  paramgen gen")
    print("  paramgen gen --filename=Name")
    sys.exit(1)

# ------------------- Setup -------------------

def setup():
    schema = input("Where are your parameter schemas? ").strip()
    cpp    = input("Where should C++ headers go? ").strip()
    ts     = input("Where should TS types go? ").strip()

    CONFIG.write_text(
        f'schema_dir = "{schema}"\n'
        f'cpp_out = "{cpp}"\n'
        f'ts_out = "{ts}"\n'
    )

    print("Parameter generator configured.")

# ------------------- Generate -------------------

def generate(filename=None):
    if not CONFIG.exists():
        sys.exit("Run 'make paramgen setup' first.")

    cfg = tomllib.loads(CONFIG.read_text())

    SRC = pathlib.Path(cfg["schema_dir"])
    CPP = pathlib.Path(cfg["cpp_out"])
    TS  = pathlib.Path(cfg["ts_out"])

    if not SRC.is_dir():
        sys.exit(f"Schema directory not found: {SRC}")

    CPP.mkdir(parents=True, exist_ok=True)
    TS.mkdir(parents=True, exist_ok=True)

    found = False

    for schema_file in SRC.glob("*.toml"):
        raw = schema_file.stem
        name = pascal_case(raw)

        if filename and name != filename:
            continue

        found = True
        data = tomllib.loads(schema_file.read_text())

        if "params" not in data:
            sys.exit(f"Missing [params] table in {schema_file}")

        params = data["params"]

        # ---------------- C++ ----------------

        cpp = [
            "#pragma once",
            '#include "AudioParameter.h"',
            '#include "ParamGenAccess.h"',
            "",
            f"struct {name} {{"
        ]

        for k, v in params.items():
            t = CPP_TYPES[v["type"]]

            def cpp_value(x):
                if isinstance(x, bool):
                    return "true" if x else "false"
                return str(x)

            d  = cpp_value(v["default"])
            mn = cpp_value(v["min"])
            mx = cpp_value(v["max"])

            suf = "f" if t == "float" else ""

            cpp.append(
                f'    AudioParameter<{t}> {k} = '
                f'__ParamGenAccess::make<{t}>("{k}", {d}{suf}, {mn}{suf}, {mx}{suf});'
            )

        cpp.append("};")

        (CPP / f"{name}.h").write_text("\n".join(cpp))

        # ---------------- TS ----------------

        def ts_value(x):
            if isinstance(x, bool):
                return "1" if x else "0"
            return str(x)

        ts = [
            'import { AudioParameter } from "./AudioParameter";',
            "",
            f"export class {name} {{"
        ]

        for k, v in params.items():
            d  = ts_value(v["default"])
            mn = ts_value(v["min"])
            mx = ts_value(v["max"])

            meta_fields = []

            if "label" in v: meta_fields.append(f'label: "{v["label"]}"')
            if "unit"  in v: meta_fields.append(f'unit: "{v["unit"]}"')
            if "ui"    in v: meta_fields.append(f'ui: "{v["ui"]}"')
            if "step"  in v: meta_fields.append(f'step: {v["step"]}')

            meta_str = "{ " + ", ".join(meta_fields) + " }" if meta_fields else "undefined"

            ts.append(
                f'  {k} = AudioParameter.__make<{TS_TYPES[v["type"]]}>'
                f'("{k}", {d}, {mn}, {mx}, "{v["type"]}", {meta_str});'
            )

        ts.append("}")

        (TS / f"{name}.ts").write_text("\n".join(ts))

        print(f"Generated: {name}")

    if filename and not found:
        sys.exit(f"Schema '{filename}.toml' not found in {SRC}")

# ------------------- Entry -------------------

if len(sys.argv) < 2:
    usage()

cmd = sys.argv[1]

if cmd == "setup":
    setup()

elif cmd == "gen":
    filename = None
    if len(sys.argv) == 3 and sys.argv[2].startswith("--filename="):
        filename = sys.argv[2].split("=", 1)[1]
    generate(filename)

else:
    usage()
