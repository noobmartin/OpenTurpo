#!/usr/bin/env python
# -*- coding: utf-8; -*-

from __future__ import print_function
from _poller import Frobnicator
from struct import Struct
import random
import sys

status = Struct('cccccffffff')

import platform
major = platform.python_version_tuple()[0]
if major == '3':
    def encode(v):
        return bytes([v])
else:
    def encode(v):
        return chr(v)

class MockPoller(Frobnicator):
    def __init__(self, *args, **kwargs):
        Frobnicator.__init__(self, *args, **kwargs)

        self._iac = random.randint(0,255)
        self._fan = random.randint(0,2)
        self._dme = random.randint(0,1)
        self._fuel_pump = random.randint(0,1)
        self._injector = random.randint(0,1)
        self._lambda = random.random()
        self._amm = random.random()
        self._amm_temp = random.random() * 50.0
        self._engine_temp = random.random() * 50.0
        self._rpm = random.random() * 12000.0
        self._fuel_consumption = random.random() * 10.0

        self._queue = []
    
    def serialize(self):
        global status
        return status.pack(
            encode(self._iac),
            encode(self._fan),
            encode(self._dme),
            encode(self._fuel_pump),
            encode(self._injector),
            self._lambda,
            self._amm,
            self._amm_temp,
            self._rpm,
            self._engine_temp,
            self._fuel_consumption,
            )

    def description(self):
        return "Mockup Mode"

    def read(self, size):
        return self._queue.pop()

    def readline(self):
        return None

    def write(self, data):
        global status

        t = data[0]
        if t == 'b': # read operation
            assert data[1] == 'a'
            self._queue.append(self.serialize())
        elif t == 'a': # set operation
            if data[1] == 'c': # fuel pump
                self._fuel_pump = data[2] == 'a' and 1 or 0
            elif data[1] == 'g': # injector
                self._injector = data[2] == 'a' and 1 or 0
            elif data[1] == 'h': # dme
                self._dme = data[2] == 'a' and 1 or 0
            elif data[1] == 'k': # fan
                if data[2] == 'a': # disabled
                    self._fan = 0
                elif data[2] == 'b': # low
                    self._fan = 1
                elif data[2] == 'c': # high
                    self._fan = 2
                else:
                    raise ValueError
            elif data[1] == 'l': # iac
                self._iac = ord(data[2])
            else:
                raise ValueError("unknown set operation: %s" % ([data]), file=sys.stderr)
        else:
            raise ValueError("unknown operation: %s" % ([data]), file=sys.stderr)

