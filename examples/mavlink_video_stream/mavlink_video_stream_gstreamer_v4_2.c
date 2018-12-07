/**********************************************************************************************************************************************************
 Copyright  ETIS — ENSEA, Université de Cergy-Pontoise, CNRS (1991-2018)
 promethe@ensea.fr


 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! THIS PROGRAM IS NOT TESTED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 Short program to Display video stream from the solo 3DR drone with GStreamer library, initialize the TCP request data stream before and register video in a file and frames in a memory buffer during the execution.
 This program is an extension of the program mavlink_video_stream_gstreamer_v3_test.c
 Firstly this program do exactly the same than the previous one, the following to register each frames of the stream in a memory buffer is a little bit different.
 Indeed, we add to the string to parse, the "appsink" and his parameters.
 Then we have to get the sink before playing the pipeline.
 After that we do the following instructions infinitely :
 1. Get the sample and the buffer
 -. Get the capture, the structure, and so the width and the height (one time)
 2. Get the map and the image matrix 
 3. Transform the image matrix in a pgm image (optional)

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
 gcc mavlink_video_stream_gstreamer_v4_2.c -o mavlink_video_stream_gstreamer_v4_2 -I/usr/local/include/gstreamer-1.0/ `pkg-config --cflags --libs gstreamer-1.0` `pkg-config --cflags --libs gstreamer-app-1.0 `
 
 Execution : 
 ./mavlink_video_stream_gstreamer_v4_2

**********************************************************************************************************************************************************/
/*********************************************************************************************************************************************************
 Authors: Raphael Bergoin, Clement Bout
 Created: 12/2018
**********************************************************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/* This assumes you have the GStreamer headers on your include path
or in the same folder as this source file */
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/app/gstappsrc.h>


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
    
	GstElement *pipeline, *sink;
	GstStateChangeReturn ret;

	GstSample *sample;
	GstCaps *caps;
	GstBuffer *buffer;
	GstStructure *structure;
	GstMapInfo map;
	unsigned char *gst_image;
	
	int width;		//Width of the video
	int height;		//Height of the video

	FILE * fp;
	int s, i;
	struct sockaddr_in serv_addr;
	
	
	/* Initialize a TCP connection in order to request video stream */
	initializeTCPConnection(&s, &serv_addr);


	/* Initialize GStreamer */
    gst_init(&argc, &argv);

	/* Build the pipeline */
    pipeline = gst_parse_launch("-v udpsrc port=5600 caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin  ! tee name=t ! queue ! videoconvert ! autovideosink t. ! queue ! avimux ! filesink location=capture.avi t. ! queue ! appsink name=sink emit-signals=true sync=false max-buffers=1 drop=true", NULL);
	
	/* Get sink */
   	sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");
	
	/* Start playing */
	ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) 
	{
      g_printerr ("Unable to set the pipeline to the playing state.\n");
      gst_object_unref (pipeline);
      return -1;
    }
	
	
	/* Get sample and buffer */
	sample = gst_app_sink_pull_sample ((GstAppSink *)sink);
    buffer = gst_sample_get_buffer(sample);
	
	/* Get capture, structure, width and height */
	caps = gst_sample_get_caps(sample);
    structure = gst_caps_get_structure(caps, 0);
    width = g_value_get_int(gst_structure_get_value(structure, "width"));
    height = g_value_get_int(gst_structure_get_value(structure, "height"));
	//printf("%d, %d\n", width, height);	//Width and height of the video
	
    /* Get map and initialize image matrix */
	gst_buffer_map(buffer, &map, GST_MAP_READ);
	gst_image = malloc(map.size * sizeof(unsigned char));
	//printf("%lu \n",map.size);				// Size of the video
	
	
	/* Main loop */
    while(1) {
		/* Iterate frames of the video stream */
        g_main_iteration(0);
		
		/* Get sample and buffer */
		sample = gst_app_sink_pull_sample ((GstAppSink *)sink);
	    buffer = gst_sample_get_buffer(sample);
		
		/* Get map and image matrix */
		gst_buffer_map(buffer, &map, GST_MAP_READ);
		memcpy(gst_image, (char*)map.data, map.size);
		
		
		/* INTERPRETATION OF gst_image with file */
		fp = fopen ("capture.pgm", "w+");
		fprintf(fp, "P2\n%d %d\n255\n", width, height);
		
		/* For each pixel of the image */
		for(i=0; i<width*height; i++)
		{
			fprintf(fp, "%d \n", gst_image[2*i+1]);	
		} 
		fclose(fp);
	}
	
	
    /* Free resources */
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
    return 0;
}