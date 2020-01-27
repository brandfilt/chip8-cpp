#!/usr/bin/env python


import re
import os
import json

from util import run_asm

def test_sub_register():
    asm = """
        LD V0, #1
        LD V1, #1
        SUB V0, V1
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == 0

def test_sub_register_with_carry():
    asm = """
        LD V0, #f
        LD V1, #1
        SUB V0, V1
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == 14
    assert emulator_debug.get("VF") == 1
