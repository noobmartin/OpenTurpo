#!/usr/bin/env python
# -*- coding: utf-8; -*-

from _poller import Poller
from struct import Struct

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
          
    def read(self, size):
        global rnd
        return bytes(rnd)

    def readline(self):
        return None

    def write(self, data):
        pass
