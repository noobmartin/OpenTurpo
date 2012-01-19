#!/usr/bin/env python
# -*- coding: utf-8; -*-

import serial
from _poller import Poller

class SerialPoller(Poller):
    def __init__(self, app, device, baudrate, **kwargs):
        Poller.__init__(self, app, **kwargs)
        self.ser = serial.Serial(port=device, baudrate=baudrate, timeout=1)
          
    def read(self, size):
        return self.ser.read(size)

    def readline(self):
        return self.ser.readline()

    def write(self, data):
        self.ser.write(data)
