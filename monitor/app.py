#!/usr/bin/env python
# -*- coding: utf-8; -*-

from gi.repository import Gtk, Gdk, GLib, GObject
import argparse
import serial
import sys, os
import ctypes
from struct import Struct
from _mockup import MockPoller
from _serial import SerialPoller

class App(object):
    fan_mode = {
        0: 'Disabled',
        1: 'Low',
        2: 'High',
    }
    relay_mode = {
        0: '<span color="#ff0000">OFF</span>',
        1: '<span color="#009900">ON</span>',
    }

    def __init__(self, pollcls, **kwargs):
        builder = Gtk.Builder()
        builder.add_from_file(os.path.join(os.path.dirname(sys.modules[__name__].__file__), "ui.xml"))
        builder.connect_signals({
                "on_window_destroy" : self.quit,
                "btn_fuel_pump_pressed": self.btn_fuel_pump_pressed,
                "btn_injector_pressed": self.btn_injector_pressed,
                "btn_dme_pressed": self.btn_dme_pressed,
                "fan_output": self.fan_output,
                "fan_changed": self.fan_changed,
                "iac_changed": self.iac_changed,
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
        self.adj_fan             = builder.get_object('adj_fan')
        self.adj_iac             = builder.get_object('adj_iac')

        self.poll = pollcls(self, **kwargs)

        sb = builder.get_object('statusbar')
        context_id = sb.get_context_id("progress_message")
        sb.push(context_id, self.poll.description())

        self.poll.start()

    def fan_output(self, spinner):
        spinner.props.text = App.fan_mode[int(spinner.get_value())]
        return True

    def fan_changed(self, widget):
        self.poll.set_fan(widget.get_value())
        return True

    def iac_changed(self, widget):
        self.poll.set_iac(widget.get_value())
        return True
    
    def btn_fuel_pump_pressed(self, widget):
        self.poll.set_fuel_pump(not widget.get_active())

    def btn_injector_pressed(self, widget):
        self.poll.set_injector(not widget.get_active())

    def btn_dme_pressed(self, widget):
        self.poll.set_dme(not widget.get_active())

    def set_iac(self, value):
        self.adj_iac.set_value(value)
        self.lb_iac.set_markup('%d' % value)

    def set_fan(self, value):
        self.adj_fan.set_value(value)
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
parser.add_argument('-d', '--device', default='/dev/ttyS0')
parser.add_argument('-r', '--baudrate', default=9600, type=int)
parser.add_argument('-t', '--pollrate', default=1000/25, type=int, help="Poll rate in ms")
parser.add_argument('--mockup', dest='pollcls', action="store_const", default=SerialPoller, const=MockPoller, help="Disable serial communication and use mockup communication.")

def run():
    args = vars(parser.parse_args())
    app = App(**args)

    GLib.threads_init()
    Gdk.threads_init()
    Gdk.threads_enter()
    Gtk.main()
    Gdk.threads_leave()

if __name__ == "__main__":
    run()
