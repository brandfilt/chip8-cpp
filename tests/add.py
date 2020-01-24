#!/usr/bin/env python

# // 7xkk ADD Vx, byte
# // Set Vx = Vx + kk

import re
import os
import json

from util import run_asm

def test_add_byte():
    asm = """
        LD V0, #1
        ADD V0, #1
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == 2

def test_add_register():
    asm = """
        LD V0, #1
        LD V1, #1
        ADD V0, V1
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == 2

def test_add_register_with_carry():
    asm = """
        LD V0, #1
        LD V1, #FF
        ADD V0, V1
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == 0
    assert emulator_debug.get("VF") == 1
