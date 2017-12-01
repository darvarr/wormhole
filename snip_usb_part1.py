# -*- coding: utf-8 -*-

"""snip_usb_part1.py: It receives from a serial port and forward data to the other one.
"""

import serial

__author__      = "Dario Varano"
__email__ = "dario.varano@ing.unipi.it"

# initialize ports
snip1_port = "/dev/ttyACM0"
snip1_serial = serial.Serial(snip1_port, 
                              baudrate=115200, 
                              parity=serial.PARITY_NONE, 
                              stopbits=serial.STOPBITS_ONE, 
                              bytesize=serial.EIGHTBITS, 
                              xonxoff = False, 
                              rtscts = False, 
                              write_timeout=None)

snip2_port = "/dev/ttyACM2"
snip2_serial = serial.Serial(snip2_port, 
                            baudrate=115200, 
                            parity=serial.PARITY_NONE, 
                            stopbits=serial.STOPBITS_ONE, 
                            bytesize=serial.EIGHTBITS, 
                            xonxoff = False, 
                            rtscts = False, 
                            timeout=None)

# reset I/O buffers of both sniffer and replayer 
snip1_serial.reset_input_buffer()
snip1_serial.reset_output_buffer()
snip2_serial.reset_input_buffer()
snip2_serial.reset_output_buffer()

#sys.stdout = open("/home/hino/Scrivania/sniplayer1.log", 'a')

while True:
    # get 1 byte
    recv_byte = snip1_serial.read()
    with open("sniplayer2.log", 'a') as infile:
        infile.write(recv_byte)
    snip2_serial.write(recv_byte)