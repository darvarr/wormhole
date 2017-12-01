# -*- coding: utf-8 -*-

"""server_replay.py: script to manage the server side of the Wormhole.

   The script manages the replayer part. When a new frame is received, it is
   then sent to the replayer serial port. It is always listening for
   incoming connection.
   
"""

import serial
import binascii
import time
import socket

__author__      = "Dario Varano"
__email__ = "dario.varano@ing.unipi.it"

def serial_writer(port, frame, frame_length):
    """A function to writes a frame to a serial port and returns the #bytes written.
    
    Keyword arguments:
    port -- the serial port where to write the incoming frame
    frame -- the frame to be written
    frame_length -- the length of the frame    
    """
    global ms
    written_data = 0    
    # send MS
    written_data += port.write(ms)
    # send length
    written_data += port.write(frame_length)
    # send frame
    written_data += port.write(frame)
    print "Written "+str(written_data)+" bytes to the replayer serial port..."
    return written_data

port_name = "/dev/ttyACM0"
state = 0
old_transmission_time = 0
old_transmitted_frame = ''
delay_time = 0.3
min_transmission_interval = 0.5
ms = b'\x53\x6E\x69\x66' #magic sequence 'Snif'

# Serial Management
replayer_serial = serial.Serial(port_name, 
                            baudrate=115200, 
                            parity=serial.PARITY_NONE, 
                            stopbits=serial.STOPBITS_ONE, 
                            bytesize=serial.EIGHTBITS, 
                            xonxoff = False, 
                            rtscts = False, 
                            timeout=None)

# reset I/O buffers of the replayer
replayer_serial.reset_input_buffer()
replayer_serial.reset_output_buffer()

print "Serial port opened! Starting server..."

# Socket Management
server_port = 9999

# socket initialization and binding
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_socket.bind(('', server_port))
print "Server ready! Waiting for an incoming connection..."
# only a connection expected
server_socket.listen(1)
# wait for the connection
conn, addr = server_socket.accept()
print "Server connected to " + str(addr)

while True:
    # get 1 byte
    recv_byte = conn.recv(1)
    # MS recognition
    if (recv_byte == 'S'):
        state = 1
    elif (state == 1) and (recv_byte == 'n'):
        state = 2
    elif (state == 2) and (recv_byte == 'i'):
        state = 3
    elif (state == 3) and (recv_byte == 'f'):
        #get frame length
        frame_length = conn.recv(1)
        #convert byte to int
        frame_length_int = int(binascii.hexlify(frame_length), 16)
        if (frame_length_int == 0):
            state = 0
        else:    
            print "Receiving a frame from client..."
            print "MS Done! Length: " + str(frame_length_int)
            #get frame data (block untill all data have been received)
            frame = conn.recv(frame_length_int, socket.MSG_WAITALL)
            print binascii.hexlify(frame)
            #check replayer timeout (avoid sending equal multiple frames "at a time")
            if (old_transmitted_frame == frame) and (time.time() - old_transmission_time > min_transmission_interval):
                state = 0
                print "Frame filtered!"
            else:
                time.sleep(delay_time)
                try:
                    serial_writer(replayer_serial, frame, frame_length)
                except serial.SerialException:
                    state = 0
                    print "An error occurred while writing to the serial port, skipping the packet..."
                else:
                    # getting trasmission time
                    old_transmission_time = time.time()
                    old_transmitted_frame = frame
                    state = 0
                    print "Frame sent!"
            print "\n"
    else:
        # Unrecognized byte, going back to state 0
        state = 0    