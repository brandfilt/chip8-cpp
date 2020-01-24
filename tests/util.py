#!/usr/bin/env python

import re
import os
import json

import pexpect

assembler = "../assembler/build/assembler"
emulator = "../emulator/build/bin/emulator"

def parse_debug(output):
    match = re.search(r"({[\w:,\"]+})", str(output))
    assert match != None

    return json.loads(match.group(1))

def run_asm(asm):
    filename = "temp.asm"
    binary_name = "out.bin"
    try:
        with open(filename, "w") as tmp:
            tmp.write(asm)
        # run assembler on file
        pexpect.run(f"{assembler} {filename} {binary_name}")
        # run emulator
        output = pexpect.run(f"{emulator} {binary_name}")
        return parse_debug(output)
    finally:
        os.remove(filename)
        os.remove(binary_name)
