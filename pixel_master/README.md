# Pixel Master

Contains the code running on the Raspberry Pi to trigger stuff when the slaves are activated.

## Install

* comment ``blacklist i2c-bcm2708`` in ``/etc/modprobe.d/raspi-blacklist.conf``.
* add ``i2c-dev`` to ``/etc/modules``
* install tools with ``sudo apt-get install i2c-tools``
* setup permission for pi user ``sudo adduser pi i2c``
* test after reboot using ``i2cdetect -y 0``
* install python i2c libs ``sudo apt-get install python-smbus``
* change address in ``master.py`` and run it to check if output is printed

## Links

* [Raspberry Pi I2C (Python) on instructable.com](http://www.instructables.com/id/Raspberry-Pi-I2C-Python/?ALLSTEPS])
