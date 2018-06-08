#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_NAME "/home/feynman/workspace/FagprojektVideo/9Lq7BNBnBycd6nxy.socket"
#define BUFFER_SIZE 2



int
main(int argc, char *argv[])
{
    struct sockaddr_un name;
    int down_flag = 0;
    int ret;
    int connection_socket;
    int data_socket;
    int result;
    char buffer[BUFFER_SIZE];
    char cmd,value;

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

			/* Wait for next data packet. */
			memset(buffer, '\0', BUFFER_SIZE);
			ret = read(data_socket, buffer, BUFFER_SIZE);
			if (ret == -1) {
				perror("read");
				exit(EXIT_FAILURE);
			}

			/* Ensure buffer is 0-terminated. */

			buffer[BUFFER_SIZE - 1] = 0;

			if (!strncmp(buffer, "", BUFFER_SIZE)) {
				printf("Breaking\n");
				fflush(stdout);
				break;
			}

			printf("Recived data: %s\n", buffer);
			fflush(stdout);

			/* Readying bit values */

	//        cmd = buffer[0] & 0x03;
	//        value = (buffer[0] & 0xFC)>>2;
	//
	//        printf("cmd: %c, value: %c\n", cmd,value);
	//        fflush(stdout);

			/* Handle commands. */

	//        if ((int)cmd == 1) {
	//            //TODO VCL CODE
	//        } else if ((int)cmd == 2) {
	//            //TODO FPS CODE
	//        } else if ((int)cmd == 3) {
	//        	//TODO RES CODE
	//        } else {
	//        	//TODO ERROR CODE
	//        }



			/* Send result. */
			memset(buffer, '\0', BUFFER_SIZE);
			sprintf(buffer, "%c", '1');
			printf("Sending data: %s\n", buffer);
			fflush(stdout);
			ret = write(data_socket, buffer, BUFFER_SIZE);

			if (ret == -1) {
				perror("read");
				exit(EXIT_FAILURE);
			}

		}

		close(connection_socket);
	}

    /* Unlink the socket. */

    unlink(SOCKET_NAME);

    exit(EXIT_SUCCESS);
}
