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
int vcl = 1;
int fps = 60;
int res = 720;

int main(int argc, char** argv) {
  Elementlist elements;
  pthread_t thread[1];
  int result_code;
  unsigned index;

  //Start the thread
  result_code = pthread_create(&thread[0], NULL, localSocket_thread,&elements);
  assert(!result_code);

  //THIS IS ONLY TO KEEP MAIN RUNNING
  GMainLoop *loop;

  GstCaps *capsFilter;
  GstBus *bus;
  gboolean link_ok;
  guint bus_watch_id;
  char *p;
  gint quality = strtol(argv[3],&p,10);
  gint port = strtol(argv[5],&p,10);

  quality=strtol(argv[3],&p,10);


  printf("%i, %i\n",quality,port);
  /* Initialisation */
  gst_init (&argc, &argv);

  loop = g_main_loop_new (NULL, FALSE);

  /* Create gstreamer elements */
  elements.pipeline =  gst_pipeline_new         ("videoStreamer-player");
  elements.source   =  gst_element_factory_make ("videotestsrc",       "source");
  elements.capsFilter = gst_caps_new_simple("video/x-raw",
                  "width", G_TYPE_INT, strtol(argv[1],&p,10),
                  "height", G_TYPE_INT, strtol(argv[2],&p,10),
                  NULL);
  elements.converter = gst_element_factory_make ("videoconvert",       "converter");
  elements.encoder =   gst_element_factory_make ("jpegenc",            "encoder");
  elements.payLoader = gst_element_factory_make ("rtpjpegpay",         "payLoader");
  elements.sink     =  gst_element_factory_make ("udpsink",            "sink");

  if (!elements.pipeline || !elements.source|| !elements.converter || !elements.encoder || !elements.payLoader || !elements.sink) {
    g_printerr ("One or more element(s) could not be created. Exiting.\n");
  }

  /* Set up the pipeline */

  /* we set the input filename to the source element */
  //g_object_set (G_OBJECT (source), "location", argv[1], NULL);

  g_object_set (G_OBJECT (elements.encoder), "quality",(int) quality, NULL);
  g_object_set (G_OBJECT (elements.sink), "host", argv[4], "port",(int) port, NULL);

  /* we add a message handler */
  bus = gst_pipeline_get_bus (GST_PIPELINE (elements.pipeline));
  bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
  gst_object_unref (bus);

  /* we add all elements into the pipeline */
  gst_bin_add_many (GST_BIN (elements.pipeline), elements.source, elements.converter, elements.encoder, elements.payLoader, elements.sink, NULL);
  link_ok = gst_element_link_filtered(elements.source,elements.converter,capsFilter);
  /* we link the elements together */
  gst_element_link_many (elements.converter, elements.encoder, elements.payLoader, elements.sink, NULL);

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

int getVCL(){
	return vcl;
}

int getFPS(){
	return fps;
}

int getRES(){
	return res;
}

int setVCL(int vcl2,Elementlist *e){

	g_object_set (G_OBJECT (e->encoder), "quality",(int) vcl2, NULL);
	vcl = vcl2;
	printf("Quality has been changed: %d\n",vcl2);
	return 1;
}

int setFPS(int fps2){
	fps = fps2;
	return 1;
}

int setRES(int res2){
	res = res2;
	return 1;
}
