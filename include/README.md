

# The C MAVLink library

## Introduction

The c_library_v1 is an auto-generated library, for the general developer guide click [here][devguide]. You can find it in this [repository][c_lib]. At the start of our project it would have been useful to find a simple explanation of the C library. In this file we will describe the library as we understand it and we will trying to make your research into this one easiest.

## Architecture

The c_library_v1 is composed by various folders, the main folder is  **`common/`** which contains global MAVLink standard messages and structure, there is also **`minimal/`**, **`standard/`**,  **`icarous/`** and **`uAvionix/`** which do not implement all the common messages. Other folders are sort of "plug in" library that implement specific messages standard, like the folder **`ardupilotmega/`**, this one implements all the function of `common/`, `uAvionix/` and `icarous/`, it also adds standard **ardupilot** function, each specific autopilot has its own folder (**`ASLUAV/`**, **`slugs/`**, **`matrixpilot/`**, **`autoquad/`** and **`ardupilotpmega/`**). You can see all the base include of a `[folder name]/` at the end of the file called : `[folder name].h`.

## How to include?

The include is the same for all autopilot, in your `.c` file you have just to insert in header this include : `#include <mavlink.h>`. The only change is in the compilation, in fact if you want to use just common messages, just add `-I [path]/c_library_v1/common` in your gcc command line. In the same way, if you want to use ardupilot messages just use `-I [path]/c_library_v1/ardupilotmega` instead of the last one.

## Main structures and functions

### Structures:

All this structure can be found in the root of the library in the file : `mavlink_types.h`. 

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

It is used to store encoded messages (received or to send), this structure will be used in the most of function implement in the library.

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

This functions can be found in the root of the library in the file : `mavlink_udp.c`

To send message with this library you have to create a `mavlink_message_t` with the good function (i will describe those function later) then put your messages in a buffer with this function :

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

It parses the buffer and return `1` when a message is totaly parse, a common implementation is :

```c
// The buffer contain all the data stream received
mavlink_message_t msg;
mavlink_status_t status;
for (int i = 0; i < repSize; i++)
{
    if (mavlink_parse_char(MAVLINK_COMM_0, buf[i], &msg, &status))
    {
        printf("Received packet: SYS: %d, COMP: %d, PAYLOAD LEN: %d, MSG ID: %d\n", msg.sysid, msg.compid, msg.len, msg.msgid);
        // Do the treat by msgid
    }
}
```

With this function you are able to send a pack message and parse a receving message, but the payload is always a binary field and you do not know how to create this message, we will now start to studie pattern and function that exist to manipulate message, each sub-library work in the same way.

## Messages

In most of folders you will find many functions and files, the most important one is `[folder names].h`(e.g. `common.h` for `common/`), it contains all the `defines` (except messages' id) and `enum` that allows you to make a readable code. You will need to inspect many times this file to find the great parameters for a message, we will come back later on the utility of those `defines` and `enum`.

The `mavlink_msg_[message name].h` are the "messages" files, there is a recuring pattern in each file, it contains method to code, decode and manipulate a MAVLink message. The pattern is the following:

### In Headers:

The header contains the `#define MAVLINK_MSG_ID_[message name] <ID>` and the associate structure `mavlink_[message name]_t`, you can link the field `msgid` of the `mavlink_message_t` with the define and use the associate structure to store a message of type `[message name]`.

### In the Body

In the body you'll find 2 important function:

* `mavlink_msg_[message name]_decode()`
* `mavlink_msg_[message name]_pack()`

#### The decode

The decode function has always the same parameters :

```c
/**
 * @brief Decode a [message name] into a struct mavlink_[message name]_t
 *
 * @param msg The message to decode
 * @param decode_message C-struct to decode the message contents into
 */
static inline void mavlink_msg_[message name]_decode(const mavlink_message_t* msg, mavlink_[message name]_t* decode_message);

```

#### The Filling of the MAVLink message

The pack function creates a `mavlink_message_t` ready to be putting in the buffer, the first three parameters are always the same but the rest depends on the message type and his parameters. 

```c
/**
 * @brief Pack a [message name] message
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_[message name]_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,[Specific parameters]);

```



[devguide]:https://mavlink.io/en/	"MAVLink's libs general dev guide"
[c_lib]: https://github.com/mavlink/c_library_v1	"C library v1 repository"