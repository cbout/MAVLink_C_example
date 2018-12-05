# The C MAVLink library

## Introduction

The c_library_v1 is an auto-generated library, for the general developer guide click [here][devguide]. You can find it in this [repository][c_lib]. If you want more information about our formation see [M1 IISC University of Cergy-Pontoise][m1iisc] and the laboratory [ETIS][etis]. At the start of our project it would have been useful to find a simple explanation of the C library. In this file we will describe the library as we understand it and we will trying to make your research into this one easiest.

## Architecture

The c_library_v1 is composed by various folders, the main folder is  **`common/`** which contains global MAVLink standard messages and structure, there is also **`minimal/`**, **`standard/`**,  **`icarous/`** and **`uAvionix/`** which do not implement all the common messages. Other folders are sort of "plug in" library that implement specific messages standard, like the folder **`ardupilotmega/`**, this one implements all the function of `common/`, `uAvionix/` and `icarous/`, it also adds standard **ardupilot** function, each specific autopilot has its own folder (**`ASLUAV/`**, **`slugs/`**, **`matrixpilot/`**, **`autoquad/`** and **`ardupilotpmega/`**). You can see all the base include of a **`<folder_name>/`** at the end of the file called : **`<folder_name>.h`**.

## How to include?

The include is the same for all autopilot, in your `.c` file you have just to insert in header this include : `#include <mavlink.h>`. The only change is in the compilation, in fact if you want to use just common messages, just add `-I <path>/c_library_v1/common` in your gcc command line. In the same way, if you want to use ardupilot messages just use `-I <path>/c_library_v1/ardupilotmega` instead of the last one. You can also use the relative path with `#include "<path>/mavlink.h"`.

## Main structures and functions

### Structures:

All those structures can be found in the root of the library in the file : `mavlink_types.h`. 

The main structure of this library is the `mavlink_message_t`:

```c
typedef struct __mavlink_message {
	uint16_t checksum; ///< sent at end of packet
	uint8_t magic;   ///< protocol magic marker
	uint8_t len;     ///< Length of payload
	uint8_t seq;     ///< Sequence of packet
	uint8_t sysid;   ///< ID of message sender system/aircraft
	uint8_t compid;  ///< ID of the message sender component
	uint8_t msgid;   ///< ID of message in payload
	uint64_t payload64[(MAVLINK_MAX_PAYLOAD_LEN+MAVLINK_NUM_CHECKSUM_BYTES+7)/8];
} mavlink_message_t;
```

It is used to store encoded messages (received or to send), this structure will be used in most of functions.

There is also the `mavlink_status_t` structure that can be used to know the current state of the communication, see the following code :

```c
typedef struct __mavlink_status {
    uint8_t msg_received;               ///< Number of received messages
    uint8_t buffer_overrun;             ///< Number of buffer overruns
    uint8_t parse_error;                ///< Number of parse errors
    mavlink_parse_state_t parse_state;  ///< Parsing state machine
    uint8_t packet_idx;                 ///< Index in current packet
    uint8_t current_rx_seq;             ///< Sequence number of last packet received
    uint8_t current_tx_seq;             ///< Sequence number of last packet sent
    uint16_t packet_rx_success_count;   ///< Received packets
    uint16_t packet_rx_drop_count;      ///< Number of packet drops
} mavlink_status_t;
```

### Functions:

Those functions can be found in the root of the library in the file : `mavlink_helpers.c`

To send a message with this library you have to create a `mavlink_message_t` with the good function (we will describe how later) then put your message in a buffer with this function :

```c
/**
 * @brief Pack a message to send it over a serial byte stream
 */
MAVLINK_HELPER uint16_t mavlink_msg_to_send_buffer(uint8_t *buffer, const mavlink_message_t *msg);
```

Once your buffer is ready you can send it with the protocol you have chosen (UDP or UART).

To read the receiving buffer you will need to use this function :

```c
/**
 * @param chan     ID of the current channel. This allows to parse different channels with this function.
 *                 a channel is not a physical message channel like a serial port, but a logic partition of
 *                 the communication streams in this case. COMM_NB is the limit for the number of channels
 *                 on MCU (e.g. ARM7), while COMM_NB_HIGH is the limit for the number of channels in Linux/Windows
 * @param c        The char to parse
 *
 * @param returnMsg NULL if no message could be decoded, the message data else
 * @param returnStats if a message was decoded, this is filled with the channel's stats
 * @return 0 if no message could be decoded or bad CRC, 1 on good message and CRC
*/
MAVLINK_HELPER uint8_t mavlink_parse_char(uint8_t chan, uint8_t c, mavlink_message_t* r_message, mavlink_status_t* r_mavlink_status);
```

It parses the buffer and return `1` when a message is totally parse, a common implementation is :

```c
// The buffer contain all the data stream received
mavlink_message_t msg;
mavlink_status_t status;
for (int i = 0; i < repSize; i++)
{
    if (mavlink_parse_char(MAVLINK_COMM_0, buf[i], &msg, &status))
    {
        printf("Received packet: SYS: %d, COMP: %d, PAYLOAD LEN: %d, MSG ID: %d\n", msg.sysid, msg.compid, msg.len, msg.msgid);
        // Do the treat by msgid (message id)
    }
}
```

With this function you are able to send a pack message and parse a message, but the payload is always a binary field and you do not know how to create this message. We will now start to study pattern and function that exist to manipulate messages, each sub-library work in the same way.

## Messages

In most of folders you will find many functions and files, the most important one is **`<folder_names>.h`**(e.g. **`common.h`** for **`common/`**), it contains all the **`defines`** (except messages' id) and **`enum`** that allows you to make a readable code. You will need to inspect many times this file to find the great parameters for a message, we will come back later on the utility of those `defines` and `enum`.

The **`mavlink_msg_<message_name>.h`** are the messages' files, there is a recurring pattern in each files, it contains method **to code, decode** and **manipulate** a MAVLink message. The global pattern is the following:

### In Header:

The header contains the **`#define MAVLINK_MSG_ID_<message_name> <ID>`** and the associated structure **`mavlink_<message_name>_t`**. You can also find an abstract of the messages' id [here][mavid].

### In the Body:

In the body you'll find 2 important functions:

* `mavlink_msg_<message_name>_decode()` to decode a message of type `mavlink_message_t` into a specific `mavlink_<message_name>__t` structure.
* `mavlink_msg_<message_name>_pack()` to pack a specific `mavlink_<message_name>__t` structure into a `mavlink_message_t `.

#### The decode

The decode function has always the same pattern of parameters :

```c
/**
 * @brief Decode a <message_name> into a struct mavlink_<message_name>_t
 *
 * @param msg The message to decode
 * @param decode_message C-struct to decode the message contents into
 */
static inline void mavlink_msg_<message_name>_decode(const mavlink_message_t* msg, mavlink_<message_name>_t* decode_message);

```

And then you have just to refer to `mavlink_<message_name>_t` structure to know fields and using it.

#### The Filling of the MAVLink message

The pack function creates a `mavlink_message_t` ready to be putting in the buffer, the first three parameters are always the same but the rest depends on the message type and its parameters. 

```c
/**
 * @brief Pack a <message_name> message
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_<message_name>_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,<Specific_parameters>);

```

To find values of those specifics parameters you can refer to the [dev guide][devguide_io] to find your message definition then you have to refer to `<folder_name>.h` to find the correct enum's name to use.

### Example

If we take the example of the command long message of `common.h`, in `mavlink_msg_command_long.h`.

In this file you'll see the command long message ID :

```c
#define MAVLINK_MSG_ID_COMMAND_LONG 76
```

Then there is the `mavlink_command_long_t` struct:

```c
typedef struct __mavlink_command_long_t {
 float param1; /*<  Parameter 1 (for the specific command).*/
 float param2; /*<  Parameter 2 (for the specific command).*/
 float param3; /*<  Parameter 3 (for the specific command).*/
 float param4; /*<  Parameter 4 (for the specific command).*/
 float param5; /*<  Parameter 5 (for the specific command).*/
 float param6; /*<  Parameter 6 (for the specific command).*/
 float param7; /*<  Parameter 7 (for the specific command).*/
 uint16_t command; /*<  Command ID (of command to send).*/
 uint8_t target_system; /*<  System which should execute the command*/
 uint8_t target_component; /*<  Component which should execute the command, 0 for all components*/
 uint8_t confirmation; /*<  0: First transmission of this command. 1-255: Confirmation transmissions (e.g. for kill command)*/
} mavlink_command_long_t;
```

And the pack function :

```c
/**
 * @brief Pack a command_long message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param target_system  System which should execute the command
 * @param target_component  Component which should execute the command, 0 for all components
 * @param command  Command ID (of command to send).
 * @param confirmation  0: First transmission of this command. 1-255: Confirmation transmissions (e.g. for kill command)
 * @param param1  Parameter 1 (for the specific command).
 * @param param2  Parameter 2 (for the specific command).
 * @param param3  Parameter 3 (for the specific command).
 * @param param4  Parameter 4 (for the specific command).
 * @param param5  Parameter 5 (for the specific command).
 * @param param6  Parameter 6 (for the specific command).
 * @param param7  Parameter 7 (for the specific command).
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_command_long_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,uint8_t target_system, uint8_t target_component, uint16_t command, uint8_t confirmation, float param1, float param2, float param3, float param4, float param5, float param6, float param7);
```

It's look a little bit mystic like that and it is if you don't know where to find information. But previously we give you this [link][devguide_io],  then you can find this table ([here][cmdlong]):

| Field Name       | Type     | Values            | Description                                                  |
| ---------------- | -------- | ----------------- | ------------------------------------------------------------ |
| target_system    | uint8_t  |                   | System which should execute the command                      |
| target_component | uint8_t  |                   | Component which should execute the command, 0 for all components |
| command          | uint16_t | [MAV_CMD][mavcmd] | Command ID (of command to send).                             |
| confirmation     | uint8_t  |                   | 0: First transmission of this command. 1-255: Confirmation transmissions (e.g. for kill command) |
| param1           | float    |                   | Parameter 1 (for the specific command).                      |
| param2           | float    |                   | Parameter 2 (for the specific command).                      |
| param3           | float    |                   | Parameter 3 (for the specific command).                      |
| param4           | float    |                   | Parameter 4 (for the specific command).                      |
| param5           | float    |                   | Parameter 5 (for the specific command).                      |
| param6           | float    |                   | Parameter 6 (for the specific command).                      |
| param7           | float    |                   | Parameter 7 (for the specific command).                      |

You may say "i have no more information", but as you can see there is a link in value column ([MAV_CMD][mavcmd]), this column will be your best friend while you are coding (except if you are a MAVLink expert). Once you know the name of the enum to put in parameters, you can go find the associated enum in `common.h` or you can follow the link and find the command you want to do.

If we want to use the command : 

| CMD ID |                                                              |                                                              |
| ------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| 80     | [MAV_CMD_NAV_ROI](https://mavlink.io/en/messages/common.html#MAV_CMD_NAV_ROI) | Sets  the region of interest (ROI) for a sensor set or the vehicle itself.  This can then be used by the vehicles control system to control the  vehicle attitude and the attitude of various sensors such as cameras. |
|        | Mission Param #1                                             | Region of interest mode. (see MAV_ROI enum)                  |
|        | Mission Param #2                                             | Waypoint index/ target ID. (see MAV_ROI enum)                |
|        | Mission Param #3                                             | ROI index (allows a vehicle to manage multiple ROI's)        |
|        | Mission Param #4                                             | Empty                                                        |
|        | Mission Param #5                                             | x the location of the fixed ROI (see MAV_FRAME)              |
|        | Mission Param #6                                             | y                                                            |
|        | Mission Param #7                                             | z                                                            |

Now you know you have to put the param `MAV_CMD_NAV_ROI` in the function and you have to refer to `MAV_ROI` enum for param 1 and 2 and `MAV_FRAME` for the param 5.

## Conclusion

Now you are ready to understand this library. If you want some concretes examples of implementation you can refer to our folder [examples/](../examples). We hope this file helps you to understand MAVLink_C Library. 

[devguide]:https://mavlink.io/en/	"MAVLink's libs general dev guide"
[c_lib]: https://github.com/mavlink/c_library_v1	"C library v1 repository"
[mavid]: https://groups.google.com/forum/#!topic/mavlink/1zgHUM67E-A	"Summary of messages' id"
[devguide_io]: https://mavlink.io/en/messages/	"Global MAVLink dev-guide"
[cmdlong]:https://mavlink.io/en/messages/common.html#COMMAND_LONG
[mavcmd]:https://mavlink.io/en/messages/common.html#MAV_CMD
[etis]: https://www-etis.ensea.fr/	"ETIS laboratory's website"
[m1iisc]: https://depinfo.u-cergy.fr/master/m1-iisc-master-1	"Master's website"