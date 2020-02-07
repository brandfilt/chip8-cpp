#!/usr/bin/env python

import re
import os
import json

from util import run_asm

def test_jp():
    asm = """
        LD V0, #1
        JP test
        EXIT
test:   LD V0, #2
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == 2
    assert emulator_debug.get("PC") == 0x200 + 8

def test_jp_with_v0():
    # hex #204 + #2 is the EXIT row
    asm = """
        LD V0, #2
        JP V0, #204
        LD V0, #1
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == 2
    assert emulator_debug.get("PC") == 0x200 + 6
