#!/usr/bin/env python

# 3xkk SE Vx, byte
# Skip next instructions if Vx = kk

import re
import os
import json

from util import run_asm

def test_se_byte():
    asm = """
        LD V0, #1
        SE V0, #1
        LD V0, #2
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == 1

def test_se_register():
    asm = """
        LD V0, #1
        LD V1, #1
        LD V3, #1
        SE V0, V1
        LD V3, #2
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V3") == 1
