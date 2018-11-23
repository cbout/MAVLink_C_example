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
 * @brief      Short program to see how to send an order of take off in mavlink to the drone
 *			   This program is an extension of the program mavlink_send_order.c and mavlink_message_decode.c
 *
 * @return     0
 */
int main(void)
{
	
	int sock;
	struct sockaddr_in targetAddr;
	struct sockaddr_in locAddr;
	uint8_t buf[BUFFER_LENGTH];
	ssize_t recsize;
	socklen_t fromlen;
	mavlink_channel_t chan = MAVLINK_COMM_0;
	int bytes_sent;
	mavlink_gps_raw_int_t gps_raw_int;
	mavlink_message_t msg;
	uint16_t len;
	
	char target_ip[100];
	strcpy(target_ip, "10.1.1.1");	//IP adress of the controler
	int local_port = 14550;		 //Listening port
	int timeout = 10;			//Time in second for waiting an answer from the server

	
	/* Init the socket to receive datagram and support UDP protocol */
	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	locAddr.sin_family = AF_INET;
	locAddr.sin_addr.s_addr = INADDR_ANY;
	locAddr.sin_port = htons(local_port);
	memset (&locAddr.sin_zero, 0, sizeof(locAddr.sin_zero));
	
	if (-1 == bind(sock,(struct sockaddr *)&locAddr, sizeof(struct sockaddr))){
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


	/* The possible socket which will received our messages */
	struct sockaddr_in possibleTarget;
  	socklen_t possibleTargetLen = sizeof(possibleTarget);

  	time_t currentTime;
  	time_t startTime = time(&currentTime);
  	double timeLeft = 0;

  	/* While we don't find a packet which can indicate sending port we read all messages received on our port */
	while (recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)(&possibleTarget), &possibleTargetLen)<=0 || possibleTarget.sin_addr.s_addr != inet_addr(target_ip)) {
		
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
	
	
	/* We need informations about GPS on the next step 
	Receive packets while we don't receive GPS_RAW_INT message */
	while (msg.msgid != MAVLINK_MSG_ID_GPS_RAW_INT){	
		memset(buf, 0, BUFFER_LENGTH);
		recsize = recvfrom(sock, (void *)buf, BUFFER_LENGTH, 0, (struct sockaddr *)&targetAddr, &fromlen);	// reception
		if (recsize > 0)
		{
			/* Something received */
			mavlink_message_t msg;
			mavlink_status_t status;
			int i;
			unsigned int temp = 0;
			
			/* For each part of the tram */
			for (i = 0; i < recsize; ++i)
			{	
				/* Parse the tram in order to get a mavlink message */
				if (mavlink_parse_char(chan, buf[i], &msg, &status))
				{
					/* Decoding message */
					if(msg.msgid == MAVLINK_MSG_ID_GPS_RAW_INT) {	//If the message is of type GPS_RAW_INT
						/* Decode informations of the message and put it in the variable */
						mavlink_msg_gps_raw_int_decode(&msg, &gps_raw_int);
					}				
				}
			}
		}
	}
	
	

	/* Packing the type of message you want in msg variable*/
	mavlink_msg_mission_count_pack(255, 0, &msg, 1, 190, 2);	//MISSION COUNT
	
	/* Put it in the buffer */
	len = mavlink_msg_to_send_buffer(buf, &msg);
	/* Send it */
	bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*)&targetAddr, sizeof(struct sockaddr_in));
	if (bytes_sent==-1) {
		perror("Error\n"); // If there is an error
		exit(EXIT_FAILURE);
	}
	else{
		printf("Mission count sent\n");		//If it's done
	}
	memset(buf, 0, BUFFER_LENGTH);
	
	
	/* Packing the type of message you want in msg variable*/
	mavlink_msg_mission_item_pack(255, 0, &msg, 1, 190, 0, MAV_FRAME_GLOBAL, MAV_CMD_NAV_WAYPOINT, 1, 1, 0, 0, 0, 0, gps_raw_int.lat, gps_raw_int.lon, gps_raw_int.alt);	//MISSION ITEM WAYPOINT
	
	/* Put it in the buffer */
	len = mavlink_msg_to_send_buffer(buf, &msg);
	/* Send it */
	bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*)&targetAddr, sizeof(struct sockaddr_in));
	if (bytes_sent==-1) {
		perror("Error mission item waypoint\n"); // If there is an error
		exit(EXIT_FAILURE);
	}
	else{
		printf("Mission item waypoint sent\n");		//If it's done
	}
	memset(buf, 0, BUFFER_LENGTH);
	
	
	/* Packing the type of message you want in msg variable*/
	mavlink_msg_mission_item_pack(255, 0, &msg, 1, 190, 1, MAV_FRAME_GLOBAL_RELATIVE_ALT, MAV_CMD_NAV_TAKEOFF, 0, 1, 15, 0, 0, 0, 0, 0, 1);	//MISSION ITEM TAKEOFF 1m
	
	/* Put it in the buffer */
	len = mavlink_msg_to_send_buffer(buf, &msg);
	/* Send it */
	bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*)&targetAddr, sizeof(struct sockaddr_in));
	if (bytes_sent==-1) {
		perror("Error mission item takeoff\n"); // If there is an error
		exit(EXIT_FAILURE);
	}
	else{
		printf("Mission item takeoff sent\n");		//If it's done
	}
	memset(buf, 0, BUFFER_LENGTH);
	
	
	/* Packing the type of message you want in msg variable*/
	mavlink_msg_set_mode_pack(255, 0, &msg, 1, MAV_MODE_GUIDED_DISARMED+MAV_MODE_FLAG_CUSTOM_MODE_ENABLED, COPTER_MODE_GUIDED); // Request change flight mode
	/* Put it in the buffer */
	len = mavlink_msg_to_send_buffer(buf, &msg);
	/* Send it */
	bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*)&targetAddr, sizeof(struct sockaddr_in));
	if (bytes_sent==-1) {
		perror("Error changing mode\n"); // If there is an error
		exit(EXIT_FAILURE);
	}
	else{
		printf("Changing mode sent\n");		//If it's done
	}
	memset(buf, 0, BUFFER_LENGTH);
	
	
	/* Packing the type of message you want in msg variable*/
	mavlink_msg_command_long_pack(255, 0, &msg, 1, 1, MAV_CMD_COMPONENT_ARM_DISARM, 0, 1, 0, 0, 0, 0, 0, 0);	//Request arm motors
	/* Put it in the buffer */
	len = mavlink_msg_to_send_buffer(buf, &msg);
	/* Send it */
	bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*)&targetAddr, sizeof(struct sockaddr_in));
	if (bytes_sent==-1) {
		perror("Error arming motors\n"); // If there is an error
		exit(EXIT_FAILURE);
	}
	else{
		printf("Arming motors\n");		//If it's done
	}
	memset(buf, 0, BUFFER_LENGTH);
	
	
	/* Packing the type of message you want in msg variable*/
	mavlink_msg_command_long_pack(255, 0, &msg, 1, 1, MAV_CMD_MISSION_START, 0, 0, 0, 0, 0, 0, 0, 0);	//Request start mission
	/* Put it in the buffer */
	len = mavlink_msg_to_send_buffer(buf, &msg);
	/* Send it */
	bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*)&targetAddr, sizeof(struct sockaddr_in));
	if (bytes_sent==-1) {
		perror("Error starting mission\n"); // If there is an error
		exit(EXIT_FAILURE);
	}
	else{
		printf("Mission start\n");		//If it's done
	}
	memset(buf, 0, BUFFER_LENGTH);

	
	
	/* End */
	close(sock);	
	return 0;
}