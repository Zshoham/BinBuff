#!/usr/bin/python3

import argparse
from multiprocessing import Process, Lock
import platform
import os
import sys
import shutil
import subprocess
import string
import re
import zipfile

ansi_escape = re.compile(r'\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])')
project_dir = os.getcwd()
is_windows = platform.system() == "Windows"
version = '0.1.0'

parser = argparse.ArgumentParser(description='Package BinBuff library for all languages')
parser.add_argument('-t', '--test', action='store_true', help="run tests before packaging")
parser.add_argument('-b', '--build', action='store_true', help="rebuild before packaging or testing")
parser.add_argument('-c', '--clean', action='store_true', help="clean all build files")


def show(text, type, print_lock):
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

    print_lock.acquire()
    print(f"{COLOR}[{type}]:{ENDC} {BOLD}{text}{ENDC}")
    print_lock.release()

def format_file(file_path):
    fd = open(file_path, 'r')
    data = fd.read()
    fd.close()
    fd = open(file_path, 'w')
    fd.write(ansi_escape.sub('', data))
    fd.close()

def c_pack(test, build, print_lock):

    c_log = open("release/log/c_log.txt", 'w')

    if build:
        c_build(print_lock, c_log)
    if test: 
        c_test(print_lock, c_log)
    
    show("packaging C implementation", "INFO", print_lock)

    if is_windows:
        if not os.path.isdir('CBinBuff/build/Release'):
            show("C - release build unavailable for packaging", "ERROR", print_lock)
            c_log.close()
            exit(1)

        shutil.copy('CBinBuff/build/Release/binbuff.lib', 'release/C/binbuff-' + version + '.lib')

    else:
        if not os.path.isdir('CBinBuff/build'):
            show("C - release build unavailable for packaging", "ERROR", print_lock)
            c_log.close()
            exit(1)

        shutil.copy('CBinBuff/build/libbinbuff.a', 'release/C/libbinbuff-' + version + '.a')
    
    shutil.copy('CBinBuff/binbuff.h', 'release/C')

    c_log.close()
    format_file('release/log/c_log.txt')
    exit(0)

def c_build(print_lock, log):
    show("building C implementation", "INFO", print_lock)

    os.chdir('CBinBuff')
    if not os.path.isdir('build'):
        os.mkdir('build')

    os.chdir('build')
    buildProcess = subprocess.run(['cmake', '..'], stdout=log)

    if not buildProcess.returncode == 0:
        show("C configuration failed", "ERROR", print_lock)
        os.chdir(project_dir)
        exit(1)

    buildProcess = subprocess.run(['cmake', '--build', '.', '--config', 'Release'], stdout=log)

    if not buildProcess.returncode == 0:
        show("C build failed", "ERROR", print_lock)
        os.chdir(project_dir)
        exit(1)
    
    os.chdir(project_dir)
    return

def c_test(print_lock, log):

    os.chdir('CBinBuff')
    if not os.path.isdir('build'):
        os.chdir(project_dir)
        c_build(print_lock, log)
        os.chdir('CBinBuff')

    os.chdir('build')
    if is_windows:
        if not os.path.isdir('Release'):
            os.chdir(project_dir)
            c_build(print_lock, log)
            os.chdir('CBinBuff')

        os.chdir('Release')
        show("testing C implementation", "INFO", print_lock)
        testProcess = subprocess.run(['binbuff_test.exe'], stdout=log)

    else:
        show("testing C implementation", "INFO", print_lock)
        testProcess = subprocess.run(['./binbuff_test'], stdout=log)

    if not testProcess.returncode == 0:
        show("c tests are failing", "ERROR", print_lock)
        os.chdir(project_dir)
        exit(1)
    
    os.chdir(project_dir)
    return


def cpp_pack(test, build, print_lock):

    cpp_log = open("release/log/cpp_log.txt", 'w')

    sys.stderr = cpp_log

    if build:
        cpp_build(print_lock, cpp_log)
    if test:
        cpp_test(print_lock, cpp_log)
    
    show("packaging Cpp implementation", "INFO", print_lock)

    if not os.path.isdir('CppBinBuff/bin/Release/' + platform.system()):
            show("Cpp - release build unavailable for packaging", "ERROR", print_lock)
            cpp_log.close()
            exit(1)

    if is_windows:
        shutil.copy('CppBinBuff/bin/Release/' + platform.system() + '/binbuff.lib', 'release/Cpp/binbuff-' + version + '.lib')
    else:
        shutil.copy('CppBinBuff/bin/Release/' + platform.system() + '/libbinbuff.a', 'release/Cpp/libbinbuff-' + version + '.a')

    shutil.copytree('CppBinBuff/include', 'release/Cpp/include')

    cpp_log.close()
    format_file("release/log/cpp_log.txt")
    exit(0)

def cpp_build(print_lock, log):
    show("building Cpp implementation", "INFO", print_lock)

    os.chdir('CppBinBuff')

    if is_windows:
        buildProcess = subprocess.run(['python', './configure.py', '-b', 'Release'], stdout=log, stderr=log)
    else:
        buildProcess = subprocess.run(['./configure.py', '-b', 'Release'], stdout=log, stderr=log)
    
    if not buildProcess.returncode == 0:
        show("Cpp build failed", "ERROR", print_lock)
        os.chdir(project_dir)
        exit(1)

    os.chdir(project_dir)
    return

def cpp_test(print_lock, log):
    show("testing Cpp implementation", "INFO", print_lock)        

    os.chdir('CppBinBuff')

    if is_windows:
        buildProcess = subprocess.run(['python', './configure.py', '-t', 'Release'], stdout=log, stderr=log)
    else:
        buildProcess = subprocess.run(['./configure.py', '-t', 'Release'], stdout=log, stderr=log)
    
    if not buildProcess.returncode == 0:
        show("Cpp tests are failing", "ERROR", print_lock)
        os.chdir(project_dir)
        exit(1)

    os.chdir(project_dir)
    return


def csharp_pack(test, build, print_lock):

    csharp_log = open("release/log/csharp_log.txt", 'w')

    if build:
        csharp_build(print_lock, csharp_log)
    if test:
        csharp_test(print_lock, csharp_log)

    show("packaging Csharp implementation", "INFO", print_lock)

    os.chdir('CsBinBuff/BinBuff')
    pack_dir = project_dir + "/release/Csharp"
    packProcess = subprocess.run(['dotnet', 'publish', '-c', 'Release', '-o', pack_dir, '/p:Version=' + version], stdout=csharp_log)
    os.chdir(project_dir)

    if not packProcess.returncode == 0:
        show("Csharp packaging failed (dotnet publish)", "ERROR", print_lock)
        csharp_log.close()
        exit(1)

    os.rename('release/Csharp/BinBuff.dll', 'release/Csharp/BinBuff-' + version +'.dll')
    os.rename('release/Csharp/BinBuff.pdb', 'release/Csharp/BinBuff-' + version +'.pdb')

    csharp_log.close()
    format_file("release/log/csharp_log.txt")

    exit(0)

def csharp_build(print_lock, log):
    show("building Csharp implementation", "INFO", print_lock)
    
    os.chdir('CsBinBuff/BinBuff')
    buildProcess = subprocess.run(['dotnet', 'build', '-c', 'Release'], stdout=log)

    if not buildProcess.returncode == 0:
        show("Csharp build failed", "ERROR", print_lock)
        os.chdir(project_dir)
        exit(1)

    os.chdir(project_dir)
    return

def csharp_test(print_lock, log):
    os.chdir('CsBinBuff/BinBuff.Test')
    buildProcess = subprocess.run(['dotnet', 'test'], stdout=log)

    show("testing Csharp implementation", "INFO", print_lock)

    if not buildProcess.returncode == 0:
        show("Csharp tests are failing", "ERROR", print_lock)
        os.chdir(project_dir)
        exit(1)

    os.chdir(project_dir)
    return


def java_pack(test, build, print_lock):

    java_log = open("release/log/java_log.txt", 'w')

    if build:
        java_build(print_lock, java_log)
    if test:
        java_test(print_lock, java_log)

    show("packaging Java implementation", "INFO", print_lock)

    if not os.path.isdir('JBinBuff/build/libs'):
        show("Java - build unavailable for packaging", "ERROR", print_lock)
        java_log.close()
        exit(1)
    
    shutil.copy('JBinBuff/build/libs/BinBuff.jar', 'release/Java/BinBuff-' + version + '.jar')

    java_log.close()
    format_file("release/log/java_log.txt")
    exit(0)

def java_build(print_lock, log):
    show("building Java implementation", "INFO", print_lock)

    os.chdir('JBinBuff')

    if is_windows:
        buildProcess = subprocess.run(['gradlew.bat', 'jar'], stdout=log)
    else:
        buildProcess = subprocess.run(['./gradlew', 'jar'], stdout=log)
    
    if not buildProcess.returncode == 0:
        show("Java build failed", "ERROR", print_lock)
        os.chdir(project_dir)
        exit(1)

    os.chdir(project_dir)
    return

def java_test(print_lock, log):

    if not os.path.isdir('JBinBuff/build/libs'):
        java_build(print_lock, log)

    show("testing Java implementation", "INFO", print_lock)

    os.chdir('JBinBuff')

    if is_windows:
        buildProcess = subprocess.run(['gradlew.bat', 'test'], stdout=log)
    else:
        buildProcess = subprocess.run(['./gradlew', 'test'], stdout=log)
    
    if not buildProcess.returncode == 0:
        show("Java tests are failing", "ERROR", print_lock)
        os.chdir(project_dir)
        exit(1)

    os.chdir(project_dir)
    return

def clean():
    if os.path.isdir('CBinBuff/build'):
        os.chdir('CBinBuff')
        shutil.rmtree('build')
        os.chdir(project_dir)

    os.chdir('CppBinBuff')
    if is_windows:
        buildProcess = subprocess.run(['python', './configure.py', '-c'])
    else:
        buildProcess = subprocess.run(['./configure.py', '-c'])
    os.chdir(project_dir)

    os.chdir('CsBinBuff/BinBuff')
    subprocess.run(['dotnet', 'clean'], stdout=subprocess.DEVNULL)
    os.chdir(project_dir)
    
    if os.path.isdir('JBinBuff/build'):
        os.chdir('JBinBuff')
        shutil.rmtree('build')
        os.chdir(project_dir)

if __name__ == '__main__':

    args = parser.parse_args()

    if os.path.isdir('release'):
        shutil.rmtree('release', ignore_errors=True)
    
    if args.clean:
        clean()
        exit(0)

    print_lock = Lock()

    os.mkdir('release')
    os.mkdir('release/log')
    os.mkdir('release/C')
    os.mkdir('release/Cpp')
    os.mkdir('release/Csharp')
    os.mkdir('release/Java')

    c_process = Process(name='c pack', target=c_pack, args=(args.test, args.build, print_lock))
    cpp_process = Process(name='cpp pack', target=cpp_pack, args=(args.test, args.build, print_lock))
    csharp_process = Process(name='csharp pack', target=csharp_pack, args=(args.test, args.build, print_lock))
    java_process = Process(name='java pack', target=java_pack, args=(args.test, args.build, print_lock))

    c_process.start()
    cpp_process.start()
    csharp_process.start()
    java_process.start()

    c_process.join()
    cpp_process.join()
    csharp_process.join()
    java_process.join()

    as_flag = lambda x: "SUCCESS" if x == 0 else "ERROR"

    print('--- SUMMERY ---')

    show('C packaging exited with code ' + str(c_process.exitcode), as_flag(c_process.exitcode), print_lock)
    show('Cpp packaging exited with code ' + str(cpp_process.exitcode), as_flag(cpp_process.exitcode), print_lock)
    show('Csharp packaging exited with code ' + str(csharp_process.exitcode), as_flag(csharp_process.exitcode), print_lock)
    show('Java packaging exited with code ' + str(java_process.exitcode), as_flag(java_process.exitcode), print_lock)

    os.mkdir("release/ar")
    os.chdir("release")
    for f in ["C", "Cpp", "Csharp", "Java"]:
        shutil.copytree(f, "ar/" + f)
        
    shutil.make_archive("BinBuff-" + platform.system() + "-" + version, 'zip', "ar")
    shutil.rmtree("ar")