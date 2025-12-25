#!/usr/bin/env python3
import time, pathlib, subprocess, sys

SCHEMA_DIR = pathlib.Path(".paramgen.toml").read_text().split("schema_dir =")[1].split("\n")[0].strip().strip('"')
GEN_CMD = ["python", "tools/param-gen/paramgen.py", "gen"]

def snapshot():
    return {p: p.stat().st_mtime for p in pathlib.Path(SCHEMA_DIR).glob("*.toml")}

last = snapshot()
print("🔁 Watching parameter schemas...")

while True:
    time.sleep(0.5)
    now = snapshot()
    if now != last:
        print("⚙️  Regenerating parameters...")
        subprocess.run(GEN_CMD)
        last = now
