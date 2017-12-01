# -*- coding: utf-8 -*-

"""client_sniffer.py: script to manage the client side of the Wormhole.

   The script manages the sniffer part. It is in charge of communication with
   the server side (Replayer) by sending the frames received from the serial
   port. It continuously keep connecting to the server IP and port if the
   connection goes down.

"""

import binascii
import time
import serial
import socket

__author__      = "Dario Varano"
__email__ = "dario.varano@ing.unipi.it"

def socket_manager(server_IP, server_port):
    """A function to set up a socket communication.
    
    Keyword arguments:
    server_IP -- the IP of the server
    server_port -- the port of the server    
    """
    socket_fail = 1
    # cycle until the connection is done
    while socket_fail:
       try:
           # socket initialization and connection
           client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
           print "Trying to connect to server..."        
           client_socket.connect((server_IP, server_port))
           socket_fail = 0
           print "Connection done!"
           return client_socket
       except socket.error:
           time.sleep(5)
           socket_fail = 1

def socket_writer(frame, frame_length):
    """A funtion to send a frame to the client and return the #bytes sent.
    
    Keyword arguments:
    frame -- frame to send to the server
    frame_length -- the length of the frame to be sent    
    """
    global server_IP, server_port, client_socket   
    sent_data = 0
    # cycle until data have been sent, otherwise wait until the server responds
    while (sent_data == 0):
        try:   
            # send MS
            sent_data += client_socket.send(ms)
            #send length
            sent_data += client_socket.send(frame_length)
            #send frame
            sent_data += client_socket.send(frame)
            print "Sent "+str(sent_data)+" bytes to the server socket..."
            return sent_data
        except socket.error:
            sent_data = 0
            print "Connection lost! Trying to reconnect..."
            client_socket = socket_manager(server_IP, server_port)

ms = b'\x53\x6E\x69\x66' #magic sequence 'Snif'
state = 0
min_transmission_interval = 1
old_transmitted_frame = ''
old_transmission_time = 0.0
delay_time = 0.3

# serial port name
port_name = "/dev/ttyACM0"
sniffer_serial = serial.Serial(port_name,
				baudrate=115200,
				parity=serial.PARITY_NONE,
				stopbits=serial.STOPBITS_ONE,
				bytesize=serial.EIGHTBITS,
				xonxoff=False,
				rtscts=False,
				timeout=None)

# reset I/O buffers of the sniffer
sniffer_serial.reset_input_buffer()
sniffer_serial.reset_output_buffer()

print "Serial port opened!"

# IP and port of the server
server_IP = '127.0.0.1'
server_port = 9999
          
while True:
    # get 1 byte
    recv_byte = sniffer_serial.read()
    # MS recognition
    if (recv_byte == 'S'):
        state = 1
    elif (state == 1) and (recv_byte == 'n'):
        state = 2
    elif (state == 2) and (recv_byte == 'i'):
        state = 3
    elif (state == 3) and (recv_byte == 'f'):
        #get frame length
        frame_length = sniffer_serial.read()
        #convert byte to int
        frame_length_int = int(binascii.hexlify(frame_length), 16)
        if (frame_length_int == 0):
            state = 0
        else:
            print "Sending a frame to the server..."            
            print "MS Done! Length: " + str(frame_length_int)
            #get frame data
            frame = sniffer_serial.read(frame_length_int)
            #frame = frame + b'\x01'
            print binascii.hexlify(frame)
            #check replayer timeout (avoid sending equal multiple frames "at a time")
            if (old_transmitted_frame == frame) and (time.time() - old_transmission_time < min_transmission_interval):
                state = 0
                print "Frame filtered!"
            else:
                time.sleep(delay_time)
                try:
                    socket_writer(frame, frame_length)
                except Exception:
                    client_socket = socket_manager(server_IP, server_port)
                # getting trasmission time
                old_transmission_time = time.time()
                old_transmitted_frame = frame
                state = 0
                print "Frame Sent!"
            print "\n"
    else:
        # Unrecognized byte, going back to state 0
        state = 0
