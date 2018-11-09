cd /mavlink_video_stream

gcc mavlink_video_stream_receiver_test_v1.c -o mavlink_video_stream_receiver_test_v1
./mavlink_video_stream_receiver_test_v1

gcc mavlink_video_stream_gstreamer_test.c -o mavlink_video_stream_gstreamer_test `pkg-config --cflags --libs gstreamer-1.0`
./mavlink_video_stream_gstreamer_test

ctrl c to stop the program