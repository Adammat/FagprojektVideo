from __future__ import print_function

import sys
import socket
import os
import asyncore
import collections
import logging

MAX_MESSAGE_LENGTH = 1024


class RemoteClient(asyncore.dispatcher):

    """Wraps a remote client socket."""

    def __init__(self, host, socket, address):
        asyncore.dispatcher.__init__(self, socket)
        self.host = host
        self.outbox = collections.deque()

    def say(self, message):
        self.outbox.append(message)

    def handle_read(self):
        data = self.recv(MAX_MESSAGE_LENGTH)
	'''global VCL
        global FPS
        global RES'''
	self.host.broadcast(data)
        '''try:
            if data[0:5] == "{VCL:" and 0 < int(data[5:-1]) < 4 and data[-1:] == "}" :
                VCL = int(data[5:-1])

                #Sends all the data to the c program
                localSock.sendall(("VCL"+ str(VCL)).encode())
		recived = localSock.recv(1024).decode().strip()

                #Broadcast the message
                self.host.broadcast("{OK:VCL=" + str(VCL) + "}")

            elif data[0:5] == "{FPS:" and 0 < int(data[5:-1]) < 61 and data[-1:] == "}" :

                FPS = int(data[5:-1])

                #Sends all the data to the c program

                localSock.sendall(("FPS"+ str(FPS)).encode())
		recived = localSock.recv(1024).decode().strip()

                #Broadcast the message
                self.host.broadcast("{OK:FPS=" + str(FPS) + "}")
           
            elif data[0:5] == "{RES:" and (int(data[5:-1]) in {480, 720, 1080}) and data[-1:] == "}" :
                RES = int(data[5:-1])

                #Sends all the data to the c program
                localSock.sendall(("RES"+ str(RES)).encode())
		recived = localSock.recv(1024).decode().strip()

                #Broadcast the message
                self.host.broadcast("{OK:RES=" + str(RES) + "}")

            elif data == "{REQ:FPS}":
                #Returns the OK message to the sender
                self.say(("{OK:FPS=" + str(FPS) + "}"))

            elif data == "{REQ:VCL}":
                #Returns the OK message to the sender
                self.say(("{OK:VCL=" + str(VCL) + "}"))

            elif data == "{REQ:RES}":
                #Returns the OK message to the sender
                self.say(("{OK:RES=" + str(RES) + "}"))

            else:
                if data[0:5] == "{FPS:" and data[-1:] == "}":
                    #Returns the NOT message to the sender
                    self.say(("{NOT:FPS=" + str(FPS) + "}"))


                elif data[0:5] == "{VCL:" and data[-1:] == "}":
                    #Returns the NOT message to the sender
                    self.say(("{NOT:VCL=" + str(VCL) + "}"))

                elif data[0:5] == "{RES:" and data[-1:] == "}":
                    #Returns the NOT message to the sender
                    self.say(("{NOT:RES=" + str(RES) + "}"))

		elif data[0:5] == "{REQ:" and data[-1:] == "}":
                    #Returns the NOT message to the sender
                    self.say(("{NOT:REQ}"))

                else:
                    #Returns the ERROR message to the sender
                    self.say(("{NOT:ERROR}"))

        except ValueError: #Will be called upon failure of the int function call. This should happen when the value in given messages are not int's. Example {REQ:asdf}
            if data[0:5] == "{FPS:" and data[-1:] == "}":
                #Returns the NOT message to the sender
                self.say(("{NOT:FPS=" + str(FPS) + "}"))

            elif data[0:5] == "{VCL:" and data[-1:] == "}":
                #Returns the NOT message to the sender
                self.say(("{NOT:VCL=" + str(VCL) + "}"))

            elif data[0:5] == "{RES:" and data[-1:] == "}":
                #Returns the NOT message to the sender
                self.say(("{NOT:RES=" + str(RES) + "}"))

            elif data[0:5] == "{REQ:" and data[-1:] == "}":
                #Returns the NOT message to the sender
                self.say(("{NOT:REQ}"))

            else:
                self.say(("{NOT:ERROR}"))

        except: #Is called whenever any other error occurs. Prints the error and returns ERROR message
            print("Unexpected error:", sys.exc_info()[0])
            self.say(("{NOT:ERROR}"))'''


    def handle_write(self):
        if not self.outbox:
            return
        message = self.outbox.popleft()
        if len(message) > MAX_MESSAGE_LENGTH:
            raise ValueError('Message too long')
        self.say(message)


class Host(asyncore.dispatcher):

    log = logging.getLogger('Host')

    def __init__(self, address=('localhost', 9999)):
        asyncore.dispatcher.__init__(self)
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.bind(address)
        self.listen(1)
        self.remote_clients = []

    def handle_accept(self):
        socket, addr = self.accept() # For the remote client.
        self.log.info('Accepted client at %s', addr)
        self.remote_clients.append(RemoteClient(self, socket, addr))

    def handle_read(self):
        self.log.info('Received message: %s', self.read())


    def broadcast(self, message):
        self.log.info('Broadcasting message: %s', message)
        for remote_client in self.remote_clients:
            remote_client.say(message)

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



if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    
    SOCKADDR = "/home/feynman/workspace/FagprojektVideo/9Lq7BNBnBycd6nxy.socket"
    #Initial values 
    FPS = 60
    VCL = 1
    RES = 720

    # Create the socket (AF_UNIX is a local socket. SEQPACKET is two way communication)
    localSock = socket.socket(socket.AF_UNIX, socket.SOCK_SEQPACKET)

    try:
        # Connect to server
        localSock.connect(SOCKADDR)
    except:
        print("Couldn't connect to local socket: {}".format(SOCKADDR))
        raise

    logging.info('Creating host')
    host = Host()
    logging.info('Creating clients')
    alice = Client(('localhost', 9999), 'Alice')
    bob = Client(('localhost', 9999), 'Bob')
    bob.say('{FPS=20}')
    alice.say('{REQ:FPS}')
    logging.info('Looping')
    asyncore.loop()

''' #Code from socketserver
    HOST, PORT = "localhost", 9999
    
    # Create the server, binding to localhost on port 9999
    server = SocketServer.TCPServer((HOST, PORT), MyTCPHandler)

    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    
    server.serve_forever()'''
