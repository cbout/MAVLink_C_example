# MAVLink video stream examples

## 

## Running ##

```
gcc mavlink_video_stream_receiver_test.c -o mavlink_video_stream_receiver_test

./mavlink_video_stream_receiver_test
```

cd /mavlink_video_stream

gcc mavlink_video_stream_receiver_test.c -o mavlink_video_stream_receiver_test
./mavlink_video_stream_receiver_test

gcc mavlink_video_stream_gstreamer_test_v1.c -o mavlink_video_stream_gstreamer_test_v1 `pkg-config --cflags --libs gstreamer-1.0`
./mavlink_video_stream_gstreamer_test_v1

gcc mavlink_video_stream_gstreamer_test_v2.c -o mavlink_video_stream_gstreamer_test_v2 `pkg-config --cflags --libs gstreamer-1.0`
./mavlink_video_stream_gstreamer_test_v2

ctrl c to stop the program