# scan the i2c bus and trigger a movie if all available
# devices returned a non-zero value

import smbus
import time
import os
bus = smbus.SMBus(1)

# repaint the creen with an image to get rid fo the console
os.system("sudo fbi -a -T 2 -d /dev/fb0 -noverbose -a /home/pi/black.jpg")

devices = [4, 8, 12, 16, 3, 5, 6, 7, 9, 10, 11, 13]

deviceColors = {
  4: [0, 0, 255],
  8: [255, 0, 0],
 12: [0, 255, 255],
 16: [255, 0, 255],
  3: [0, 255, 0],
  5: [255, 255, 0],
  6: [64, 0, 255],
  7: [0, 64, 255],
  9: [0, 0, 255],
 10: [0, 255, 255],
 11: [0, 255, 0],
 13: [255, 64, 0]
}
deviceBrightnesses = {
  4: 128,
  8: 128,
 12: 128,
 16: 128,
  3: 128,
  5: 128,
  6: 128,
  7: 128,
  9: 128,
 10: 128,
 11: 128,
 13: 128
}
deviceDelays = {
  4: 22,
  8: 22,
 12: 22,
 16: 22,
  3: 22,
  5: 22,
  6: 22,
  7: 22,
  9: 22,
 10: 22,
 11: 22,
 13: 22
}

# method to reset all devices to off
def disableDevices():
    for device in devices:
        print "Sending" ,[0, 0, 0], "to", device
        try:
            bus.write_i2c_block_data(device,0,[0, 0, 0])
        except IOError:
            ""
        else:
            ""

def setBlockBit(bit):
    for device in devices:
        try:
            bus.write_i2c_block_data(device,2,[bit])
        except IOError:
            ""
        else:
            ""

# set a given device to a color
def setColor(device, color):
    bus.write_i2c_block_data(device, 0, color)

# toggle all strips to their color
def showAll():
    for device in devices:
        try:
            bus.write_i2c_block_data(device, 4, [])
        except IOError:
            ""
        else:
            ""

# toggle all strips to their color
def hideAll():
    for device in devices:
        try:
            bus.write_i2c_block_data(device, 5, [])
        except IOError:
            ""
        else:
            ""



# set brightness on device
def setBrightness(device, brightness):
    try:
        bus.write_i2c_block_data(device, 1, [brightness])
    except IOError:
        ""
    else:
        ""

# set up some decent colors upon first run
for device in devices:
    print "Setting color", deviceColors[device], "on", device
    setColor(device, deviceColors[device])
    setBrightness(device, deviceBrightnesses[device])
    bus.write_i2c_block_data(device, 3, [deviceDelays[device]])

# main loop (detec whats going on)
while True:
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
    
    print "There where", activeDevices, "active devices and I need", len(devices)
    
    # trigger movie as applicable
    #if (activeDevices >= len(devices)-2):
    if (activeDevices >= 2):
        print "Triggering Movie"
        setBlockBit(1)
        showAll()
        os.system("omxplayer --vol 0 --amp 0 -b /home/pi/hai_rund_1080x720rz_hires.mov")
        print "Done... disabling LEDS"
        setBlockBit(0)
        disableDevices()
        hideAll()

