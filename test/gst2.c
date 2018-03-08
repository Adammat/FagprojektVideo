#include <gst/gst.h>

int main(int argc, char *argv[]){
	GstElement *pipeline, *source, *convert, *scale, *sink, *filter;
	GstCaps *format;
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;

	//Initialize Gstreamer:
	gst_init(&argc, &argv);

	//Create elements:
	source = gst_element_factory_make("v4l2src","source");
	format = gst_caps_from_string("video/x-raw,format=YUY2,width=640,height=480,framerate=30/1");
	convert = gst_element_factory_make("videoconvert","convert");
	scale = gst_element_factory_make("videoscale","scale"),
	sink = gst_element_factory_make("ximagesink","sink");

	filter = gst_element_factory_make("capsfilter","filter");


	// Set caps and device for v4l2src
	g_object_set(filter,"caps",format,NULL);
	g_object_set(source,"device","/dev/video0",NULL);
	//Create empty pipeline
	pipeline = gst_pipeline_new("test-pipe");

	if(!pipeline || !source || !convert || !scale || !sink){
		g_printerr("Not all elements could be created, failed:\n");
		if(!pipeline)g_printerr("Pipeline..\n");
		if(!source)g_printerr("Source..\n");
		if(!convert)g_printerr("Convert..\n.");
		if(!scale)g_printerr("Scale..\n");
		if(!sink)g_printerr("Sink..\n");
		return -1;
	}
	
	//Build pipeline
	gst_bin_add_many(GST_BIN(pipeline),source,convert,scale,sink,NULL);
	if(gst_element_link_many(source,convert,scale,sink,NULL) != TRUE){
		g_printerr("Elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	// Modify sources properties?
	g_object_set(source, "pattern", 0, NULL);

	//Start playing
	ret = gst_element_set_state(pipeline,GST_STATE_PLAYING);
	if(ret==GST_STATE_CHANGE_FAILURE){
		g_printerr("unable to set the pipeline to playing state.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	//Wait untill error or EOS
	bus = gst_element_get_bus(pipeline);
	msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

	//Parse msg:
	if(msg != NULL){
		GError *err;
		gchar *debug_info;
		switch(GST_MESSAGE_TYPE(msg)){
			case GST_MESSAGE_ERROR:
				gst_message_parse_error(msg, &err, &debug_info);
				g_printerr("Error recieved from element %s: %s\n", GST_OBJECT_NAME(msg->src),err->message);
				g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
				g_clear_error(&err);
				g_free(debug_info);
				break;
			case GST_MESSAGE_EOS:
				g_print("End of stream reached.\n");
				break;
			default:
				// Should not be reached...
				g_printerr("Unexpected message...\n");
				break;
		}
		gst_message_unref(msg);
	}
	//Free resources:
	gst_object_unref(bus);
	gst_element_set_state(pipeline,GST_STATE_NULL);
	gst_object_unref(pipeline);
	return 0;
}
