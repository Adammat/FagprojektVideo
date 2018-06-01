import socketserver

class MyTCPHandler(socketserver.BaseRequestHandler):

    
    
    """
    The request handler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """
    
    def handle(self):
        global VCL
        global FPS
        global RES
        # self.request is the TCP socket connected to the client
        self.data = self.request.recv(1024).decode().strip()
        print( "{} wrote:".format(self.client_address[0]))
        print (self.data)
        # just send back the same data, but upper-cased
        try:
            if self.data[0:5] == "{VCL:" and 0 < int(self.data[5:-1]) < 4 and self.data[-1:] == "}" :
                VCL = int(self.data[5:-1])
                self.request.sendall(("{OK:VCL=" + self.data[5:-1] + "}").encode())
            elif self.data[0:5] == "{FPS:" and 0 < int(self.data[5:-1]) < 61 and self.data[-1:] == "}" :
                FPS = int(self.data[5:-1])
                self.request.sendall(("{OK:FPS=" + self.data[5:-1] + "}").encode())            
            elif self.data[0:5] == "{RES:" and (int(self.data[5:-1]) in {480, 720, 1080}) and self.data[-1:] == "}" :
                RES = int(self.data[5:-1])
                self.request.sendall(("{OK:FPS=" + self.data[5:] + "}").encode())
            elif self.data == "{REQ:FPS}":
                self.request.sendall(("{OK:FPS=" + str(FPS) + "}").encode())
            elif self.data == "{REQ:VCL}":
                self.request.sendall(("{OK:VCL=" + str(VCL) + "}").encode())
            elif self.data == "{REQ:RES}":
                self.request.sendall(("{OK:RES=" + str(RES) + "}").encode())
            else:
                if self.data[0:5] == "{FPS:" and self.data[-1:] == "}":
                    self.request.sendall(("{NOT:FPS=" + str(FPS) + "}").encode())
                elif self.data[0:5] == "{VCL:" and self.data[-1:] == "}":
                    self.request.sendall(("{NOT:VCL=" + str(VCL) + "}").encode())
                elif self.data[0:5] == "{RES:" and self.data[-1:] == "}":
                    self.request.sendall(("{NOT:RES=" + str(RES) + "}").encode())
                else:
                    self.request.sendall(("{NOT:ERROR}").encode())
        except:
            if self.data[0:5] == "{FPS:" and self.data[-1:] == "}":
                self.request.sendall(("{NOT:FPS=" + str(FPS) + "}").encode())
            elif self.data[0:5] == "{VCL:" and self.data[-1:] == "}":
                self.request.sendall(("{NOT:VCL=" + str(VCL) + "}").encode())
            elif self.data[0:5] == "{RES:" and self.data[-1:] == "}":
                self.request.sendall(("{NOT:RES=" + str(RES) + "}").encode())
            else:
                self.request.sendall(("{NOT:ERROR}").encode())

if __name__ == "__main__":
    HOST, PORT = "localhost", 9999
        # Initial values 
    FPS = 60
    VCL = 1
    RES = 720
    # Create the server, binding to localhost on port 9999
    server = socketserver.TCPServer((HOST, PORT), MyTCPHandler)

    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    
    server.serve_forever()