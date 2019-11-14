#!/usr/bin/env python

import re
import os
import json

from util import run_asm

def test_call():
    asm = """
        CALL test
        EXIT

test:   EXIT
    """
    output = run_asm(asm)
    match = re.search(r"({[\w:,\"]+})", str(output))
    assert match != None

    emulator_debug = json.loads(match.group(1))
    assert emulator_debug.get("PC") == 0x200 + 6
