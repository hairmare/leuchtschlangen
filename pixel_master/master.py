# simple example app, will be expanded to do work after PoC

import smbus
import time
bus = smbus.SMBus(0)

I2C_ADDRESS = 0x20

bus.write_byte(I2C_ADDRESS,0xFF)
value=bus.read_byte(I2C_ADDRESS)

print "%02X" % value
