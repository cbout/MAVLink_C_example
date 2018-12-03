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
 * @brief      Short program to see how to send mavlink order to the drone
 *			   This program is an extension of the program mavlink_send_UDP_test.c
 *
 * @return     0
 */
int main(void)
{
	
	int sock;
	struct sockaddr_in targetAddr;				// The socket of the target address
	struct sockaddr_in locAddr;					// The socket of the local address
	struct sockaddr_in possibleTarget;			// The possible socket which will received our messages
  	socklen_t possibleTargetLen = sizeof(possibleTarget);
	uint16_t len;
	uint8_t buf[BUFFER_LENGTH];
	int bytes_sent;
	
	mavlink_message_t msg;						// Variable of type mavlink message 
	
	char target_ip[100];
	strcpy(target_ip, "10.1.1.1");				//IP adress of the controler
	int local_port = 14550;		 				//Listening port
	
	int timeout = 10;							//Time in second for waiting an answer from the server
  	time_t currentTime;
  	time_t startTime = time(&currentTime);
  	double timeLeft = 0;

	
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
	printf("INIT listenning : UDPin : 0.0.0.0:%d\n", ntohs(locAddr.sin_port));
	
	
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


  	/* While we don't find a packet which can indicate sending port we read all messages received on our port */
	while (recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)(&possibleTarget), &possibleTargetLen)<=0 || possibleTarget.sin_addr.s_addr != inet_addr(target_ip)) 
	{
		
		memset(buf,0,256);
		time(&currentTime);
		timeLeft = difftime(currentTime, startTime);
		if (timeLeft > timeout)
		{
			perror("Connection time out");
			close(sock);
			return -1;	
		}
	}

	/* Init the socket to send datagram and support UDP protocol */
	targetAddr.sin_family = AF_INET;
	targetAddr.sin_addr.s_addr = inet_addr(target_ip);
	targetAddr.sin_port = possibleTarget.sin_port;
	memset (&targetAddr.sin_zero, 0, sizeof(targetAddr.sin_zero));

	/* Initialization sending done */
	printf("INIT target : UDPout : %s:%d\n",target_ip,ntohs(targetAddr.sin_port));
	

	/* Packing the type of message you want in msg variable*/
	mavlink_msg_set_mode_pack(255, 0, &msg, 1, MAV_MODE_MANUAL_ARMED+MAV_MODE_FLAG_CUSTOM_MODE_ENABLED, COPTER_MODE_ALT_HOLD); // Request change flight mode
	/* Put it in the buffer */
	len = mavlink_msg_to_send_buffer(buf, &msg);
	/* Send it */
	bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*)&targetAddr, sizeof(struct sockaddr_in));
	if (bytes_sent==-1) 
	{
		perror("Error changing mode\n"); // If there is an error
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Changing mode sent\n");		//If it's done
	}
	memset(buf, 0, BUFFER_LENGTH);
	
	
	/* Packing the type of message you want in msg variable*/
	mavlink_msg_command_long_pack(255, 0, &msg, 1, 0, MAV_CMD_COMPONENT_ARM_DISARM, 0, 1, 0, 0, 0, 0, 0, 0);	//Request arm motors
	/* Put it in the buffer */
	len = mavlink_msg_to_send_buffer(buf, &msg);
	/* Send it */
	bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*)&targetAddr, sizeof(struct sockaddr_in));
	if (bytes_sent==-1) 
	{
		perror("Error arming motors\n"); // If there is an error
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Arming motors\n");		//If it's done
	}
	memset(buf, 0, BUFFER_LENGTH);
	
	
	sleep(4);
	
	
	/* Packing the type of message you want in msg variable*/
	mavlink_msg_command_long_pack(255, 0, &msg, 1, 0, MAV_CMD_COMPONENT_ARM_DISARM, 0, 0, 0, 0, 0, 0, 0, 0);	//Request disarm motors
	/* Put it in the buffer */
	len = mavlink_msg_to_send_buffer(buf, &msg);
	/* Send it */
	bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*)&targetAddr, sizeof(struct sockaddr_in));
	if (bytes_sent==-1) 
	{
		perror("Error disarming motors\n"); // If there is an error
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Disarming motors\n");		//If it's done
	}
	memset(buf, 0, BUFFER_LENGTH);
	
	
	/* End */
	close(sock);	
	return 0;
}