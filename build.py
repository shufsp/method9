import os
import platform
import subprocess

if platform.system() == "Windows":
    build_command = "mingw32-make"  # For Windows with Mingw
else:
    build_command = "make"  # Default for Linux/macOS

if not os.path.exists("build"):
    os.makedirs("build")

os.chdir("build")

cmake_result = subprocess.run(["cmake", ".."])

if cmake_result.returncode != 0:
    print("CMake configuration failed, invalid CMakeLists.txt?")
    exit(1)

build_result = subprocess.run([build_command])

if build_result.returncode != 0:
    print("Build failed")
else:
    print("Build SUCCESS")
