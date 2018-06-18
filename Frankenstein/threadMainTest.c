#include "cpipetest.h"
#include "threadMainTest.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gst/gst.h>
#include <glib.h>



static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data)
{
  GMainLoop *loop = (GMainLoop *) data;

  switch (GST_MESSAGE_TYPE (msg)) {

    case GST_MESSAGE_EOS:
      g_print ("End of stream\n");
      g_main_loop_quit (loop);
      break;

    case GST_MESSAGE_ERROR: {
      gchar  *debug;
      GError *error;

      gst_message_parse_error (msg, &error, &debug);
      g_free (debug);

      g_printerr ("Error: %s\n", error->message);
      g_error_free (error);

      g_main_loop_quit (loop);
      break;
    }
    default:
      break;
  }

  return TRUE;
}

int main(int argc, char* argv[]) {
  Elementlist elements;
  pthread_t thread[1];
  int result_code;
  unsigned index;

  //Start the thread
  result_code = pthread_create(&thread[0], NULL, localSocket_thread,&elements);
  assert(!result_code);

  //THIS IS ONLY TO KEEP MAIN RUNNING
  GMainLoop *loop;

  GstBus *bus;
  gboolean link_ok;
  guint bus_watch_id;
  char *p;

  elements.width = strtol(argv[1],&p,10);
  elements.height = strtol(argv[2],&p,10);
  elements.vcl = strtol(argv[3],&p,10);
  int port=strtol(argv[5],&p,10);



  /* Initialisation */
  gst_init (&argc, &argv);

  loop = g_main_loop_new (NULL, FALSE);

  /* Create gstreamer elements */
  elements.pipeline =  gst_pipeline_new         ("videoStreamer-player");
  elements.source   =  gst_element_factory_make ("videotestsrc",       "source");
  elements.capsFilter = gst_element_factory_make("capsfilter",		   "capsFilter");
  elements.converter = gst_element_factory_make ("videoconvert",       "converter");
  elements.encoder =   gst_element_factory_make ("jpegenc",            "encoder");
  elements.payLoader = gst_element_factory_make ("rtpjpegpay",         "payLoader");
  elements.sink     =  gst_element_factory_make ("udpsink",            "sink");


  /*gst_caps_new_simple("video/x-raw",
                  "width", G_TYPE_INT, strtol(argv[1],&p,10),
                  "height", G_TYPE_INT, strtol(argv[2],&p,10),
                  NULL);*/
  if (!elements.pipeline || !elements.source|| !elements.capsFilter || !elements.converter || !elements.encoder || !elements.payLoader || !elements.sink) {
    g_printerr ("One or more element(s) could not be created. Exiting.\n");
  }

  /* Set up the pipeline */

  /* we set the input filename to the source element */
  g_object_set (G_OBJECT (elements.capsFilter), "caps",
  				   gst_caps_new_simple("video/x-raw",
                  "width", G_TYPE_INT, strtol(argv[1],&p,10),
                  "height", G_TYPE_INT, strtol(argv[2],&p,10),
                  NULL) , NULL);
  g_object_set (G_OBJECT (elements.source), "pattern",18, NULL);
  g_object_set (G_OBJECT (elements.encoder), "quality",elements.vcl, NULL);
  g_object_set (G_OBJECT (elements.sink), "host", argv[4], "port",(int) port, NULL);

  /* we add a message handler */
  bus = gst_pipeline_get_bus (GST_PIPELINE (elements.pipeline));
  bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
  gst_object_unref (bus);

  /* we add all elements into the pipeline */
  gst_bin_add_many (GST_BIN (elements.pipeline), elements.source,elements.capsFilter ,elements.converter, elements.encoder, elements.payLoader, elements.sink, NULL);
  //link_ok = gst_element_link_filtered(elements.source,elements.converter,elements.capsFilter);
  /* we link the elements together */
  gst_element_link_many (elements.source,elements.capsFilter,elements.converter, elements.encoder, elements.payLoader, elements.sink, NULL);

  /* Set the pipeline to "playing" state*/
  g_print ("Now transmitting\n");
  gst_element_set_state (elements.pipeline, GST_STATE_PLAYING);

  /* Iterate */
  g_print ("Running...\n");
  g_main_loop_run (loop);

  /* Out of the main loop, clean up nicely */
  g_print ("Returned, stopping playback\n");
  gst_element_set_state (elements.pipeline, GST_STATE_NULL);

  g_print ("Deleting pipeline\n");
  gst_object_unref (GST_OBJECT (elements.pipeline));
  g_source_remove (bus_watch_id);
  g_main_loop_unref (loop);

  return 0;
}

int getVCL(Elementlist *e){
	return e->vcl;
}

int getFPS(Elementlist *e){
	return e->fps;
}

int getRES(Elementlist *e){
	return e->height;
}

int setVCL(int vcl,Elementlist *e){

	g_object_set (G_OBJECT (e->encoder), "quality",(int) vcl, NULL);
	e->vcl = vcl;
	printf("Quality has been changed: %d\n",vcl);
	return 1;
}

int setFPS(int fps, Elementlist *e){

	g_object_set (G_OBJECT (e->capsFilter), "caps",
    				   gst_caps_new_simple("video/x-raw",
                    "width", G_TYPE_INT, (e->width),
                    "height", G_TYPE_INT, (e->height),
					"framerate",GST_TYPE_FRACTION,fps,1,
                    NULL) , NULL);
	e->fps = fps;

	return 1;
}

int setRES(int w,int h,Elementlist *e){
	g_object_set (G_OBJECT (e->capsFilter), "caps",
					   gst_caps_new_simple("video/x-raw",
					"width", G_TYPE_INT, w,
					"height", G_TYPE_INT, h,
					"framerate",GST_TYPE_FRACTION,(e->fps),1,
					NULL) , NULL);
	e->width = w;
	e->height = h;

	return 1;
}
