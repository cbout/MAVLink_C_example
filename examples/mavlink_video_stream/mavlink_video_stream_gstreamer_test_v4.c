#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/* This assumes you have the GStreamer headers on your include path
or in the same folder as this source file */
#include <gst/gst.h>


/**
 * @brief      Display video stream from the solo 3DR drone with GStreamer library, initialize the TCP request data stream before and register video in a file
 *			   This program is an extension of the program mavlink_video_stream_gstreamer_v3_test.c
 *
 * @return     0
 */
int main(int argc, char *argv[])
{
    
	GstElement *pipeline;
    GstBus *bus;
	GstStateChangeReturn ret;
    GMainLoop *loop;

	int s;
	struct sockaddr_in serv_addr;
	

	/* Initialize a TCP connection in order to request video stream */
	s = socket(PF_INET, SOCK_STREAM, 0);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr ("10.1.1.1");	// Address of the server
	serv_addr.sin_port = htons (5502);	// Port of the server
	memset (&serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero));
	
	connect (s, (struct sockaddr *)&serv_addr, sizeof serv_addr);

	
	/* Initialize GStreamer */
    gst_init(&argc, &argv);

	/* Build the pipeline */
	//.bmp files
	//pipeline = gst_parse_launch("-v udpsrc port=5600 caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin  ! tee name=t ! queue ! videorate ! avenc_bmp ! multifilesink location=\"frame/bmp/frame%06d.bmp\" t. ! queue ! videoconvert ! autovideosink t. ! queue ! avimux ! filesink location=capture.avi", NULL);
	//.jpg files
    pipeline = gst_parse_launch("-v udpsrc port=5600 caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin  ! tee name=t ! queue ! videorate ! jpegenc ! multifilesink location=\"frame/jpg/frame%06d.jpg\" t. ! queue ! videoconvert ! autovideosink t. ! queue ! avimux ! filesink location=capture.avi", NULL);
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