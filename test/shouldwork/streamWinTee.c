//gst-launch-1.0 -v v4l2src device=/dev/video0 ! video/x-raw,format=YUY2,width=640,height=480,framerate=30/1 ! tee name=t ! queue ! videoconvert ! videoscale ! ximagesink t. ! queue ! videoconvert ! x264enc tune=zerolatency ! h264parse ! rtph264pay ! udpsink host=targetIP port=5000
#include <gst/gst.h>

int main(int argc, char *argv[]){
	GstElement *pipeline, *source, *convert, *convert2, *scale, *isink, *netsink, *filter, *enc, *parse, *pay, *tee, *qdisp, *qstream;
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
	convert2 = gst_element_factory_make("videoconvert","convert2");
	scale = gst_element_factory_make("videoscale","scale");
	isink = gst_element_factory_make("ximagesink","sink");
	tee = gst_element_factory_make("tee","tee");
	
	netsink = gst_element_factory_make("udpsink","netsink");
	enc = gst_element_factory_make("x264enc","enc");
	pay = gst_element_factory_make("rtph264pay","pay");
	parse = gst_element_factory_make("h264parse","parse");
	filter = gst_element_factory_make("capsfilter","filter");

	qdisp = gst_element_factory_make("queue","qdisp");
	qstream = gst_element_factory_make("queue","qstream");


	// Set caps and device for v4l2src
	g_object_set(filter,"caps",format,NULL);
	g_object_set(source,"device","/dev/video0",NULL);
	g_object_set(netsink,"port",5000,NULL);
	g_object_set(netsink,"host","blasty.hopto.org",NULL);
	//g_object_set(enc,"pass","qual",NULL);
	g_object_set(enc,"tune",4,NULL);
	//g_object_set(enc,"tune","zerolatency",NULL);



	//Create empty pipeline
	pipeline = gst_pipeline_new("test-pipe");

	if(!pipeline || !source || !filter || !convert || !convert2 || !tee || !qdisp || !enc || !pay || !parse || !netsink || !scale || !isink || !qstream){
		g_printerr("Not all elements could be created, failed:\n");
		if(!pipeline)g_printerr("Pipeline..\n");
		if(!filter)g_printerr("Filter..\n");
		if(!source)g_printerr("Source..\n");
		if(!convert)g_printerr("Convert..\n");
		if(!convert2)g_printerr("Convert2..\n");
		if(!enc)g_printerr("Enc..\n.");
		if(!pay)g_printerr("Pay..\n");
		if(!parse)g_printerr("Parse..\n");
		if(!netsink)g_printerr("netSink..\n");
		if(!scale)g_printerr("scale..\n");
		if(!isink)g_printerr("isink..\n");
		if(!tee)g_printerr("Tee..\n");
		if(!qdisp)g_printerr("Queue, display..\n");
		if(!qstream)g_printerr("Queue, stream..\n");

		return -1;
	}
	
	//Build pipeline
	gst_bin_add_many(GST_BIN(pipeline),source,filter,tee,qstream,convert2,enc,parse,pay,netsink,qdisp,convert,scale,isink,NULL);
	if(!gst_element_link_many(source,filter,tee,NULL) || !gst_element_link_many(tee,qstream,convert2,enc,parse,pay,netsink,NULL) || !gst_element_link_many(tee,qdisp,convert,scale,isink,NULL)){
		g_printerr("Elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	// Modify sources properties?

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
