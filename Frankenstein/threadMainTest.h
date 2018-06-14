#ifndef THREADMAINTEST_H
#define THREADMAINTEST_H
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gst/gst.h>
#include <glib.h>
struct elmts {
  GstElement *pipeline;
  GstElement *source;
  GstElement *converter;
  GstCaps *capsFilter;
  GstElement *encoder;
  GstElement *payLoader;
  GstElement *sink;
};
typedef struct elmts Elementlist;
int getVCL();
int getFPS();
int getRES();
int setVCL(int vcl2, Elementlist *e);
int setFPS(int fps2);
int setRES(int res2);

#endif // SENSOR_H
