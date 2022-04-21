#!/usr/bin/env python3

import subprocess
import sys
import os
import re

INCREMENT_AMOUNT = 2
IMAGINE_DIR = os.getcwd()
CONTAINER_DIR = "/mnt/ImagineRIT2022"
CONTAINER_NAME = "imaginerit-embedded-dev"
SERIAL_DEVICE = "/dev/ttyUSB0"

ENV_LAUNCH_COMMAND = f"""
podman run -dt --name={CONTAINER_NAME} -v "{IMAGINE_DIR}":{CONTAINER_DIR} --device={SERIAL_DEVICE}:rwm --group-add keep-groups --annotation io.crun.keep_original_groups=1 --annotation run.oci.keep_original_groups=1 --security-opt label=disable {CONTAINER_NAME} """


CONFIG_LOC = IMAGINE_DIR + "/main/config.h"
VAR_NAME = "CONFIG_TAG_NUM"

try:
    subprocess.check_call(["ls", SERIAL_DEVICE])
except subprocess.CalledProcessError as e:
    print(f"Serial device {SERIAL_DEVICE} not found")
    sys.exit(1)


try:
    subprocess.check_output(ENV_LAUNCH_COMMAND, shell=True)

    with open(CONFIG_LOC, "r") as file:
        filestr = file.read()
        
    lines = filestr.split("\n")
    
    devNum = int(re.search(f"{VAR_NAME} 0x..", filestr).group()[-2:], 16)
    
    try:
        subprocess.check_call(["podman", "exec", CONTAINER_NAME, "/root/.espressif/python_env/idf4.3_py3.9_env/bin/python", "/root/esp/esp-idf/tools/idf.py", "flash", "-C", CONTAINER_DIR])
    except subprocess.CalledProcessError as e:
        print(e.output)
        sys.exit(1)
    
    print(f"""
    ===============================
    Sniffer 0x{'{:02x}'.format(devNum)} SAFE TO DISCONNECT
    ===============================
    """)
    
    devNum += INCREMENT_AMOUNT
    for i, line in enumerate(lines):
        if VAR_NAME in line:
            lines[i] = f"#define {VAR_NAME} 0x{'{:02x}'.format(devNum)}"
            print(line)
            break
    
    with open(CONFIG_LOC, "w") as file:
            file.write("\n".join(lines))
    
    
    subprocess.check_call(["podman", "exec", CONTAINER_NAME, "/root/.espressif/python_env/idf4.3_py3.9_env/bin/python", "/root/esp/esp-idf/tools/idf.py","build", "-C", CONTAINER_DIR])

finally:
    try:
        subprocess.check_output(f"podman kill {CONTAINER_NAME}", shell=True)
    finally:
        subprocess.check_output(f"podman rm {CONTAINER_NAME}", shell=True)
