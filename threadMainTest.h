#ifndef THREADMAINTEST_H
#define THREADMAINTEST_H
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int getVCL();
int getFPS();
int getRES();
int setVCL(int vcl2);
int setFPS(int fps2);
int setRES(int res2);

#endif // SENSOR_H
