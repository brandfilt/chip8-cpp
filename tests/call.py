#!/usr/bin/env python

import pexpect


def test_call():
    child = pexpect.spawn("../assembler/build/assembler call.asm call.bin")
    child = pexpect.spawn("../emulator/build/bin/emulator call.bin")
    child.expect("*DESTROY*")
