/* This assumes you have the GStreamer headers on your include path
or in the same folder as this source file */
#include <gst/gst.h>


/**
 * @brief      Display video stream from the solo 3DR drone with GStreamer library
 *			   Previously need to request video stream with TCP
 *
 * @return     0
 */
int main(int argc, char *argv[])
{
    
	GstElement *pipeline;
    GstBus *bus;
	GstStateChangeReturn ret;
    GMainLoop *loop;

	/* Initialize GStreamer */
    gst_init(&argc, &argv);

	/* Build the pipeline */
    pipeline = gst_parse_launch("-v udpsrc port=5600 caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink", NULL);
	bus = gst_element_get_bus(pipeline);
	
	/* Start playing */
	ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) 
	{
      g_printerr ("Unable to set the pipeline to the playing state.\n");
      gst_object_unref (pipeline);
      return -1;
    }
	
	/* Loop */
	loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);
	
    /* Free resources */
    g_main_loop_unref (loop);
    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
    return 0;
}