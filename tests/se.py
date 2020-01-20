#!/usr/bin/env python

# 3xkk SE Vx, byte
# Skip next instructions if Vx = kk

import re
import os
import json

from util import run_asm

def test_call():
    asm = """
        LD V0, #1
        SE V0, #1
        LD V0, #2
        EXIT
    """
    output = run_asm(asm)
    match = re.search(r"({[\w:,\"]+})", str(output))
    assert match != None

    emulator_debug = json.loads(match.group(1))
    assert emulator_debug.get("V0") == 1
