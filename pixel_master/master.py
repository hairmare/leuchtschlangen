# scan the i2c bus and trigger a movie if all available
# devices returned a non-zero value

import smbus
import time
import os
bus = smbus.SMBus(1)

# repaint the creen with an image to get rid fo the console
#os.system("sudo fbi -T 2 -d /dev/fb0 -noverbose -a /home/pi/black.jpg")

devices = [4, 8]
devices = [4]

# method to reset all devices to off
def disableDevices():
    for device in devices:
        print "Sending" ,[0, 0, 0], "to", device
        bus.write_i2c_block_data(device,0,[0, 0, 0])

# set a given device to a color
def setColor(device, color):
    bus.write_i2c_block_data(device, 0, color)

# set up some decent colors upon first run
for device in devices:
    setColor(device, [255, 255, 255])

# main loop (detec whats going on)
while False:
    # re-init variable on each run
    activeDevices = 0

    # check for device state
    for address in devices:
        value = 0
        try:
            value = bus.read_byte(address)
        except IOError:
            ""
        else:
            "" 
        if (value > 0):
            activeDevices += 1
    
    if (activeDevices > 0):
        print "There where", activeDevices, "active devices and I need", len(devices)
    
    # trigger movie as applicable
    if (activeDevices >= len(devices)-1):
        print "Triggering Movie"
        #os.system("omxplayer --vol 0 --amp 0 -b /home/pi/test-mpeg_512kb.mp4")
        disableDevices()

