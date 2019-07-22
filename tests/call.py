#!/usr/bin/env python

import pexpect
import signal


def test_call():
    pexpect.run("../assembler/build/assembler call.asm call.bin")
    output = pexpect.run("../emulator/build/bin/emulator call.bin")
    print(output)
