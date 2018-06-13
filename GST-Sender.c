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

int main (int argc, char *argv[]) {
  GMainLoop *loop;

  GstElement *pipeline, *source, *videox, *converter, *encoder,*payLoader, *sink;
  GstBus *bus;
  guint bus_watch_id;

  /* Initialisation */
  gst_init (&argc, &argv);

  loop = g_main_loop_new (NULL, FALSE);

  /* Create gstreamer elements */
  pipeline =  gst_pipeline_new         ("videoStreamer-player");
  source   =  gst_element_factory_make ("videotestsrc",       "source");
  videox =   gst_element_factory_make("video/x-raw",         "videox");
  converter = gst_element_factory_make ("videoconvert",       "converter");
  encoder =   gst_element_factory_make ("jpegenc",            "encoder");
  payLoader = gst_element_factory_make ("rtpjpegpay",         "payLoader");
  sink     =  gst_element_factory_make ("udpsink",            "sink");

  if (!pipeline || !source || !videox || !converter || !encoder || !payLoader || !sink) {
    g_printerr ("One or more element(s) could not be created. Exiting.\n");
    return -1;
  }

  /* Set up the pipeline */

  /* we set the input filename to the source element */
  //g_object_set (G_OBJECT (source), "location", argv[1], NULL);
  g_object_set (G_OBJECT (videox), "width",  argv[1], "height", argv[2], NULL);
  g_object_set (G_OBJECT (encoder), "quality", argv[3], NULL);
  g_object_set (G_OBJECT (sink), "host", argv[4], "port", argv[5], NULL);

  /* we add a message handler */
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
  gst_object_unref (bus);

  /* we add all elements into the pipeline */
  gst_bin_add_many (GST_BIN (pipeline), source, videox, converter, encoder, payLoader, sink, NULL);

  /* we link the elements together */
  gst_element_link_many (source, videox, converter, encoder, payLoader, sink, NULL);

  /* Set the pipeline to "playing" state*/
  g_print ("Now transmitting");
  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  /* Iterate */
  g_print ("Running...\n");
  g_main_loop_run (loop);

  /* Out of the main loop, clean up nicely */
  g_print ("Returned, stopping playback\n");
  gst_element_set_state (pipeline, GST_STATE_NULL);

  g_print ("Deleting pipeline\n");
  gst_object_unref (GST_OBJECT (pipeline));
  g_source_remove (bus_watch_id);
  g_main_loop_unref (loop);

  return 0;
}

