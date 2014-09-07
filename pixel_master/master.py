# scan the i2c bus and trigger a movie if all available
# devices returned a non-zero value

import smbus
import time
import os
bus = smbus.SMBus(1)

# repaint the creen with an image to get rid fo the console
os.system("sudo fbi -T 2 -d /dev/fb0 -noverbose -a /home/pi/black.jpg")

# main loop
while True:
    # re-init variable on each run
    activeDevices = 0
    devices = []

    # detect available devices (we do this on each run since it is fast and allows
    # reconfiguring without a reboot
    for address in range(2, 255):
        try:
            bus.write_byte(address,0xFF)
        except IOError:
            "" # ignore non existant devices
        else:
            print "Detected slave on", address
            devices += [address]
    
    print "Found", len(devices), "devices on the BUS"
    
    # check for device state
    for address in devices:
        try:
            value = bus.read_byte(address)
        except IOError:
            "" 
        else:
            "" 
        if (value > 0):
            activeDevices += 1
    
    print "There where", activeDevices, "active devices"
    
    # trigger movie as applicable
    if (activeDevices == len(devices)):
        print "Triggering Movie"
        os.system("omxplayer --vol 0 --amp 0 -b /home/pi/test-mpeg_512kb.mp4")

