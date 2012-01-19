#!/usr/bin/env python
# -*- coding: utf-8; -*-

import threading
import time
import sys
from struct import Struct
from gi.repository import GObject

status = Struct('cccccffffff')

class Poller(threading.Thread):
    def __init__(self, app, pollrate, **kwargs):
        threading.Thread.__init__(self)
        self.lock = threading.Lock()
        self.app = app
        self.delay = float(pollrate) / 1000
        self._run = True

    def stop(self):
        self._run = False

    def set_fuel_pump(self, value):
        with self.lock:
            self.write('ac' + (value and 'a' or 'b'))
            line = self.readline()
            if line:
                print('io:', line, file=sys.stderr)

    def set_injector(self, value):
        with self.lock:
            self.write('ag' + (value and 'a' or 'b'))
            line = self.readline()
            if line:
                print('io:', line, file=sys.stderr)

    def set_dme(self, value):
        with self.lock:
            self.write('ah' + (value and 'a' or 'b'))
            line = self.readline()
            if line:
                print('io:', line, file=sys.stderr)
            
    def read(self, size):
        raise NotImplementedError

    def readline(self):
        raise NotImplementedError

    def write(self, data):
        raise NotImplementedError

    def run(self):
        global status
        t = time.time()
        last = 0
        while self._run:
            with self.lock:
                self.write('ba')
                data = self.read(status.size)
                row = status.unpack(data)
                time.sleep(0.05)

            GObject.idle_add(self.app.set_iac, ord(row[0]))
            GObject.idle_add(self.app.set_fan, ord(row[1]))
            GObject.idle_add(self.app.set_dme, ord(row[2]))
            GObject.idle_add(self.app.set_fuel_pump, ord(row[3]))
            GObject.idle_add(self.app.set_injector, ord(row[4]))
            GObject.idle_add(self.app.set_lambda, row[5])
            GObject.idle_add(self.app.set_amm, row[6])
            GObject.idle_add(self.app.set_amm_temp, row[7])
            GObject.idle_add(self.app.set_rpm, row[8])
            GObject.idle_add(self.app.set_engine_temp, row[9])
            GObject.idle_add(self.app.set_fuel_consumption, row[10])
            
            now = time.time()
            wall = now - t
            delta = self.delay - wall
            t += self.delay
            read = now-last
            last = now
            if delta < 0:
                print('Not reading fast enough, try lowering poll-rate (current: %dms, read operation took %dms)' % (int(self.delay*1000), int(read*1000)), file=sys.stderr)
                continue
            time.sleep(delta)
