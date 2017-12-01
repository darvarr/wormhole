# -*- coding: utf-8 -*-

"""sniplayer_server.py: script to manage the server side of the Wormhole.

   The script manages the server part. When a new frame is received, it is
   then sent to the sniplayer serial port. When a new frame is read from the 
   serial port, it is forwarded to the client through the socket. 
   It is always listening for incoming connection.
   
"""

import serial
import threading
import socket

__author__      = "Dario Varano"
__email__ = "dario.varano@ing.unipi.it"

class SnifAndSendThread (threading.Thread):
    """A thread which receives from the serial port and write to the socket.
    
    Args:
        snif_serial : Serial port where to read.
        log_file_name : Path of the log file.
        server_socket : Socket of the server, where the incoming frame are forwarded
    
    Attributes:
        snif_serial : Serial port where to write.
        log_file_name : Path of the log file.
        server_socket : Socket of the server, where the incoming frame are forwarded
    
    """
    def __init__(self, snif_serial, log_file_name, server_socket):
        threading.Thread.__init__(self)
        self.snif_serial = snif_serial
        self.server_socket = server_socket
        self.log_file_name = log_file_name
    def run(self):
        print "Thread SnifAndSend started!"
        # wait for the connection
        conn, addr = self.server_socket.accept()
        print "Server connected to " + str(addr)
        while True:
            # get 1 byte
            recv_byte = self.snif_serial.read()
            with open(self.log_file_name, 'a') as infile:
                infile.write(recv_byte)
            try:
                conn.send(recv_byte)
            except socket.error:
                print "Lost connection with " + str(addr)
                conn, addr = self.server_socket.accept()
                print "Server connected to " + str(addr)

class RecvAndRplyThread (threading.Thread):
    """A thread which receives from the socket and write to the serial port.
    
    Args:
        replayer_serial : Serial port where to write.
        log_file_name : Path of the log file.
        server_socket : Socket of the server, where the incoming frame are received
    
    Attributes:
        replayer_serial : Serial port where to write.
        log_file_name : Path of the log file.
        server_socket : Socket of the server, where the incoming frame are received
    
    """
    def __init__(self, replay_serial, log_file_name, server_socket):
        threading.Thread.__init__(self)
        self.replay_serial = replay_serial
        self.log_file_name = log_file_name
        self.server_socket = server_socket
    def run(self):
        print "Thread RecvAndRply started!"
        # wait for the connection
        conn, addr = self.server_socket.accept()
        print "Server connected to " + str(addr)
        while True:
            try:
                recv_byte = conn.recv(1)
            except socket.error:
                print "Lost connection with " + str(addr)
                conn, addr = self.server_socket.accept()
                print "Server connected to " + str(addr)
            with open(self.log_file_name, 'a') as infile:
                infile.write(recv_byte)
            self.replay_serial.write(recv_byte)

port = "/dev/ttyACM0"
serial = serial.Serial(port, 
                            baudrate=115200, 
                            parity=serial.PARITY_NONE, 
                            stopbits=serial.STOPBITS_ONE, 
                            bytesize=serial.EIGHTBITS, 
                            xonxoff = False, 
                            rtscts = False, 
                            timeout=None)

# reset I/O buffers of both sniffer and replayer 
serial.reset_input_buffer()
serial.reset_output_buffer()

print "Serial ports opened"

server_port_recv = 9999
server_port_send = 8888

# socket 1 initialization and binding
server_socket_recv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket_recv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_socket_recv.bind(('', server_port_recv))
print "Server ready! Waiting for an incoming connection..."
# only a connection expected
server_socket_recv.listen(1)

# socket 2 initialization and binding
server_socket_send = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket_send.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_socket_send.bind(('', server_port_send))
print "Server ready! Waiting for an incoming connection..."
# only a connection expected
server_socket_send.listen(1)

# path of log files
log_file_name_snif = "/home/hino/Scrivania/server_snif.log"
log_file_name_rply = "/home/hino/Scrivania/server_rply.log"

# thread list
threads = []

# Create threads
thread_snif = SnifAndSendThread(serial, log_file_name_snif, server_socket_send)
thread_rply = RecvAndRplyThread(serial, log_file_name_rply, server_socket_recv)

# Start threads
thread_snif.start()
thread_rply.start()

# Add threads to thread list
threads.append(thread_snif)
threads.append(thread_rply)