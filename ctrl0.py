# -*- coding: utf-8 -*-

"""ctrl0.py: It sends the command to turn off the Radio to the replayer serial port.
"""

import serial

__author__      = "Dario Varano"
__email__ = "dario.varano@ing.unipi.it"

# initialize port
replayer_port = "/dev/ttyACM0"
replayer_serial = serial.Serial(replayer_port, 
                            baudrate=115200, 
                            parity=serial.PARITY_NONE, 
                            stopbits=serial.STOPBITS_ONE, 
                            bytesize=serial.EIGHTBITS, 
                            xonxoff = False, 
                            rtscts = False, 
                            timeout=None)
                            
replayer_serial.write("ctrl0")