cd /mavlink_communication


gcc mavlink_initialization_UDP_test.c -o mavlink_initialization_UDP_test
./mavlink_initialization_UDP_test

gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega -o mavlink_send_UDP_test mavlink_send_UDP_test.c
./mavlink_send_UDP_test

gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega -o mavlink_receive_UDP_test mavlink_receive_UDP_test.c
./mavlink_receive_UDP_test

gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega -o mavlink_message_decode_test mavlink_message_decode_test.c
./mavlink_message_decode_test

gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega -o mavlink_send_order_test mavlink_send_order_test.c
./mavlink_send_order_test

gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega -o mavlink_send_take_off mavlink_send_take_off.c
./mavlink_send_take_off


ctrl c to stop the program