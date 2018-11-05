cd /mavlink_communication

gcc mavlink_initialization_UDP_test.c -o mavlink_initialization_UDP_test
./mavlink_initialization_UDP_test

gcc -std=c99 -I ../include/ardupilotmega -o mavlink_send_UDP_test mavlink_send_UDP_test.c
./mavlink_send_UDP_test

gcc -std=c99 -I ../include/ardupilotmega -o mavlink_receive_UDP_test mavlink_receive_UDP_test.c
./mavlink_receive_UDP_test


ctrl c to stop the program