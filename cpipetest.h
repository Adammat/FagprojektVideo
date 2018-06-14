#ifndef CPIPTEST_H
#define CPIPTEST_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int getVCL();
int getFPS();
int getRES();
int setVCL(int vcl2);
int setFPS(int fps2);
int setRES(int res2);

#endif // SENSOR_H
