#!/usr/bin/env python

import re
import os
import json

from util import run_asm

def test_subn_not_borrow():
    asm = """
        LD V0, #1
        LD V1, #2
        SUBN V0, V1
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == 1
    assert emulator_debug.get("VF") == 1

def test_subn_borrow():
    asm = """
        LD V0, #2
        LD V1, #1
        SUBN V0, V1
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == 255
    assert emulator_debug.get("VF") == 0
