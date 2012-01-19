#!/usr/bin/env python
# -*- coding: utf-8; -*-

from _poller import Poller
from struct import Struct
import random
import sys

status = Struct('cccccffffff')

# for testing
rnd = list(open('/dev/urandom', 'rb').read(status.size))
rnd[1] //= 86
rnd[2] //= 129
rnd[3] //= 129
rnd[4] //= 129

class MockPoller(Poller):
    def __init__(self, *args, **kwargs):
        Poller.__init__(self, *args, **kwargs)

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
          
    def read(self, size):
        return self._queue.pop()

    def readline(self):
        return None

    def write(self, data):
        global status

        t = data[0]
        if t == 'b': # read operation
            assert data[1] == 'a'
            self._queue.append(status.pack(
                    bytes([self._iac]),
                    bytes([self._fan]),
                    bytes([self._dme]),
                    bytes([self._fuel_pump]),
                    bytes([self._injector]),
                    self._lambda,
                    self._amm,
                    self._amm_temp,
                    self._rpm,
                    self._engine_temp,
                    self._fuel_consumption,
                    ))
        elif t == 'a': # set operation
            if data[1] == 'c': # fuel pump
                self._fuel_pump = data[2] == 'a' and 1 or 0
            elif data[1] == 'g': # injector
                self._injector = data[2] == 'a' and 1 or 0
            elif data[1] == 'h': # dme
                self._dme = data[2] == 'a' and 1 or 0
            else:
                print("unknown set operation:", data, file=sys.stderr)
        else:
            print("unknown operation:", data, file=sys.stderr)

