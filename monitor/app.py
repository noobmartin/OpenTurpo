#!/usr/bin/env python
# -*- coding: utf-8; -*-

from gi.repository import Gtk, Gdk, GLib, GObject
import argparse
import serial
import threading
import time
import sys, os
from struct import Struct

status = Struct('cccccffffff')

# for testing
rnd = list(open('/dev/urandom', 'rb').read(status.size))
rnd[1] //= 86
rnd[2] //= 129
rnd[3] //= 129
rnd[4] //= 129

class Poller(threading.Thread):
    def __init__(self, app, pollrate, **kwargs):
        threading.Thread.__init__(self)
        self.lock = threading.Lock()
        self.app = app
        self.delay = float(pollrate) / 1000
        self._run = True
        self.start()

    def stop(self):
        self._run = False

    def set_fuel_pump(self, value):
        with self.lock:
            self.write('ac' + (value and 'a' or 'b'))
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
  
class App(object):
    fan_mode = {
        0: 'Disabled',
        1: 'Low',
        2: 'High',
    }
    relay_mode = {
        0: '<span color="#ff0000">OFF</span>',
        1: 'ON',
    }

    def __init__(self, pollcls, **kwargs):
        builder = Gtk.Builder()
        builder.add_from_file(os.path.join(os.path.dirname(sys.modules[__name__].__file__), "ui.xml"))
        builder.connect_signals({
                "on_window_destroy" : self.quit,
                "btn_fuel_pump_pressed": self.btn_fuel_pump_pressed,
        })
        self.window = builder.get_object("window1")
        self.window.show()

        accelgroup = Gtk.AccelGroup()
        key, modifier = Gtk.accelerator_parse('<Control>q')
        accelgroup.connect(key, modifier, 0, self.quit)
        self.window.add_accel_group(accelgroup)

        self.lb_iac              = builder.get_object('lb_iac')
        self.lb_fan              = builder.get_object('lb_fan')
        self.lb_dme              = builder.get_object('lb_dme')
        self.lb_fuel_pump        = builder.get_object('lb_fuel_pump')
        self.lb_injector         = builder.get_object('lb_injector')
        self.lb_lambda           = builder.get_object('lb_lambda')
        self.lb_amm              = builder.get_object('lb_amm')
        self.lb_amm_temp         = builder.get_object('lb_amm_temp')
        self.lb_engine_temp      = builder.get_object('lb_engine_temp')
        self.lb_rpm              = builder.get_object('lb_rpm')
        self.lb_fuel_consumption = builder.get_object('lb_fuel_consumption')
        self.btn_fuel_pump       = builder.get_object('btn_fuel_pump')
        self.btn_injector        = builder.get_object('btn_injector')
        self.btn_dme             = builder.get_object('btn_dme')

        self.poll = pollcls(self, **kwargs)
    
    def btn_fuel_pump_pressed(self, widget):
        self.poll.set_fuel_pump(not widget.get_active())

    def set_iac(self, value):
        self.lb_iac.set_markup('%d' % value)

    def set_fan(self, value):
        self.lb_fan.set_markup(App.fan_mode[value])

    def set_dme(self, value):
        self.btn_dme.set_active(value == 1)
        self.lb_dme.set_markup(App.relay_mode[value])

    def set_injector(self, value):
        self.btn_injector.set_active(value == 1)
        self.lb_injector.set_markup(App.relay_mode[value])

    def set_fuel_pump(self, value):
        self.btn_fuel_pump.set_active(value == 1)
        self.lb_fuel_pump.set_markup(App.relay_mode[value])

    def set_lambda(self, value):
        self.lb_lambda.set_markup('%02.3fV' % value)

    def set_amm(self, value):
        self.lb_amm.set_markup('%.3fKg/h' % value)

    def set_amm_temp(self, value):
        self.lb_amm_temp.set_markup('%.3f°C' % value)

    def set_engine_temp(self, value):
        self.lb_engine_temp.set_markup('%.3f°C' % value)

    def set_rpm(self, value):
        self.lb_rpm.set_markup('%d' % value)

    def set_fuel_consumption(self, value):
        self.lb_fuel_consumption.set_markup('%f' % value)
    
    def quit(self, *args):
        self.poll.stop()
        Gtk.main_quit()
        return True

parser = argparse.ArgumentParser(description='Herp a derp.')
parser.add_argument('-d', '--device', default='/dev/ttyS1')
parser.add_argument('-r', '--baudrate', default=9600, type=int)
parser.add_argument('-t', '--pollrate', default=1000/25, type=int, help="Poll rate in ms")
parser.add_argument('--mockup', dest='pollcls', action="store_const", default=SerialPoller, const=MockPoller, help="Disable serial communication and use mockup communication.")

args = vars(parser.parse_args())

if __name__ == "__main__":
    app = App(**args)

    GLib.threads_init()
    Gdk.threads_init()
    Gdk.threads_enter()
    Gtk.main()
    Gdk.threads_leave()
