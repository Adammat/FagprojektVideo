//gst-launch-1.0 -v udpsrc port=5000 ! "application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)H264,packetization-mode=(string)1" ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! videoscale ! ximagesink
#include <gst/gst.h>

int main(int argc, char *argv[]){
	GstElement *pipeline, *source, *convert, *scale, *isink, *filter, *dec, *parse, *depay;
	GstCaps *format;
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;

	//Initialize Gstreamer:
	gst_init(&argc, &argv);

	//Create elements:
	source = gst_element_factory_make("udpsrc","source");
	format = gst_caps_from_string("application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)H264,packetization-mode=(string)1");
	convert = gst_element_factory_make("videoconvert","convert");
	scale = gst_element_factory_make("videoscale","scale");
	isink = gst_element_factory_make("ximagesink","isink");
	dec = gst_element_factory_make("avdec_h264","dec");
	depay = gst_element_factory_make("rtph264depay","depay");
	parse = gst_element_factory_make("h264parse","parse");
	filter = gst_element_factory_make("capsfilter","filter");


	// Set caps and other flags
	g_object_set(filter,"caps",format,NULL);
	g_object_set(source,"port",5000,NULL);



	//Create empty pipeline
	pipeline = gst_pipeline_new("test-pipe");

	if(!pipeline || !source || !filter || !depay || !parse || !dec || !convert || !scale || !isink){
		g_printerr("Not all elements could be created, failed:\n");
		if(!pipeline)g_printerr("Pipeline..\n");
		if(!filter)g_printerr("Filter..\n");
		if(!source)g_printerr("Source..\n");
		if(!convert)g_printerr("Convert..\n");
		if(!dec)g_printerr("Enc..\n.");
		if(!depay)g_printerr("Pay..\n");
		if(!parse)g_printerr("Parse..\n");
		if(!scale)g_printerr("Scale..\n");
		if(!isink)g_printerr("imageSink..\n");
		return -1;
	}
	
	//Build pipeline
	gst_bin_add_many(GST_BIN(pipeline),source,filter,depay,parse,dec,convert,scale,isink,NULL);
	if(gst_element_link_many(source,filter,depay,parse,dec,convert,scale,isink,NULL) != TRUE){
		g_printerr("Elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	// Modify sources properties?
	//g_object_set(source, "pattern", 0, NULL);

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
