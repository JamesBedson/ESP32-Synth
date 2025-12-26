#!/usr/bin/env python3
import time, pathlib, subprocess, sys

CONFIG = pathlib.Path(".paramgen.toml")
SCHEMA_DIR = CONFIG.read_text().split("schema_dir =")[1].split("\n")[0].strip().strip('"')
GEN_SCRIPT = pathlib.Path("tools/param-gen/paramgen.py")
GEN_CMD = ["python", str(GEN_SCRIPT), "gen"]

def snapshot():
    files = list(pathlib.Path(SCHEMA_DIR).glob("*.toml"))
    files.extend([CONFIG, GEN_SCRIPT])
    return {p: p.stat().st_mtime for p in files if p.exists()}

last = snapshot()
print("🔁 Watching parameter schemas...")

while True:
    time.sleep(0.5)
    now = snapshot()
    if now != last:
        print("⚙️  Regenerating parameters...")
        subprocess.run(GEN_CMD)
        last = now
