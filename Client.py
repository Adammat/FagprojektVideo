import sys
import socket
import os
import asyncore
import collections
import logging

'''
HOST, PORT = "localhost", 9999
data = " ".join(sys.argv[1:])

# Create a socket (SOCK_STREAM means a TCP socket)
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    # Connect to server and send data
    sock.connect((HOST, PORT))
    sock.sendall((data + "\n").encode())

    # Receive data from the server and shut down
    received = sock.recv(1024).decode().strip()
finally:
    sock.close()

print ("Sent:     {}".format(data))
print ("Received: {}".format(received))
'''
class Client(asyncore.dispatcher):

    def __init__(self, host_address, name):
        asyncore.dispatcher.__init__(self)
        self.log = logging.getLogger('Client (%7s)' % name)
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.name = name
        self.log.info('Connecting to host at %s', host_address)
        self.connect(host_address)
        self.outbox = collections.deque()

    def say(self, message):
        self.outbox.append(message)
        self.log.info('Enqueued message: %s', message)

    def handle_write(self):
        if not self.outbox:
            return
        message = self.outbox.popleft()
        if len(message) > MAX_MESSAGE_LENGTH:
            raise ValueError('Message too long')
        self.send(message)

    def handle_read(self):
        message = self.recv(MAX_MESSAGE_LENGTH)
        self.log.info('Received message: %s', message)

if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)
    logging.info('Creating clients')
    alice = Client(('localhost', 9999), 'Alice')
    bob = Client(('localhost', 9999), 'Bob')
    alice.say('{REQ:FPS}')
    bob.say('{FPS=20}')
    alice.say('{REQ:FPS}')
