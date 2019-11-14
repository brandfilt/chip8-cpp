#!/usr/bin/env python

import re
import os
import json

import pexpect

assembler = "../assembler/build/assembler"
emulator = "../emulator/build/bin/emulator"

def run_asm(asm):
    filename = "temp.asm"
    binary_name = "out.bin"
    try:
        with open(filename, "w") as tmp:
            tmp.write(asm)
        pexpect.run(f"{assembler} {filename} {binary_name}")
        return pexpect.run(f"{emulator} {binary_name}")
    finally:
        os.remove(filename)
        os.remove(binary_name)
