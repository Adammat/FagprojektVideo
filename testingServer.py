import sys
import socket
import os
import asyncore
import collections
import logging
import unittest

MAX_MESSAGE_LENGTH = 1024

def sendRecive(data,sock):
    received = "ERROR"

    # Send data
    sock.sendall((data + "\n").encode())
    
        # Receive data from the server and shut down
    received = sock.recv(1024).decode().strip()

    return received




class SendReciveTestCase(unittest.TestCase):    
    def setUp(self):
        self.alice = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.alice.connect(('localhost', 9999))
        sendRecive("{FPS:60}", self.alice)
        sendRecive("{VCL:1}", self.alice)
        sendRecive("{RES:720}", self.alice)

    def tearDown(self):
        self.alice.close()
 
    def test_REQ_FPS(self):
        self.assertEqual(sendRecive("{REQ:FPS}",self.alice),"{FPS=60}")
        
    def test_REQ_VCL(self):
        self.assertEqual(sendRecive("{REQ:VCL}",self.alice),"{VCL=1}")    
    
    def test_REQ_RES(self):
        self.assertEqual(sendRecive("{REQ:RES}",self.alice),"{RES=720}")
        
    def test_REQ_Error(self): 
        self.assertEqual(sendRecive("{REQ:MONEY}",self.alice),"{NOT:REQ}")
        self.assertEqual(sendRecive("{REQ:RES",self.alice) ,"{NOT:FORMATERR}")
        self.assertEqual(sendRecive("{ REQ:FPS}",self.alice),"{NOT:UNKNOWNCMD}")
    
    def test_FPS_Update(self):
        self.assertEqual(sendRecive("{FPS:50}",self.alice) ,"{FPS=50}")
        self.assertEqual(sendRecive("{REQ:FPS}",self.alice),"{FPS=50}")
    
    def test_VCL_Update(self):
        self.assertEqual(sendRecive("{VCL:3}",self.alice)  ,"{VCL=3}")
        self.assertEqual(sendRecive("{REQ:VCL}",self.alice),"{VCL=3}")
        
    def test_RES_Update(self):
        self.assertEqual(sendRecive("{RES:1080}",self.alice),"{RES=1080}")
        self.assertEqual(sendRecive("{REQ:RES}",self.alice) ,"{RES=1080}")

    def test_FPS_OutOfBounds(self):
        #Lower limit
        self.assertEqual(sendRecive("{FPS:-50}",self.alice) ,"{NOT:FPS=60}")
        self.assertEqual(sendRecive("{REQ:FPS}",self.alice) ,"{FPS=60}")
        #Upper limit
        self.assertEqual(sendRecive("{FPS:160}",self.alice) ,"{NOT:FPS=60}")
        self.assertEqual(sendRecive("{REQ:FPS}",self.alice) ,"{FPS=60}")
        
    def test_VCL_OutOfBounds(self):
        #Lower limit
        self.assertEqual(sendRecive("{VCL:0}",self.alice) ,"{NOT:VCL=1}")
        self.assertEqual(sendRecive("{REQ:VCL}",self.alice),"{VCL=1}")
        #Upper limit
        self.assertEqual(sendRecive("{VCL:101}",self.alice)  ,"{NOT:VCL=1}")
        self.assertEqual(sendRecive("{REQ:VCL}",self.alice),"{VCL=1}")
        
    def test_RES_OutOfBounds(self):
        #Out of the 480, 720, 1080 scope
        self.assertEqual(sendRecive("{RES:2160}",self.alice),"{NOT:RES=720}")
        self.assertEqual(sendRecive("{REQ:RES}",self.alice) ,"{RES=720}")
    
    
    def test_FPS_Error(self):
        #Command reconised, wrong value
        self.assertEqual(sendRecive("{FPS:abc}",self.alice) ,"{NOT:FPS=60}")
        self.assertEqual(sendRecive("{REQ:FPS}",self.alice) ,"{FPS=60}")
        #Unreconized command (Missing '}')
        self.assertEqual(sendRecive("{FPS:50",self.alice)   ,"{NOT:FORMATERR}")
        self.assertEqual(sendRecive("{REQ:FPS}",self.alice) ,"{FPS=60}")
        #Unreconized command (Extra space)
        self.assertEqual(sendRecive("{ FPS:50}",self.alice)  ,"{NOT:UNKNOWNCMD}")
        self.assertEqual(sendRecive("{REQ:FPS}",self.alice) ,"{FPS=60}")
        
    def test_VCL_Error(self):
        #Command reconised, wrong value
        self.assertEqual(sendRecive("{VCL:abc}",self.alice) ,"{NOT:VCL=1}")
        self.assertEqual(sendRecive("{REQ:VCL}",self.alice) ,"{VCL=1}")
        #Unreconized command (Missing '}')
        self.assertEqual(sendRecive("{VCL:50",self.alice)   ,"{NOT:FORMATERR}")
        self.assertEqual(sendRecive("{REQ:VCL}",self.alice) ,"{VCL=1}")
        #Unreconized command (Extra space)
        self.assertEqual(sendRecive("{ VCL:3}",self.alice)   ,"{NOT:UNKNOWNCMD}")
        self.assertEqual(sendRecive("{REQ:VCL}",self.alice) ,"{VCL=1}")

    def test_RES_Error(self):
        #Command reconised, wrong value
        self.assertEqual(sendRecive("{RES:abc}",self.alice) ,"{NOT:RES=720}")
        self.assertEqual(sendRecive("{REQ:RES}",self.alice) ,"{RES=720}")
        #Unreconized command (Missing '}')
        self.assertEqual(sendRecive("{RES:480",self.alice)   ,"{NOT:FORMATERR}")
        self.assertEqual(sendRecive("{REQ:RES}",self.alice) ,"{RES=720}")
        #Unreconized command (Extra space)
        self.assertEqual(sendRecive("{ RES:480}",self.alice)  ,"{NOT:UNKNOWNCMD}")
        self.assertEqual(sendRecive("{REQ:RES}",self.alice) ,"{RES=720}")        

    def test_FPS_WeirdInt(self): #TODO Might need to be changed
        self.assertEqual(sendRecive("{FPS:0050}",self.alice) ,"{FPS=50}")
        self.assertEqual(sendRecive("{REQ:FPS}",self.alice),"{FPS=50}")
        
    def test_VCL_WeirdInt(self): #TODO Might need to be changed
        self.assertEqual(sendRecive("{VCL:03}",self.alice)  ,"{VCL=3}")
        self.assertEqual(sendRecive("{REQ:VCL}",self.alice),"{VCL=3}")
        
    def test_RES_WeirdInt(self): #TODO Might need to be changed
        self.assertEqual(sendRecive("{RES:0001080}",self.alice),"{RES=1080}")
        self.assertEqual(sendRecive("{REQ:RES}",self.alice) ,"{RES=1080}")

    def test_Broadcasting(self): 
        # Creating second user
        bob = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        bob.connect(('localhost', 9999))

        # Sending REQ, to make sure these aren't broadcasted
        self.assertEqual(sendRecive("{REQ:RES}",self.alice) ,"{RES=720}")
        self.assertEqual(sendRecive("{REQ:VCL}",self.alice),"{VCL=1}")
        self.assertEqual(sendRecive("{REQ:FPS}",self.alice),"{FPS=60}")

        # FPS broadcast test
        self.assertEqual(sendRecive("{FPS:50}",self.alice) ,"{FPS=50}")
        self.assertEqual(bob.recv(1024).decode().strip(),"{FPS=50}")

        # VCL broadcast test
        self.assertEqual(sendRecive("{VCL:50}",self.alice) ,"{VCL=50}")
        self.assertEqual(bob.recv(1024).decode().strip(),"{VCL=50}")

        # RES broadcast test
        self.assertEqual(sendRecive("{RES:1080}",self.alice) ,"{RES=1080}")
        self.assertEqual(bob.recv(1024).decode().strip(),"{RES=1080}")

        #Clearing up
        bob.close()
 
        
if __name__ == '__main__':
#    bob = Client(('localhost', 9999), 'Bob')
    unittest.main()  
