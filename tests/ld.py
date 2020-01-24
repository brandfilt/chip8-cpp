#!/usr/bin/env python


# // 6xkk LD Vx, byte
# // Set Vx = kk

# // 8xy0 LD Vx, Vy
# // Set Vx = Vy

import re
import os
import json

from util import run_asm

def test_ld_byte():
    asm = """
        LD V0, #12
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == int("12", 16)

def test_ld_register():
    asm = """
        LD V0, #1
        LD V1, #2
        LD V0, V1
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == int("2", 16)
