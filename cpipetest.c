#include "threadMainTest.h" //TODO Must be exchanged wih actual working.h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_NAME "/home/feynman/workspace/FagprojektVideo/9Lq7BNBnBycd6nxy.socket" //TODO Skal ændres
#define BUFFER_SIZE 3

enum cmd{ERR,OK,REQ,VCL,FPS,RES};


void* localSocket_thread(){
    struct sockaddr_un name;
    int down_flag = 0;
    int ret;
    int connection_socket;
    int data_socket;
    int result;
    char buffer[BUFFER_SIZE];
    enum cmd cmd;
	int value;
	int resArrHeight[3] = {480,720,1080}; //Kan også skrives som 480*1.5^i, hvor i er index
	int resArrWidth[3] = {640,1280,1920}; //Kan også skrives som 640*(i+1), hvor i er index

    /*
     * In case the program exited inadvertently on the last run,
     * remove the socket.
     */

    unlink(SOCKET_NAME);

    /* Create local socket. */

    connection_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (connection_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /*
     * For portability clear the whole structure, since some
     * implementations have additional (nonstandard) fields in
     * the structure.
     */

    memset(&name, 0, sizeof(struct sockaddr_un));

    /* Bind socket to socket name. */

    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);

    ret = bind(connection_socket, (const struct sockaddr *) &name,
               sizeof(struct sockaddr_un));
    if (ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /*
     * Prepare for accepting connections. The backlog size is set
     * to 20. So while one request is being processed other requests
     * can be waiting.
     */

    ret = listen(connection_socket, 20);
    if (ret == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    /* Wait for incoming connection. */
    for(;;){
    	data_socket = accept(connection_socket, NULL, NULL);
    	if (data_socket == -1) {
    		perror("accept");
    		exit(EXIT_FAILURE);
    	}
    	/* This is the main loop for handling connections. */

		for(;;) {

			printf("vcl: %i, fps: %i, res %i\n", getVCL(),getFPS(),getRES());
			fflush(stdout);
			/* Wait for next data packet. */
			memset(buffer, '\0', BUFFER_SIZE);
			ret = read(data_socket, buffer, BUFFER_SIZE);
			if (ret == -1) {
				perror("read");
				exit(EXIT_FAILURE);
			}

			/* Ensure buffer is 0-terminated. */

			buffer[BUFFER_SIZE - 1] = 0;


			printf("Recived data: %s\n", buffer);
			fflush(stdout);

			/* Readying bit values */

	        cmd = buffer[0];
	        value = buffer[1];

	        printf("cmd: %d, value: %i\n", cmd,value);
	        fflush(stdout);

	        memset(buffer, '\0', BUFFER_SIZE);
			/* Handle commands. */

	        switch(cmd){
	        case ERR: //This is only called when the socket is closed from client
	        	goto LOOPBREAK; //HACK
	        	break;
	        case REQ:
	        	switch(value){
	        	case VCL:
	        		sprintf(buffer, "%c%c", VCL, getVCL());
	        		break;
	        	case FPS:
	        		sprintf(buffer, "%c%c", FPS, getFPS());
	        		break;
	        	case RES:
	        		sprintf(buffer, "%c%c", RES, (getRES()-480)/240); //HACK Converts 480,720,1080 into 0,1,2
//	        		switch(getRES())
//	        		case 480:
//	        			sprintf(buffer, "%c%c", RES, 0);
//	        			break;
//	        		case 720:
//	        			sprintf(buffer, "%c%c", RES, 1);
//	        			break;
//	        		case 1080:
//	        			sprintf(buffer, "%c%c", RES, 2);
//	        		default:
//	        			sprintf(buffer, "%c", ERR);
	        		break;
	        	default:
		        	sprintf(buffer, "%c", ERR);
	        	}
	        	break;
	        case VCL:
	        	sprintf(buffer, "%c", setVCL(value));
	        	break;
	        case FPS:
	        	sprintf(buffer, "%c", setFPS(value));
	        	break;
	        case RES:
	        	sprintf(buffer, "%c", setRES(resArrHeight[value]));
	        	break;
	        default:
	        	sprintf(buffer, "%c", ERR);
	        }


			/* Send result. */
			printf("Sending data: %s\n", buffer);
			fflush(stdout);
			ret = write(data_socket, buffer, BUFFER_SIZE);

			if (ret == -1) {
				perror("send");
				exit(EXIT_FAILURE);
			}

		}

		LOOPBREAK:close(data_socket);
	}

    /* Unlink the socket. */

    unlink(SOCKET_NAME);

    return NULL;
}



