#!/usr/bin/env python

import re
import os
import json

from util import run_asm

def test_call():
    asm = """
        CALL test
        EXIT

test:   EXIT
    """
    emulator_debug = run_asm(asm)
    assert emulator_debug.get("PC") == 0x200 + 6
