Import("env")
import subprocess
import os
import shutil

def build_ui(source, target, env):
    print("=" * 50)
    print("Building UI...")
    print("=" * 50)

    project_dir = env.get("PROJECT_DIR")
    ui_src_dir = os.path.join(project_dir, "ui")
    build_dir = os.path.join(project_dir, "data-build")
    data_dir = os.path.join(project_dir, "data")

    if not os.path.exists(ui_src_dir):
        print("Warning: /ui directory not found")
        return
    
    print("Running npm build...")
    result = subprocess.run(
        ["npm", "run", "build"],
        cwd=ui_src_dir,
        capture_output=True,
        text=True,
        check=True
    )
    print(result.stdout)
    print("UI built successfully!")

    # Clear data folder
    if os.path.exists(data_dir):
        for item in os.listdir(data_dir):
            item_path = os.path.join(data_dir, item)
            if os.path.isfile(item_path):
                os.remove(item_path)
            elif os.path.isdir(item_path):
                shutil.rmtree(item_path)
    
    # Copy built files from data-build to data
    for item in os.listdir(build_dir):
        src = os.path.join(build_dir, item)
        dst = os.path.join(data_dir, item)
        if os.path.isdir(src):
            shutil.copytree(src, dst)
        else:
            shutil.copy2(src, dst)
    
    print(f"Deployment files copied to /data folder")

env.AddPreAction("uploadfs", build_ui)
env.AddPreAction("upload", build_ui)
