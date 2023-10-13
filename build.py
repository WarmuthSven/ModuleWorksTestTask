#!/usr/bin/env python
import os
import sys
import subprocess

def cmake_not_found_error():
    print('ERROR: Could not start cmake. Is it installed? Is it available from console?')
    print('ERROR: You can install it from https://cmake.org/ and add it to PATH variable to make it available from console')
    print('ERROR: Alternatively you can invoke it yourself via cmake-gui and generate project files for your build system')
    sys.exit(-1)

def cmake_call(cmd, cwd='.', is_output_only_on_error=False):
    print(['cmake'] + cmd)
    sys.stdout.flush()
    if not is_output_only_on_error:
        try:
            rc = subprocess.call(['cmake'] + cmd, cwd=cwd)
        except FileNotFoundError as exc:
            if sys.platform == "win32":
                try:
                    rc = subprocess.call(['C:\\Program Files\\CMake\\bin\\cmake.exe'] + cmd, cwd=cwd)
                except FileNotFoundError as exc:
                    cmake_not_found_error()
            else:
                cmake_not_found_error()
        if rc != 0:
            print('ERROR: Cmake finished with returncode=' + str(rc))
            sys.exit(rc)

project_folder = os.path.dirname(__file__)
print('Building cmake project...')
build_dir =  os.path.join(project_folder, 'build')
os.makedirs(build_dir, exist_ok=True)
config = 'Release'
cmake_call(['../', '-DCMAKE_BUILD_TYPE=' + config], build_dir)
cmake_call(['--build', '.', '--target', 'cutSphereMove', '--config', config], build_dir)



