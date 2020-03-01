#!/usr/bin/env python3

import argparse
import os
import platform
import stat
import shutil
import subprocess
import sys
from subprocess import CalledProcessError

# project constants:
project_name = "Binbuff c++ implementation"
cmake_component = "binbuff"


parser = argparse.ArgumentParser(description=f'Configure {project_name}')
group = parser.add_mutually_exclusive_group()
group.add_argument('-r', '--reconfigure', action='store_true', help='force reconfiguration of the project (does not update delete googletest)')
group.add_argument('-c', '--clean', action='store_true', help='clean the build system and dependencies')
parser.add_argument('-b', '--build', type=str, choices=['Debug', 'Release'], help='build the project and create binaries')
parser.add_argument('-t', '--test', type=str, choices=['Debug', 'Release'], help='run the tests')

project_dir = os.getcwd()

def rm_readonly(action, name, exc):
    os.chmod(name, stat.S_IWRITE)
    os.remove(name)

def clone_googletest():
    show("cloning googletest repository", "INFO")
    try:
        subprocess.check_output(['git', 'clone', 'https://github.com/google/googletest.git'])
    except CalledProcessError as e:
        show(e.output, "ERROR")
        show("could not clone googletest repository, please make sure you are able to connect to the internet and have git installed.", "ERROR")
        exit(1)
    
    show("googletest cloned", "SUCCESS")
    
def run_cmake():
    show("generating project files", "INFO")
    os.mkdir('build')
    os.chdir('build')
    try:    
        if platform.system() == "Windows": 
            subprocess.run(['cmake', '..', '-Dgtest_force_shared_crt=ON'])
        else:
            subprocess.run(['cmake', '..'])
    except:
        show("could not execute cmake generate command, please make sure cmake is installed on this machine", "ERROR")
        exit(1)
    os.chdir(project_dir)
    show("project files generated", "SUCCESS")

def cmake_build(config):
    show(f"building project with {config} configuration", "INFO")
    os.chdir('build')
    if platform.system() == "Windows":
        subprocess.run(['cmake', '--build', '.', '--target', 'ALL_BUILD', '--config', config])
        subprocess.run(['cmake', f'-DCOMPONENT={cmake_component}', '-DBUILD_TYPE='+config, '-P', 'cmake_install.cmake'])
    else:
        subprocess.run(['cmake', '--build', '.'])
        subprocess.run(['cmake', f'-DCOMPONENT={cmake_component}', '-DBUILD_TYPE='+config, '-P', 'cmake_install.cmake'])
    os.chdir(project_dir)
    show(f"{config} build is complete", "SUCCESS")

def cmake_test(config):
    show(f"running tests with {config} configuration", "INFO")
    if not os.path.isdir('bin/' + config):
        show(config + " configuration is unavailable, building in " + config + " configuration now...", "WARNING")
        cmake_build(config)
    os.chdir('bin/' + config)
    if platform.system() not in os.listdir():
        show("could not find suitable tests to run", "ERROR")
        os.chdir(project_dir)
        return

    os.chdir(platform.system())
    test_file = [f for f in os.listdir() if 'test' in f][0]
    subprocess.run(f"{os.getcwd()}/{test_file}")
    os.chdir(project_dir)

def show(text, type):
    INFO = '\033[95m'
    SUCCESS = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    ERROR = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'

    COLOR = 0
    if type == "INFO":      COLOR = INFO
    if type == "SUCCESS":   COLOR = SUCCESS
    if type == "WARNING":   COLOR = WARNING
    if type == "ERROR":     COLOR = ERROR

    print(f"{COLOR}[{type}]:{ENDC} {BOLD}{text}{ENDC}")

if __name__ == '__main__':
    os.system('')
    args = parser.parse_args()

    build = os.path.isdir('build')
    googletest = os.path.isdir('googletest')
    bin = os.path.isdir('bin')

    if args.reconfigure or args.clean:
        if build: 
            shutil.rmtree('build')
            build = False
        if bin:
            shutil.rmtree('bin')
            bin = False

    if args.clean:
        if googletest: 
            shutil.rmtree('googletest', onerror=rm_readonly)
            googletest = False
        exit()
    
    if not bin:
        os.mkdir('bin')

    if not googletest:
        clone_googletest()
    
    if not build:
        run_cmake()
        cmake_build('Debug')
        cmake_test('Debug')

    if args.test:
        cmake_test(args.test)

    if args.build:
        cmake_build(args.build)
