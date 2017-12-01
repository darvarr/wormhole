# -*- coding: utf-8 -*-

"""sniplayer_server.py: script to manage the server side of the Wormhole.

   The script manages the server part. When a new frame is received, it is
   then sent to the sniplayer serial port. When a new frame is read from the 
   serial port, it is forwarded to the client through the socket. 
   It continuously keep connecting to the server IP and port if the
   connection goes down.
   
"""

import serial
import threading
import socket
import time

__author__      = "Dario Varano"
__email__ = "dario.varano@ing.unipi.it"

class SnifAndSendThread (threading.Thread):
   """A thread which receives from the serial port and write to the socket.
    
    Args:
        snif_serial : Serial port where to read.
        log_file_name : Path of the log file.
        client_socket : Socket of the server, where the incoming frame are sent
        server_port : Port of the server
    
    Attributes:
        snif_serial : Serial port where to write.
        log_file_name : Path of the log file.
        client_socket : Socket of the server, where the incoming frame are sent
        server_port : Port of the server
    
    """
   def __init__(self, snif_serial, log_file_name, client_socket, server_port):
      threading.Thread.__init__(self)
      self.snif_serial = snif_serial
      self.client_socket = client_socket
      self.server_port = server_port
      self.log_file_name = log_file_name
   def run(self):
        print "Thread SnifAndSend started!"
        while True:
            # get 1 byte
            recv_byte = self.snif_serial.read()
            with open(self.log_file_name, 'a') as infile:
                infile.write(recv_byte)
            try:
                self.client_socket.send(recv_byte)
            except socket.error:
                self.client_socket = socket_manager(server_ip, self.server_port)

class RecvAndRplyThread (threading.Thread):
    """A thread which receives from the socket and write to the serial port.
    
    Args:
        replayer_serial : Serial port where to write.
        log_file_name : Path of the log file.
        client_socket : Socket of the server, where the incoming frame are received
        server_port : Port of the server
    
    Attributes:
        replayer_serial : Serial port where to write.
        log_file_name : Path of the log file.
        client_socket : Socket of the server, where the incoming frame are received
        server_port : Port of the server
    
    """
    def __init__(self, replay_serial, log_file_name, client_socket, server_port):
      threading.Thread.__init__(self)
      self.replay_serial = replay_serial
      self.log_file_name = log_file_name
      self.client_socket = client_socket
      self.server_port = server_port
    def run(self):
        print "Thread RecvAndRply started!"
        while True:
            try:
                recv_byte = self.client_socket.recv(1)
            except socket.error:
                self.client_socket = socket_manager(server_ip, self.server_port)
            with open(self.log_file_name, 'a') as infile:
                infile.write(recv_byte)
            self.replay_serial.write(recv_byte)


def socket_manager(server_IP, server_port):
    """A function to set up a socket communication.
    
    Keyword arguments:
    server_IP -- the IP of the server
    server_port -- the port of the server    
    """
    print "Socket Manager called"
    socket_fail = 1
    while socket_fail:
       try:
           # socket initialization and connection
           client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
           print "Trying to connect to server, port = "+ str(server_port) +" ..."        
           client_socket.connect((server_IP, server_port))
           socket_fail = 0
           print "Connection done! Port = " + str(server_port)
           return client_socket
       except socket.error:
           time.sleep(5)
           socket_fail = 1

ms = b'\x53\x6E\x69\x66' #magic sequence 'Snif'

# initialize ports
port = "/dev/ttyACM0"
serial = serial.Serial(port, 
                              baudrate=115200, 
                              parity=serial.PARITY_NONE, 
                              stopbits=serial.STOPBITS_ONE, 
                              bytesize=serial.EIGHTBITS, 
                              xonxoff = False, 
                              rtscts = False, 
                              write_timeout=None)

# reset I/O buffers of both sniffer and replayer 
serial.reset_input_buffer()
serial.reset_output_buffer()

print "Serial port opened!"

# socket initialization and connection
server_ip = "127.0.0.1"
server_port_send = 9999
server_port_recv = 8888

client_socket_send = socket_manager(server_ip, server_port_send)
client_socket_recv = socket_manager(server_ip, server_port_recv)

# path of the log files
log_file_name_snif = "/home/hino/Scrivania/client_snif.log"
log_file_name_rply = "/home/hino/Scrivania/client_rply.log"

# thread list
threads = []

# Create threads
thread_snif = SnifAndSendThread(serial, log_file_name_snif, client_socket_send, server_port_send)
thread_rply = RecvAndRplyThread(serial, log_file_name_rply, client_socket_recv, server_port_recv)

# Start threads
thread_snif.start()
thread_rply.start()

# Add threads to thread list
threads.append(thread_snif)
threads.append(thread_rply)