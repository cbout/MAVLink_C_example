Order :
1) mavlink_video_stream_receiver
2) mavlink_video_stream_gstreamer_v1
3) mavlink_video_stream_gstreamer_v2
4) mavlink_video_stream_gstreamer_v3
5) mavlink_video_stream_gstreamer_v4_1
6) mavlink_video_stream_gstreamer_v4_2



cd /mavlink_video_stream


gcc mavlink_video_stream_receiver.c -o mavlink_video_stream_receiver
./mavlink_video_stream_receiver

gcc mavlink_video_stream_gstreamer_v1.c -o mavlink_video_stream_gstreamer_v1 `pkg-config --cflags --libs gstreamer-1.0`
./mavlink_video_stream_gstreamer_v1

gcc mavlink_video_stream_gstreamer_v2.c -o mavlink_video_stream_gstreamer_v2 `pkg-config --cflags --libs gstreamer-1.0`
./mavlink_video_stream_gstreamer_v2

gcc mavlink_video_stream_gstreamer_v3.c -o mavlink_video_stream_gstreamer_v3 `pkg-config --cflags --libs gstreamer-1.0`
./mavlink_video_stream_gstreamer_v3

gcc mavlink_video_stream_gstreamer_v4_1.c -o mavlink_video_stream_gstreamer_v4_1 `pkg-config --cflags --libs gstreamer-1.0`
./mavlink_video_stream_gstreamer_v4_1

gcc mavlink_video_stream_gstreamer_v4_2.c -o mavlink_video_stream_gstreamer_v4_2 `pkg-config --cflags --libs gstreamer-1.0`
./mavlink_video_stream_gstreamer_v4_2


ctrl c to stop the program