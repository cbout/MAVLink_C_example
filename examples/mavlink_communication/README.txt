Order :
1) mavlink_initialization_UDP
2) mavlink_send_UDP
3) mavlink_receive_UDP
4) mavlink_message_decode
5) mavlink_send_order
6) mavlink_send_take_off
7) mavlink_send_take_off_translation



cd /mavlink_communication


gcc mavlink_initialization_UDP.c -o mavlink_initialization_UDP
./mavlink_initialization_UDP

gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega -o mavlink_send_UDP mavlink_send_UDP.c
./mavlink_send_UDP

gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega -o mavlink_receive_UDP mavlink_receive_UDP.c
./mavlink_receive_UDP

gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega -o mavlink_message_decode mavlink_message_decode.c
./mavlink_message_decode

gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega -o mavlink_send_order mavlink_send_order.c
./mavlink_send_order

gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega -o mavlink_send_take_off mavlink_send_take_off.c
./mavlink_send_take_off

gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega -o mavlink_send_take_off_translation mavlink_send_take_off_translation.c
./mavlink_send_take_off_translation


ctrl c to stop the program