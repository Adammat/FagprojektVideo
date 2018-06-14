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

    def rdymsg(self,command, value):
        return command + chr(value)

    def handle_read(self):
        recv_message = self.recv(MAX_MESSAGE_LENGTH)
        print("Full message recived: {}".format(recv_message))
        dataList = recv_message.split('}')
        if(len(dataList) == 1 and dataList[0] != ''):
            self.say(("{NOT:FORMATERR}"))
        #global localSock
        for data in dataList[:-1]:
            data = data + "}"
            print("Data recived: {}".format(data))
            try:
                if data[0:5] == "{VCL:" and 0 < int(data[5:-1]) < 101 and data[-1:] == "}" :
                    vcl = int(data[5:-1])
    
                    #Sends all the data to the c program
                    localSock.sendall(self.rdymsg(cVCL,vcl).encode())
    		    recived = localSock.recv(1024).decode().strip()

                    #Broadcast the message
                    if recived[0] == cOK :
                        self.host.broadcast("{VCL=" + str(vcl) + "}")
                    else:
                        #Server sent wrong message
                        self.say(("{NOT:SERVERERROR}")) 

                elif data[0:5] == "{FPS:" and 0 < int(data[5:-1]) < 61 and data[-1:] == "}" :

                    fps = int(data[5:-1])

                    #Sends all the data to the c program
                    localSock.sendall(self.rdymsg(cFPS,fps).encode())
    		    recived = localSock.recv(1024).decode().strip()

                    if recived[0] == cOK :
                        #Broadcast the message
                        self.host.broadcast("{FPS=" + str(fps) + "}")
                    else:
                        #Server sent wrong message
                        self.say(("{NOT:SERVERERROR}")) 
                   
                elif data[0:5] == "{RES:" and (int(data[5:-1]) in {480, 720, 1080}) and data[-1:] == "}" :
                    res = int(data[5:-1])
                    resindex = [480,720,1080].index(res)
                        #Sends all the data to the c program

                    localSock.sendall(self.rdymsg(cRES,resindex).encode())
    		    recived = localSock.recv(1024).decode().strip()
    
                    if recived[0] == cOK :
                        #Broadcast the message    
                        self.host.broadcast("{RES=" + str(res) + "}")
                    else:
                        #Server sent wrong message
                        self.say(("{NOT:SERVERERROR}")) 
                elif data == "{REQ:VCL}":
                    

                    #Asks for data from the c program
                    localSock.sendall((cREQ+cVCL).encode())
    		    recived = localSock.recv(1024).decode().strip()
    
                    if recived[0] == cVCL :
                        #Returns the OK message to the sender
                        self.say(("{VCL=" + str(ord(recived[1])) + "}"))
                    else:
                        #Server sent wrong message
                        self.say(("{NOT:SERVERERROR}")) 

                elif data == "{REQ:FPS}":
                    #Asks for data from the c program
                    localSock.sendall((cREQ+cFPS).encode())
    		    recived = localSock.recv(1024).decode().strip()

                    #Returns the OK message to the sender
                    if recived[0] == cFPS :
                        #Returns the OK message to the sender
                        self.say(("{FPS=" + str(ord(recived[1])) + "}"))
                    else:
                        #Server sent wrong message
                        self.say(("{NOT:SERVERERROR}")) 


                elif data == "{REQ:RES}":
                    
                    #Asks for data from the c program
                    localSock.sendall((cREQ + cRES).encode())
    		    recived = localSock.recv(1024).decode().strip()

                    if recived[0] == cRES:
                        #Returns the OK message to the sender
                        self.say(("{RES=" + str(ord(recived[1])) + "}"))
                    else:
                        #Server sent wrong message
                        self.say(("{NOT:SERVERERROR}")) 
    
                else:
                    if data[0:5] == "{FPS:" and data[-1:] == "}": 
                        #Returns the NOT message to the sender
                        self.say(("{NOT:FPS=" + str(fps) + "}"))
        
                    elif data[0:5] == "{VCL:" and data[-1:] == "}":
                        #Returns the NOT message to the sender
                        self.say(("{NOT:VCL=" + str(vcl) + "}"))
    
                    elif data[0:5] == "{RES:" and data[-1:] == "}":
                        #Returns the NOT message to the sender
                        self.say(("{NOT:RES=" + str(res) + "}"))
    
          	    elif data[0:5] == "{REQ:" and data[-1:] == "}":
                            #Returns the NOT message to the sender
                        self.say(("{NOT:REQ}"))
    
                    else:
                            #Returns the ERROR message to the sender
                        self.say(("{NOT:UNKNOWNCMD}"))
    
            except ValueError: #Will be called upon failure of the int function call. This should happen when the value in given messages are not int's. Example {REQ:asdf}
                if data[0:5] == "{FPS:" and data[-1:] == "}":
                       #Returns the NOT message to the sender
                    self.say(("{NOT:FPS=" + str(fps) + "}"))
    
                elif data[0:5] == "{VCL:" and data[-1:] == "}":
                    #Returns the NOT message to the sender
                    self.say(("{NOT:VCL=" + str(vcl) + "}"))
    
                elif data[0:5] == "{RES:" and data[-1:] == "}":
                        #Returns the NOT message to the sender
                    self.say(("{NOT:RES=" + str(res) + "}"))
    
                elif data[0:5] == "{REQ:" and data[-1:] == "}":
                        #Returns the NOT message to the sender
                    self.say(("{NOT:REQ}"))
    
                else:
                    self.say(("{NOT:ERROR}"))
    
            except: #Is called whenever any other error occurs. Prints the error and returns ERROR message
                print("Unexpected error:", sys.exc_info()[0])
                self.say(("{NOT:ERROR}"))


    def handle_write(self):
        if not self.outbox:
            return
        message = self.outbox.popleft()
        if len(message) > MAX_MESSAGE_LENGTH:
            raise ValueError('Message too long')
        print("Sending message: {}, to {}".format(message, self.getpeername())) 
        self.sendall(message)

class Host(asyncore.dispatcher):

    log = logging.getLogger('Host')

    def __init__(self, address):
        asyncore.dispatcher.__init__(self)
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)
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


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    
    SOCKADDR = "/home/feynman/workspace/FagprojektVideo/9Lq7BNBnBycd6nxy.socket"
    #Initial values 
    HOST, PORT = 'localhost', 9999

    #Define commands (ENUM)
    cERR, cOK, cREQ, cVCL, cFPS, cRES = chr(0), chr(1), chr(2),chr(3),chr(4),chr(5) 

    # Create the socket (AF_UNIX is a local socket. SEQPACKET is two way communication)
    localSock = socket.socket(socket.AF_UNIX, socket.SOCK_SEQPACKET)

    try:
        # Connect to server
        localSock.connect(SOCKADDR)
    except:
        print("Couldn't connect to local socket: {}".format(SOCKADDR))
        raise

    logging.info('Creating host')
    host = Host((HOST,PORT))
    logging.info('Host Socketname: %s', host.getsockname())
    '''logging.info('Creating clients')
    alice = Client(('localhost', 9999), 'Alice')
    bob = Client(('localhost', 9999), 'Bob')
    bob.say('{FPS=20}')
    alice.say('{REQ:FPS}')'''
    logging.info('Looping')
    asyncore.loop()

''' #Code from socketserver
    HOST, PORT = "localhost", 9999
    
    # Create the server, binding to localhost on port 9999
    server = SocketServer.TCPServer((HOST, PORT), MyTCPHandler)

    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    
    server.serve_forever()'''


