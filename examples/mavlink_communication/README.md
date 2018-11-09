# MAVLink communication example

## Introduction

If you want more information about our formation see [M1 IISC University of Cergy-Pontoise][m1iisc] and the laboratory [ETIS][etis]. We will do a little abstract of each file, for a detailled explain please see [Documentation pdf](./Documentation_code_v2.pdf).

## Mavlink_initialization_UDP

To compile this file use this commande in this folder.

```bash
gcc -std=c99 mavlink_initialization_UDP_test.c -o mavlink_initialization_UDP_test
./mavlink_initialization_UDP_test
```

This file contains an example of UDP connection to a 3DR Solo Drone.

## Mavlink_send_UDP

```bash
gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega mavlink_send_UDP_test.c -o mavlink_send_UDP_test
./mavlink_send_UDP_test
```



## Mavlink_receive_UDP

```bash
gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega  mavlink_receive_UDP_test.c -o mavlink_receive_UDP_test
./mavlink_receive_UDP_test
```



## Mavlink_message_decode

```bash
gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega  mavlink_message_decode_test.c -o mavlink_message_decode_test
./mavlink_message_decode_test
```



## Mavlink_send_order_UDP

```bash
gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega mavlink_send_order_test.c -o mavlink_send_order_test
./mavlink_send_order_test
```











[devguide]: https://mavlink.io/en/	"MAVLink's libs general dev guide"
[c_lib]: https://github.com/mavlink/c_library_v1	"C library v1 repository"
[etis]: https://www-etis.ensea.fr/	"ETIS laboratory's website"
[m1iisc]: https://depinfo.u-cergy.fr/master/m1-iisc-master-1	"Master's website"