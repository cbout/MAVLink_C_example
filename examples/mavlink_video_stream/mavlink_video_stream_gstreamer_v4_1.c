/**********************************************************************************************************************************************************
 Copyright  ETIS — ENSEA, Université de Cergy-Pontoise, CNRS (1991-2018)
 promethe@ensea.fr


 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! THIS PROGRAM IS NOT TESTED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 Short program to display video stream from the solo 3DR drone with GStreamer library, initialize the TCP request data stream before and register video in a file and frames in differents files.
 This program is an extension of the program mavlink_video_stream_gstreamer_v3_test.c
 This program do exactly the same than the previous one, it only add the functionality to register each frames of the stream on files.
 To do it, we only add to the string to parse, the image encoding and the "multifilesink" to files.

 This software is governed by the CeCILL v2.1 license under French law and abiding by the rules of distribution of free software.
 You can use, modify and/ or redistribute the software under the terms of the CeCILL v2.1 license as circulated by CEA, CNRS and INRIA at the following URL "http://www.cecill.info".
 As a counterpart to the access to the source code and  rights to copy, modify and redistribute granted by the license,
 users are provided only with a limited warranty and the software's author, the holder of the economic rights,  and the successive licensors have only limited liability.
 In this respect, the user's attention is drawn to the risks associated with loading, using, modifying and/or developing or reproducing the software by the user in light of its specific status of free software,
 that may mean  that it is complicated to manipulate, and that also therefore means that it is reserved for developers and experienced professionals having in-depth computer knowledge.
 Users are therefore encouraged to load and test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data to be ensured
 and, more generally, to use and operate it in the same conditions as regards security.
 The fact that you are presently reading this means that you have had knowledge of the CeCILL v2.1 license and that you accept its terms.
 

 See more details about this program in the file : documentation_video_stream.
 
 
 Compilation : 
 cd /mavlink_video_stream
 gcc mavlink_video_stream_gstreamer_v4_1.c -o mavlink_video_stream_gstreamer_v4_1 `pkg-config --cflags --libs gstreamer-1.0`
 
 Execution : 
 ./mavlink_video_stream_gstreamer_v4_1

**********************************************************************************************************************************************************/
/*********************************************************************************************************************************************************
 Authors: Raphael Bergoin, Clement Bout
 Created: 12/2018
**********************************************************************************************************************************************************/

#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/* This assumes you have the GStreamer headers on your include path
or in the same folder as this source file */
#include <gst/gst.h>


/**
 * @brief    	Initialize a TCP connection in order to request video stream
 *
 * @param	s			The socket
 * @param	serv_addr	Struct of the socket
 *
 */
void initializeTCPConnection(int *s, struct sockaddr_in *serv_addr){
	*s = socket(PF_INET, SOCK_STREAM, 0);

	serv_addr->sin_family = AF_INET;
	serv_addr->sin_addr.s_addr = inet_addr ("10.1.1.1");	// Address of the server
	serv_addr->sin_port = htons (5502);	// Port of the server
	memset (serv_addr->sin_zero, 0, sizeof(serv_addr->sin_zero));
	
	connect (*s, (struct sockaddr *)serv_addr, sizeof *serv_addr);
}


/**
 * @brief      Main
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
	initializeTCPConnection(&s, &serv_addr);

	
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