#!/usr/bin/env python

# Cxkk RND Vx, byte
# Set Vx = random byte AND kk

import re
import os
import json

from util import run_asm

def test_rnd():
    asm = """
        LD V0, #0
        RND V0, #f
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") != 0

def test_rnd_v0_is_zero():
    asm = """
        LD V0, #0
        RND V0, #0
        EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("V0") == 0
