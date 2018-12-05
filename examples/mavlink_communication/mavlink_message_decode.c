#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <arpa/inet.h>
#include <string.h>

/* This assumes you have the mavlink headers on your include path
or in the same folder as this source file */
#include <mavlink.h>


#define BUFFER_LENGTH 2041


/**
 * @brief      Short program to see how to decode a mavlink message after receiving it
 *			   This program is an extension of the program mavlink_receive_UDP_test.c
 *
 * @return     0
 */
int main(void)
{
	
	int sock;
	struct sockaddr_in locAddr;				// The socket of the local address
	struct sockaddr_in targetAddr;			// The socket of the target address
	uint8_t buf[BUFFER_LENGTH];
	ssize_t recsize;
	socklen_t fromlen;
	int local_port = 14550;		 // Listening port
	
	int i;
	unsigned int temp = 0;
	
	mavlink_channel_t chan = MAVLINK_COMM_0;	// Variable of type mavlink channel 
	mavlink_message_t msg;						// Variable of type mavlink message 
	mavlink_status_t status;					// Variable of type mavlink status 
	mavlink_sys_status_t sys_status;			// Variable of type mavlink sys_status 
	
	
	/* Init the socket to receive datagram and support UDP protocol */
	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	locAddr.sin_family = AF_INET;
	locAddr.sin_addr.s_addr = INADDR_ANY;
	locAddr.sin_port = htons(local_port);
	memset (&locAddr.sin_zero, 0, sizeof(locAddr.sin_zero));
	
	if (-1 == bind(sock,(struct sockaddr *)&locAddr, sizeof(struct sockaddr)))
	{
		perror("error bind failed");
		close(sock);
    	return -1;
	}
	
	/* Initialization listenning done */
	printf("INIT listenning :\nUDPin: 0.0.0.0:%d\n", ntohs(locAddr.sin_port));
	
	
	/* Attempt to make it non blocking */
	#if (defined __QNX__) | (defined __QNXNTO__)
	if (fcntl(sock, F_SETFL, O_NONBLOCK | FASYNC) < 0)
	#else
	if (fcntl(sock, F_SETFL, O_NONBLOCK | O_ASYNC) < 0)
	#endif
	{
		fprintf(stderr, "error setting nonblocking: %s\n", strerror(errno));
		close(sock);
    	return -1;
	}


	/* Receive packets infinitely */
	while (1){
		
		memset(buf, 0, BUFFER_LENGTH);
		recsize = recvfrom(sock, (void *)buf, BUFFER_LENGTH, 0, (struct sockaddr *)&targetAddr, &fromlen);	// reception
		/* If something received */
		if (recsize > 0)
		{
			
			printf("Bytes Received : %d\n", (int)recsize);	//Size
			/* For each part of the tram */
			for (i = 0; i < recsize; ++i)
			{
				temp = buf[i];
				printf("%02x ", (unsigned char)temp); //Field of the tram in hexadecimal
				
				/* Parse the tram in order to get a mavlink message */
				if (mavlink_parse_char(chan, buf[i], &msg, &status))
				{
					/* Information about the packet received */
					printf("\nReceived packet: SYS: %d, COMP: %d, LEN: %d, MSG ID: %d\n", msg.sysid, msg.compid, msg.len, msg.msgid);
					
					
					/* Decoding message */
					if(msg.msgid == MAVLINK_MSG_ID_SYS_STATUS) //If the message is of type SYS_STATUS
					{	
						/* Decode informations of the message and put it in the variable */
						mavlink_msg_sys_status_decode(&msg, &sys_status);
						/* Display informations */
						printf("SYS_STATUS :\nOnboard_control_sensors_present : %d, Onboard_control_sensors_enabled : %d, Onboard_control_sensors_health : %d, Load : %d, Voltage_battery : %d, Current_battery : %d, Drop_rate_comm : %d, Errors_comm : %d, Errors_count1 : %d, Errors_count2 : %d, Errors_count3 : %d, Errors_count4 : %d, Battery_remaining : %d\n\n", sys_status.onboard_control_sensors_present, sys_status.onboard_control_sensors_enabled, sys_status.onboard_control_sensors_health, sys_status.load, sys_status.voltage_battery, sys_status.current_battery, sys_status.drop_rate_comm, sys_status.errors_comm, sys_status.errors_count1, sys_status.errors_count2, sys_status.errors_count3, sys_status.errors_count4, sys_status.battery_remaining);
					}
					else 
					{
						printf("This an other type of message\n\n");	//If other type of message
					}
					
					
				}
			}
			printf("\n");
		}
		printf("\n");

		sleep(1); // Sleep one second
	}

	
	/* End */
	close(sock);	
	return 0;
}