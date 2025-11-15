#!/usr/bin/env python3
import os

def replace_in_file(filename, placeholder, value):
    with open(filename, "r", encoding="utf-8") as f:
        content = f.read()
    content = content.replace(placeholder, value)
    with open(filename, "w", encoding="utf-8") as f:
        f.write(content)

def main():
    project_name = input("Enter your project name: ").strip()
    if not project_name:
        print("Project name cannot be empty.")
        return

    replace_in_file("CMakeLists.txt", "{{PROJECT_NAME}}", project_name)
    replace_in_file("src/Utils.cpp", "{{PROJECT_NAME}}", project_name)
    replace_in_file("src/Renderer.cpp", "{{PROJECT_NAME}}", project_name)
    replace_in_file("README.md", "{{PROJECT_NAME}}", project_name)
    replace_in_file("build-appimage.sh", "{{PROJECT_NAME}}", project_name)
    replace_in_file(".github/workflows/release.yml", "{{PROJECT_NAME}}", project_name)

    os.rename("resources/project.desktop.in", f'resources/{project_name}.desktop.in')
    os.rename("resources/project.png", f'resources/{project_name}.png')
    os.rename("resources/project.rc.in", f'resources/{project_name}.rc.in')

    print(f"Project configured as '{project_name}'")

if __name__ == "__main__":
    main()
