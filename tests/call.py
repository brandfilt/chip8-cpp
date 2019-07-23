#!/usr/bin/env python

import pexpect
import signal

import re
import json


def test_call():
    pexpect.run("../assembler/build/assembler call.asm call.bin")
    output = pexpect.run("../emulator/build/bin/emulator call.bin")
    match = re.search(r"({[\w:,\"]+})", str(output))
    assert match != None

    emulator_debug = json.loads(match.group(1))
    assert emulator_debug.get("PC") == 0x200 + 6
