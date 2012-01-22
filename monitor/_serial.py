#!/usr/bin/env python
# -*- coding: utf-8; -*-

import serial
from _poller import Frobnicator

class SerialPoller(Frobnicator):
    def __init__(self, app, device, baudrate, **kwargs):
        Frobnicator.__init__(self, app, **kwargs)
        self.ser = serial.Serial(port=device, baudrate=baudrate, timeout=10)
        self._desc = "Serial Mode   Device: %s   Baud: %d" % (device, baudrate)

    def description(self):
        return self._desc

    def read(self, size):
        return self.ser.read(size)

    def readline(self):
        return self.ser.readline()

    def write(self, data):
        self.ser.write(data)
