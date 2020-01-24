#!/usr/bin/env python


import re
import os
import json

from util import run_asm

def test_or():
    asm = """
        LD V0, #1
        LD V1, #3
        OR V0, V1
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == 3

def test_and():
    asm = """
        LD V0, #1
        LD V1, #3
        AND V0, V1
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == 1

def test_xor():
    asm = """
        LD V0, #1
        LD V1, #3
        XOR V0, V1
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == 2
