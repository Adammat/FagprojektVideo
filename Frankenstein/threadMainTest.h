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
  GstElement *capsFilter;
  GstElement *encoder;
  GstElement *payLoader;
  GstElement *sink;
  int fps;
  int vcl;
  int height;
  int width;
};
typedef struct elmts Elementlist;
int getVCL();
int getFPS();
int getRES();
int setVCL(int vcl2, Elementlist *e);
int setFPS(int fps2, Elementlist *e);
int setRES(int w,int h);

#endif 
