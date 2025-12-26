#!/usr/bin/env python3

import tomllib, pathlib, sys, re

CONFIG = pathlib.Path(".paramgen.toml")

CPP_TYPES = { "float": "float", "int": "int", "bool": "bool" }
TS_TYPES  = { "float": "number", "int": "number", "bool": "boolean" }

def pascal_case(name: str) -> str:
    # Preserve original casing when no separators are present.
    if not re.search(r'[_\-\s]+', name):
        return name

    parts = re.split(r'[_\-\s]+', name)
    out = []
    for p in parts:
        if not p:
            continue
        # Uppercase first char, keep the rest as-is to avoid lowercasing.
        out.append(p[0].upper() + p[1:])
    return ''.join(out)

def identifier(name: str) -> str:
    s = re.sub(r'[^A-Za-z0-9_]+', '_', name)
    if not s:
        s = "param"
    if re.match(r'^[0-9]', s):
        s = "_" + s
    return s

def param_var_name(param_id: str) -> str:
    return param_id.split(".")[-1]

def enum_type_name(param_name: str) -> str:
    return pascal_case(identifier(param_var_name(param_name))) + "Choice"

def enum_member_name(choice: str) -> str:
    parts = re.split(r'[^A-Za-z0-9]+', str(choice))
    parts = [p for p in parts if p]
    if not parts:
        base = "Choice"
    else:
        base = ''.join(p[0].upper() + p[1:] for p in parts)
    if not re.match(r'^[A-Za-z_]', base):
        base = "Choice" + base
    return base

def unique_names(names):
    used = {}
    out = []
    for n in names:
        count = used.get(n, 0)
        if count == 0:
            out.append(n)
        else:
            out.append(f"{n}{count + 1}")
        used[n] = count + 1
    return out

def usage():
    print("Usage:")
    print("  paramgen setup")
    print("  paramgen gen")
    print("  paramgen gen --filename=Name")
    sys.exit(1)

def collect_param_tables(data):
    params = {}

    def walk(obj, prefix=""):
        if not isinstance(obj, dict):
            return

        if "type" in obj:
            params[prefix] = obj
            return

        for k, v in obj.items():
            name = k if not prefix else f"{prefix}.{k}"
            walk(v, name)

    for k, v in data.items():
        walk(v, k)

    return params

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
        try:
            data = tomllib.loads(schema_file.read_text())
        except tomllib.TOMLDecodeError as e:
            print(f"Skipping (invalid TOML): {schema_file} ({e})")
            continue

        params = collect_param_tables(data)
        if not params:
            print(f"Skipping (no parameter tables): {schema_file}")
            continue

        namespaces = set()
        invalid_ids = []
        for param_id in params.keys():
            if "." not in param_id:
                invalid_ids.append(param_id)
                continue
            namespaces.add(param_id.split(".", 1)[0])

        if invalid_ids:
            print(f"Skipping (invalid param ids; expected at least one namespace dot): {schema_file}")
            for pid in invalid_ids:
                print(f"  - {pid}")
            continue

        if len(namespaces) != 1:
            print(f"Skipping (expected exactly one namespace): {schema_file}")
            continue

        def get_meta(v):
            meta_fields = []
            if "label" in v: meta_fields.append(f'label: "{v["label"]}"')
            if "unit"  in v: meta_fields.append(f'unit: "{v["unit"]}"')
            if "ui"    in v: meta_fields.append(f'ui: "{v["ui"]}"')
            if "step"  in v: meta_fields.append(f'step: {v["step"]}')
            return "{ " + ", ".join(meta_fields) + " }" if meta_fields else "undefined"

        def validate_param(k, v):
            if not isinstance(v, dict):
                print(f"Skipping param '{k}' (not a table) in {schema_file}")
                return None
            if "type" not in v:
                print(f"Skipping param '{k}' (missing type) in {schema_file}")
                return None
            t = v["type"]
            if t == "choice":
                choices = v.get("choices")
                if not isinstance(choices, list) or not choices:
                    print(f"Skipping param '{k}' (missing choices) in {schema_file}")
                    return None
                if any(not isinstance(c, str) for c in choices):
                    print(f"Skipping param '{k}' (choices must be strings) in {schema_file}")
                    return None
                if "default" not in v:
                    v["default"] = choices[0]
                return v
            if t not in CPP_TYPES:
                print(f"Skipping param '{k}' (unsupported type '{t}') in {schema_file}")
                return None
            for req in ("default", "min", "max"):
                if req not in v:
                    print(f"Skipping param '{k}' (missing {req}) in {schema_file}")
                    return None
            return v

        validated_params = []
        for k, v in params.items():
            v = validate_param(k, v)
            if v is not None:
                validated_params.append((k, v))

        has_choice = any(v["type"] == "choice" for _, v in validated_params)

        # ---------------- C++ ----------------

        cpp_includes = [
            "#pragma once",
            '#include "../ParameterUtils/AudioParameter.h"',
            '#include "../ParameterUtils/ParamGenAccess.h"',
        ]
        if has_choice:
            cpp_includes.append('#include "../ParameterUtils/ChoiceParameter.h"')

        cpp = cpp_includes + [
            "",
            f"class {name} {{",
            "public:",
            f"    static {name}& instance() {{",
            f"        static {name} inst;",
            f"        return inst;",
            f"    }}",
            "",
            f"    {name}(const {name}&) = delete;",
            f"    {name}& operator=(const {name}&) = delete;",
            f"    {name}({name}&&) = delete;",
            f"    {name}& operator=({name}&&) = delete;",
            "",
            "private:",
            f"    {name}() = default;",
            "public:"
        ]

        ts = [
            'import { AudioParameter } from "./AudioParameter";',
        ]
        if has_choice:
            ts.append('import { ChoiceParameter } from "./ChoiceParameter";')
        ts.append("")

        def ts_string_list(items):
            return ", ".join([f'"{c}"' for c in items])

        ts.append(f"export class {name} {{")
        ts.append(f"  private static _instance: {name} | null = null;")
        ts.append("")
        ts.append(f"  static instance(): {name} {{")
        ts.append(f"    if (!{name}._instance) {{")
        ts.append(f"      {name}._instance = new {name}();")
        ts.append(f"    }}")
        ts.append(f"    return {name}._instance;")
        ts.append(f"  }}")
        ts.append("")
        ts.append(f"  private constructor() {{}}")
        ts.append("")

        for k, v in validated_params:
            if v["type"] != "choice":
                continue
            choice_const = pascal_case(identifier(param_var_name(k))) + "Choices"
            choices = v["choices"]
            ts.append(
                f"  static readonly {choice_const} = [{ts_string_list(choices)}] as const;"
            )
        if has_choice:
            ts.append("")

        for k, v in validated_params:
            if v["type"] == "choice":
                choices = v["choices"]
                member_names = unique_names([enum_member_name(c) for c in choices])
                enum_name = enum_type_name(k)
                choices_name = f"{identifier(param_var_name(k))}_choices"
                choice_const = pascal_case(identifier(param_var_name(k))) + "Choices"

                default = v.get("default", choices[0])
                if isinstance(default, str):
                    try:
                        default_idx = choices.index(default)
                    except ValueError:
                        default_idx = 0
                elif isinstance(default, int):
                    default_idx = default
                else:
                    default_idx = 0

                if default_idx < 0 or default_idx >= len(choices):
                    default_idx = 0

                cpp.append(f"    enum class {enum_name} : int {{")
                for idx, member in enumerate(member_names):
                    cpp.append(f"        {member} = {idx},")
                cpp.append("    };")
                cpp.append("")
                cpp.append(f"    static constexpr const char* {choices_name}[] = {{")
                for c in choices:
                    cpp.append(f'        "{c}",')
                cpp.append("    };")
                cpp.append("")
                cpp.append(
                    f"    ChoiceParameter<{enum_name}, {len(choices)}> {identifier(param_var_name(k))} = "
                    f'{{"{k}", {enum_name}::{member_names[default_idx]}, {choices_name}}};'
                )

                ts_default = default_idx
                ts.append(
                    f"  {identifier(param_var_name(k))} = ChoiceParameter.__make"
                    f'("{k}", {ts_default}, 0, {len(choices) - 1}, "int", {get_meta(v)}, {name}.{choice_const});'
                )
                continue

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
                f'    AudioParameter<{t}> {identifier(param_var_name(k))} = '
                f'__ParamGenAccess::make<{t}>("{k}", {d}{suf}, {mn}{suf}, {mx}{suf});'
            )

            def ts_value(x):
                if isinstance(x, bool):
                    return "1" if x else "0"
                return str(x)

            d  = ts_value(v["default"])
            mn = ts_value(v["min"])
            mx = ts_value(v["max"])

            ts.append(
                f'  {identifier(param_var_name(k))} = AudioParameter.__make<{TS_TYPES[v["type"]]}>'
                f'("{k}", {d}, {mn}, {mx}, "{v["type"]}", {get_meta(v)});'
            )

        cpp.append("};")
        ts.append("}")

        (CPP / f"{name}.h").write_text("\n".join(cpp))
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
