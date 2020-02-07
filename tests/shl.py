#!/usr/bin/env python

import re
import os
import json

from util import run_asm

def test_shl():
    asm = """
        LD V0, #1
        SHL V0
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == 2

def test_shl_with_first_bit_one():
    asm = """
        LD V0, #80
        SHL V0
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == 0
    assert emulator_debug.get("VF") == 1
