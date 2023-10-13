#!/usr/bin/env python
from genericpath import isfile
import os
from re import L
import sys
import subprocess
import time

if sys.platform == "win32":
    import ctypes
    from ctypes import wintypes

    SIZE_T = ctypes.c_size_t

    class PROCESS_MEMORY_COUNTERS_EX(ctypes.Structure):
        _fields_ = [
            ("cb", wintypes.DWORD),
            ("PageFaultCount", wintypes.DWORD),
            ("PeakWorkingSetSize", SIZE_T),
            ("WorkingSetSize", SIZE_T),
            ("QuotaPeakPagedPoolUsage", SIZE_T),
            ("QuotaPagedPoolUsage", SIZE_T),
            ("QuotaPeakNonPagedPoolUsage", SIZE_T),
            ("QuotaNonPagedPoolUsage", SIZE_T),
            ("PagefileUsage", SIZE_T),
            ("PeakPagefileUsage", SIZE_T),
            ("PrivateUsage", SIZE_T),
        ]

    GetProcessMemoryInfo = ctypes.windll.psapi.GetProcessMemoryInfo
    GetProcessMemoryInfo.argtypes = [
        wintypes.HANDLE,
        ctypes.POINTER(PROCESS_MEMORY_COUNTERS_EX),
        wintypes.DWORD,
    ]
    GetProcessMemoryInfo.restype = wintypes.BOOL

    OpenProcess = ctypes.windll.kernel32.OpenProcess
    OpenProcess.argtypes = [wintypes.DWORD, wintypes.BOOL, wintypes.DWORD]
    OpenProcess.restype = wintypes.HANDLE

    CloseHandle = ctypes.windll.kernel32.CloseHandle

    def get_memory_info(process):  # type: (int) -> Optional[int]
        counters = PROCESS_MEMORY_COUNTERS_EX()
        ret = GetProcessMemoryInfo(
            process, ctypes.byref(counters), ctypes.sizeof(counters)
        )
        if not ret:
            return None
        return getattr(counters, "PeakWorkingSetSize")


project_folder = os.path.dirname(__file__)

def run_test(exe, input, output, reference_output):
    cmd = [exe, input, output]
    print('Running test: ' + input)
    start = time.time()
    process_handle = None
    try:
        child = subprocess.Popen(cmd, close_fds=(sys.platform != 'win32'))
        if sys.platform == 'win32':
            process_handle = OpenProcess(0x400, False, child.pid)
        child.communicate(timeout=600) # 10 minutes is the default timeout
        status = 'Finished'
        rc = child.returncode
    except subprocess.TimeoutExpired:
        child.kill()
        status = 'Timeout'
        print('ERROR: Failing with timeout')
        rc = child.returncode
    end = time.time()
    print('Time: ' + str(end - start) + 's')

    if sys.platform == "win32" and process_handle is not None:
        memory_bytes = get_memory_info(process_handle)
        print('Peak memory: ' + str(round(memory_bytes / (1024 * 1024), 2)) + 'Mb')
        CloseHandle(process_handle)

    if rc != 0:
        print('ERROR: Child process ended with returncode=' + str(rc))
        print('ERROR: Failing command: "' + exe + '" "' + input + '" "' + output + '"')
    elif not os.path.isfile(output):
        print('ERROR: no output file found')
        print('ERROR: Failing command: "' + exe + '" "' + input + '" "' + output + '"')
    else:
        print('Status: ' + status)

def dump_visualization_result(test_output_path, test_reference_path, html_output_dir):
    import shutil
    visualizer_dir =  os.path.join(project_folder, 'PointVisualizer')
    if not os.path.isdir(visualizer_dir):
        print('ERROR: Cannot find PointVisualizer directory')
        sys.exit(1)

    if not os.path.isfile(os.path.join(html_output_dir, 'OrbitControls.js')):
        shutil.copyfile(os.path.join(visualizer_dir, 'OrbitControls.js'), os.path.join(html_output_dir, 'OrbitControls.js'))
    if not os.path.isfile(os.path.join(html_output_dir, 'three.js')):
        shutil.copyfile(os.path.join(visualizer_dir, 'three.js'), os.path.join(html_output_dir, 'three.js'))
    
    html_output = ''
    with open(os.path.join(visualizer_dir, 'PointVisualizer.html'), 'rt') as f:
        html_output = f.read()
    
    test_output = ''
    if os.path.isfile(test_output_path):
        if (os.path.getsize(test_output_path) / (1024 * 1024)) > 100:
            print('WARNING: test output is bigger than 100Mb, skipping generation of visualizer file')
            return
        else:
            with open(test_output_path, 'rt') as f:
                test_output = f.read()
    
    test_reference = ''
    longest_common_path = None
    if test_reference_path is not None and os.path.isfile(test_reference_path):
        if (os.path.getsize(test_reference_path) / (1024 * 1024)) > 100:
            print('WARNING: test output is bigger than 100Mb, skipping generation of visualizer file')
            return
        else:
            with open(test_reference_path, 'rt') as f:
                test_reference = f.read()
    
        longest_common_path = os.path.commonpath([test_output_path, test_reference_path])

    if test_output != '':
        test_name = test_output_path if longest_common_path is None else os.path.relpath(test_output_path, longest_common_path)
        test_name = test_name.replace('\\', '/')
        html_output = html_output.replace('%NEW_FILENAME%', test_name)
        html_output = html_output.replace('%NEW_TEST_OUTPUT%', test_output)
    if test_reference != '':
        test_name = test_reference_path if longest_common_path is None else os.path.relpath(test_reference_path, longest_common_path)
        test_name = test_name.replace('\\', '/')
        html_output = html_output.replace('%OLD_FILENAME%', test_name)
        html_output = html_output.replace('%OLD_TEST_OUTPUT%', test_reference)

    html_path = os.path.join(html_output_dir, os.path.basename(test_output_path)+'.html')
    with open(html_path, 'wt') as f:
        f.write(html_output)
    print("PointVisualizer: " + html_path)


project_folder = os.path.dirname(__file__)
build_dir =  os.path.join(project_folder, 'build') if len(sys.argv) < 2 else os.path.abspath(sys.argv[1])
os.makedirs(build_dir, exist_ok=True)
config = 'Release'
print('Finding cutSphereMove executable...')
if sys.platform == "win32":
    exe_candidates = [os.path.join(build_dir, config, 'cutSphereMove.exe'), os.path.join(build_dir, 'cutSphereMove.exe')]
else:
    exe_candidates = [os.path.join(build_dir, 'cutSphereMove')]

cutSphereMoveExecutable = None
for candidate in exe_candidates:
    if os.path.isfile(candidate):
        cutSphereMoveExecutable = candidate

if cutSphereMoveExecutable is None:
    print('ERROR: Did you forget to build?')
    print('ERROR: Cannot find cutSphereMove anywhere, tried: ')
    for candidate in exe_candidates:
        print('\t' + exe_candidates)
    print('ERROR: If your cmake-generated build folder is in other place - pass it as first argument to this script')
    print
    sys.exit(1)

print('Running tests...')
test_output_dir = os.path.join(build_dir, 'tests_output')
os.makedirs(test_output_dir, exist_ok=True)
test_html_output_dir = os.path.join(build_dir, 'tests_html_output')
os.makedirs(test_html_output_dir, exist_ok=True)
for test in os.listdir(os.path.join(project_folder, 'tests', 'input')):
    print(''.join(['-']*80))
    test_input_path = os.path.join(project_folder, 'tests', 'input', test)
    test_output_path = os.path.join(test_output_dir, test)
    test_reference_path = os.path.join(project_folder, 'tests', 'reference', test)
    if not os.path.isfile(test_reference_path):
        test_reference_path = None

    run_test(cutSphereMoveExecutable, test_input_path, test_output_path, test_reference_path)
    dump_visualization_result(test_output_path, test_reference_path, test_html_output_dir)
print(''.join(['-']*80))