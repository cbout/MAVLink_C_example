# MAVLink_C_example #

This project is released by BERGOIN Raphäel and BOUT Clément, students in the University of Cergy-Pontoise. It contains some examples of our understanding of the MAVLink C library (refer to this [github project ](https://github.com/mavlink/c_library_v1) to see the entire MAVLink c library project).

## Introduction ##

As part of our studies, in the [Master 1 Computer sciences and engineering of complex systems (IISC)](https://depinfo.u-cergy.fr/master/m1-iisc-master-1), we work with the [ETIS](https://www-etis.ensea.fr/) laboratory to release a stable communication between a 3DR Solo and C program which can control it.

We have decided to use the [c_library_v1](https://github.com/mavlink/c_library_v1) (MIT-licenced project) and we do not find many examples of implementation. That is why we are going to summarize all the information we have found. We will also expose our understanding of the MAVLink v1 communication and the C library across some examples in the folder examples.

## MAVLink ##

> Micro Air Vehicle Link (MAVLink) is a very lightweight messaging protocol for communicating with drones (and between onboard drone components). 

MAVLink can be used in UART and UDP connection. You can refer to this [article](https://discuss.ardupilot.org/t/mavlink-step-by-step/9629) written by **Pedro Albuquerque** which explain MAVLink protocol step by step. But we will do an abstract of all the informations we have compiled to realize our project. We start with the common structure of packets.

### Packet structure

For performance reasons packet's informations are binary, the common frame's size is in range [8-263] bytes.

[![Packet structure](https://discuss.ardupilot.org/uploads/default/original/2X/c/c836b6311cc167360e4d24f0e492c06fa8885442.png)](https://discuss.ardupilot.org/t/mavlink-step-by-step/9629)

| Byte index     | Content           | Value                  | Explanation                                                  |
| -------------- | ----------------- | ---------------------- | ------------------------------------------------------------ |
| 0              | Packet start sign | for MAVLink v1.0: 0xFE | Indicates the start of a new packet and the MAVLink packet version. |
| 1              | Payload length    | 0x00 to 0xFF           | Indicates the length of the packet's payload.                |
| 2              | Packet sequence   | 0x00 to 0xFF           | Because of UDP connection, we need to count packets to be sure no packet were lost. |
| 3              | System ID         | 0x00 to 0xFF           | ID of the Sending system (255 for ground control and 1 for vehicle). |
| 4              | Component ID      | 0x00 to 0xFF           | ID of the sending component.                                 |
| 5              | Message ID        | 0x00 to 0xFF           | ID of the message, the ID defines the size of the payload and its content. |
| 6 to (n+6)     | Data              | (0 - 255) bytes        | Message's data, it depends on the message ID.                |
| (n+7) to (n+8) | Checksum          | 2 bytes                | ITU X.25/SAE AS-4 hash from byte 1 to n+6                    |

