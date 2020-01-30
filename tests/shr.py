#!/usr/bin/env python


import re
import os
import json

from util import run_asm

# 8xy6 SHR Vx {, Vy}
# Set Vx = Vx SHR 1
# If the least significant bit of Vx is 1, set VF to 1.
# Divide Vx by 2.

def test_shr():
    asm = """
        LD V0, #2
        SHR V0
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == 1

def test_shr_with_last_bit_one():
    asm = """
        LD V0, #3
        SHR V0
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == 1
    assert emulator_debug.get("VF") == 1
