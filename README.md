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

| Byte index     | Content           | Value                                    | Explanation                                                  |
| -------------- | ----------------- | ---------------------------------------- | ------------------------------------------------------------ |
| 0              | Packet start sign | for MAVLink v1.x: 0xFE and 0xFD for v2.x | Indicates the start of a new packet and the MAVLink packet version. |
| 1              | Payload length    | 0x00 to 0xFF                             | Indicates the length of the packet's payload.                |
| 2              | Packet sequence   | 0x00 to 0xFF                             | Because of UDP connection, we need to count packets to be sure no packet were lost. |
| 3              | System ID         | 0x00 to 0xFF                             | ID of the Sending system (255 for ground control and 1 for vehicle). |
| 4              | Component ID      | 0x00 to 0xFF                             | ID of the sending component.                                 |
| 5              | Message ID        | 0x00 to 0xFF                             | ID of the message, the ID defines the size of the payload and its content. |
| 6 to (n+6)     | Data              | (0 - 255) bytes                          | Message's data, it depends on the message ID.                |
| (n+7) to (n+8) | Checksum          | 2 bytes                                  | ITU X.25/SAE AS-4 hash from byte 1 to n+6                    |

### Messages

The MAVLink message is a stream of bytes that has been encoded by a Mission Planner (MP) or a Ground Station (GS). Each message has a **message ID** ([all ids](https://groups.google.com/forum/#!topic/mavlink/1zgHUM67E-A)) and a **payload**, payload's content depends of the message ID. There is **common messages** (see [common.xml](https://mavlink.io/en/messages/common.html)) and **specific messages** for each autopilot (e.g. [ardupilot](https://mavlink.io/en/messages/ardupilotmega.html) or [autoquad](https://mavlink.io/en/messages/autoquad.html)). All common messages are not implemented for each autopilot but they are complementary. We will describe the heartbeat message:

#### Heartbeat (ID: #0)

The heartbeat is one of the most important messages, GCS have to send it each second to notify he is alive and the connection is available.  

| Field Name      | Type     | Values                                                       | Description                |
| --------------- | -------- | ------------------------------------------------------------ | -------------------------- |
| type            | uint8_t  | [MAV_TYPE](https://mavlink.io/en/messages/common.html#MAV_TYPE) | The type of the MAV        |
| autopilot       | uint8_t  | [MAV_AUTOPILOT](https://mavlink.io/en/messages/common.html#MAV_AUTOPILOT) | The autopilot class        |
| base_mode       | uint8_t  | [MAV_MODE_FLAG](https://mavlink.io/en/messages/common.html#MAV_MODE_FLAG) | System mode bitmap         |
| custom_mode     | uint32_t |                                                              | Specific autopilot's flags |
| system_status   | uint8_t  | [MAV_STATE](https://mavlink.io/en/messages/common.html#MAV_STATE) | System status              |
| mavlink_version | uint8_t  |                                                              | MAVLink version            |

For example, for a heartbeat the message will looking to this :

|              MAVLink version              |       Payload length       |               Packet sequence               |             Systeme ID             |               Component ID                |           Message ID            |            Data            | Checksum |
| :---------------------------------------: | :------------------------: | :-----------------------------------------: | :--------------------------------: | :---------------------------------------: | :-----------------------------: | :------------------------: | -------- |
| FE<br />Start of a packet of MAVLink v1.0 | 09<br />Len of the payload | 00<br />Mean it's the first message we sent | FF<br />Mean we are Ground Control | 00<br />For GC no need to specify Comp ID | 00<br />Refer to Heartbeat's ID | 00 00 00 00 06 03 C0 04 03 | F5 C0    |

Data :

* type: MAV_TYPE_GCS (0x06)
* autopilot: MAV_AUTOPILOT_ARDU_PILOTMEGA(0x03)
* base mode: MAV_MODE_FLAG_SAFETY_ARMED + MAV_MODE_FLAG_MANUAL_INPUT_ENABLED(0xc0)
* custom mode: No custom mode(0x00 00 00 00)
* systeme status: MAV_STATE_ACTIVE(0x04)
* mavlink_version: 0x03





(for commands messages refer to : [MAVLink_commande_message](http://ardupilot.org/dev/docs/copter-commands-in-guided-mode.html))