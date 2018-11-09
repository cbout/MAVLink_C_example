# MAVLink communication example

## Introduction

If you want more information about our formation see [M1 IISC University of Cergy-Pontoise][m1iisc] and the laboratory [ETIS][etis]. We will do a little abstract of each file, for a detailled explain please see [Documentation pdf](./Documentation_code_v2.pdf).

## Mavlink_initialization_UDP

To compile and execute this file use this commande in this folder.

```bash
gcc -std=c99 mavlink_initialization_UDP.c -o mavlink_initialization_UDP
./mavlink_initialization_UDP
```

This file contains an example of UDP connection to a 3DR Solo Drone, to connect an other drone may you will have to use other port and IP adress.

## Mavlink_send_UDP

To compile and execute this file use this commande in this folder.

```bash
gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega mavlink_send_UDP.c -o mavlink_send_UDP
./mavlink_send_UDP
```



## Mavlink_receive_UDP

To compile and execute this file use this commande in this folder.

```bash
gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega  mavlink_receive_UDP.c -o mavlink_receive_UDP
./mavlink_receive_UDP
```



## Mavlink_message_decode

To compile and execute this file use this commande in this folder.

```bash
gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega  mavlink_message_decode.c -o mavlink_message_decode
./mavlink_message_decode
```



## Mavlink_send_order_UDP

To compile and execute this file use this commande in this folder.

```bash
gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega mavlink_send_order.c -o mavlink_send_order
./mavlink_send_order
```











[devguide]: https://mavlink.io/en/	"MAVLink's libs general dev guide"
[c_lib]: https://github.com/mavlink/c_library_v1	"C library v1 repository"
[etis]: https://www-etis.ensea.fr/	"ETIS laboratory's website"
[m1iisc]: https://depinfo.u-cergy.fr/master/m1-iisc-master-1	"Master's website"