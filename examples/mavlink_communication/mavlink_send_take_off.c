/**********************************************************************************************************************************************************
 Copyright  ETIS — ENSEA, Université de Cergy-Pontoise, CNRS (1991-2018)
 promethe@ensea.fr


 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! THIS PROGRAM IS NOT TESTED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 Short program to see how to send an order of take off in mavlink to the drone.
 This program is an extension of the program mavlink_send_order.c and mavlink_message_decode.c.
 After performing actions of the previous programs to initialize socket, seing how to send mavlink order in UDP and seing how to receive and decode a mavlink message.
 First of all, we have to get informations about GPS.
 So in the same way than before, while we don't receive a "GPS_RAW_INT" message, we listen incoming messages and decode it when we receive the right one.
 Now we can write the mission.
 First step, we send a "MISSION COUNT" message in the same way that we have seen previously.
 Then a "MISSION ITEM" message of type "WAYPOINT".
 Next a "MISSION ITEM" message of type "TAKEOFF".
 After that we have to request to change flight mode and to request to arm motors, doing the same process than in the mavlink_send_order file.
 Finnaly, we request to start mission with a "command_long" message.

 This software is governed by the CeCILL v2.1 license under French law and abiding by the rules of distribution of free software.
 You can use, modify and/ or redistribute the software under the terms of the CeCILL v2.1 license as circulated by CEA, CNRS and INRIA at the following URL "http://www.cecill.info".
 As a counterpart to the access to the source code and  rights to copy, modify and redistribute granted by the license,
 users are provided only with a limited warranty and the software's author, the holder of the economic rights,  and the successive licensors have only limited liability.
 In this respect, the user's attention is drawn to the risks associated with loading, using, modifying and/or developing or reproducing the software by the user in light of its specific status of free software,
 that may mean  that it is complicated to manipulate, and that also therefore means that it is reserved for developers and experienced professionals having in-depth computer knowledge.
 Users are therefore encouraged to load and test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data to be ensured
 and, more generally, to use and operate it in the same conditions as regards security.
 The fact that you are presently reading this means that you have had knowledge of the CeCILL v2.1 license and that you accept its terms.
 

 See more details about this program in files : documentation_communication, mavlink-devguide and mavlink_protocol.
 
 
 Compilation : 
 cd /mavlink_communication
 gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega mavlink_send_take_off.c -o mavlink_send_take_off
 
 Execution : 
 ./mavlink_send_take_off

**********************************************************************************************************************************************************/
/*********************************************************************************************************************************************************
 Authors: Raphael Bergoin, Clement Bout
 Created: 11/2018
**********************************************************************************************************************************************************/

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
 * @brief      	Send the order to start the mission
 *
 * @param		sock		the socket
 * @param		targetAddr	the structure of the target socket
 *			   
 */
void startMission(int *sock, struct sockaddr_in *targetAddr){
	
	int bytes_sent;
	uint16_t len;
	mavlink_message_t msg;						// Variable of type mavlink message
	uint8_t buf[BUFFER_LENGTH];
	
	
	/* Packing the type of message you want in msg variable*/
	mavlink_msg_command_long_pack(255, 0, &msg, 1, 1, MAV_CMD_MISSION_START, 0, 0, 0, 0, 0, 0, 0, 0);	//Request start mission
	
	/* Put it in the buffer */
	len = mavlink_msg_to_send_buffer(buf, &msg);
	/* Send it */
	bytes_sent = sendto(*sock, buf, len, 0, (struct sockaddr*)targetAddr, sizeof(struct sockaddr_in));
	if (bytes_sent==-1) 
	{
		perror("Error\n"); 					// If there is an error
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Mission start\n");		//If it's done
	}
	memset(buf, 0, BUFFER_LENGTH);
}


/**
 * @brief      	Send the order to arm motors
 *
 * @param		sock		the socket
 * @param		targetAddr	the structure of the target socket
 *			   
 */
void armMotors(int *sock, struct sockaddr_in *targetAddr){
	
	int bytes_sent;
	uint16_t len;
	mavlink_message_t msg;						// Variable of type mavlink message
	uint8_t buf[BUFFER_LENGTH];
	
	
	/* Packing the type of message you want in msg variable*/
	mavlink_msg_command_long_pack(255, 0, &msg, 1, 1, MAV_CMD_COMPONENT_ARM_DISARM, 0, 1, 0, 0, 0, 0, 0, 0);	//Request arm motors
	
	/* Put it in the buffer */
	len = mavlink_msg_to_send_buffer(buf, &msg);
	/* Send it */
	bytes_sent = sendto(*sock, buf, len, 0, (struct sockaddr*)targetAddr, sizeof(struct sockaddr_in));
	if (bytes_sent==-1) 
	{
		perror("Error\n"); 					// If there is an error
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Arming motors\n");		//If it's done
	}
	memset(buf, 0, BUFFER_LENGTH);
}


/**
 * @brief      	Send a set mode
 *
 * @param		sock		the socket
 * @param		targetAddr	the structure of the target socket
 *			   
 */
void sendSetmode(int *sock, struct sockaddr_in *targetAddr){
	
	int bytes_sent;
	uint16_t len;
	mavlink_message_t msg;						// Variable of type mavlink message
	uint8_t buf[BUFFER_LENGTH];
	
	
	/* Packing the type of message you want in msg variable*/
	mavlink_msg_set_mode_pack(255, 0, &msg, 1, MAV_MODE_GUIDED_DISARMED+MAV_MODE_FLAG_CUSTOM_MODE_ENABLED, COPTER_MODE_GUIDED); // Request change flight mode
	
	/* Put it in the buffer */
	len = mavlink_msg_to_send_buffer(buf, &msg);
	/* Send it */
	bytes_sent = sendto(*sock, buf, len, 0, (struct sockaddr*)targetAddr, sizeof(struct sockaddr_in));
	if (bytes_sent==-1) 
	{
		perror("Error\n"); 					// If there is an error
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Changing mode sent\n");		//If it's done
	}
	memset(buf, 0, BUFFER_LENGTH);
}


/**
 * @brief      	Send a mission item take off
 *
 * @param		sock		the socket
 * @param		targetAddr	the structure of the target socket
 *			   
 */
void sendTakeoff(int *sock, struct sockaddr_in *targetAddr){
	
	int bytes_sent;
	uint16_t len;
	mavlink_message_t msg;						// Variable of type mavlink message
	uint8_t buf[BUFFER_LENGTH];
	
	
	/* Packing the type of message you want in msg variable*/
	mavlink_msg_mission_item_pack(255, 0, &msg, 1, 190, 1, MAV_FRAME_GLOBAL_RELATIVE_ALT, MAV_CMD_NAV_TAKEOFF, 0, 1, 15, 0, 0, 0, 0, 0, 1);	//MISSION ITEM TAKEOFF 1m
	
	/* Put it in the buffer */
	len = mavlink_msg_to_send_buffer(buf, &msg);
	/* Send it */
	bytes_sent = sendto(*sock, buf, len, 0, (struct sockaddr*)targetAddr, sizeof(struct sockaddr_in));
	if (bytes_sent==-1) 
	{
		perror("Error\n"); 					// If there is an error
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Mission item takeoff sent\n");		//If it's done
	}
	memset(buf, 0, BUFFER_LENGTH);
}


/**
 * @brief      	Send a mission item waypoint
 *
 * @param		sock		the socket
 * @param		targetAddr	the structure of the target socket
 * @param		gps_raw_int Variable of type mavlink gps_raw 
 *			   
 */
void sendWaypoint(int *sock, struct sockaddr_in *targetAddr, mavlink_gps_raw_int_t gps_raw_int){
	
	int bytes_sent;
	uint16_t len;
	mavlink_message_t msg;						// Variable of type mavlink message
	uint8_t buf[BUFFER_LENGTH];
	
	
	/* Packing the type of message you want in msg variable*/
	mavlink_msg_mission_item_pack(255, 0, &msg, 1, 190, 0, MAV_FRAME_GLOBAL, MAV_CMD_NAV_WAYPOINT, 1, 1, 0, 0, 0, 0, gps_raw_int.lat, gps_raw_int.lon, gps_raw_int.alt);	//MISSION ITEM WAYPOINT
	
	/* Put it in the buffer */
	len = mavlink_msg_to_send_buffer(buf, &msg);
	/* Send it */
	bytes_sent = sendto(*sock, buf, len, 0, (struct sockaddr*)targetAddr, sizeof(struct sockaddr_in));
	if (bytes_sent==-1) 
	{
		perror("Error\n"); 					// If there is an error
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Mission item waypoint sent\n");		//If it's done
	}
	memset(buf, 0, BUFFER_LENGTH);
}


/**
 * @brief      	Send the mission count
 *
 * @param		sock		the socket
 * @param		targetAddr	the structure of the target socket
 *			   
 */
void sendMissionCount(int *sock, struct sockaddr_in *targetAddr){
	
	int bytes_sent;
	uint16_t len;
	mavlink_message_t msg;						// Variable of type mavlink message
	uint8_t buf[BUFFER_LENGTH];
	
	
	/* Packing the type of message you want in msg variable*/
	mavlink_msg_mission_count_pack(255, 0, &msg, 1, 190, 2);	//MISSION COUNT
	
	/* Put it in the buffer */
	len = mavlink_msg_to_send_buffer(buf, &msg);
	/* Send it */
	bytes_sent = sendto(*sock, buf, len, 0, (struct sockaddr*)targetAddr, sizeof(struct sockaddr_in));
	if (bytes_sent==-1) 
	{
		perror("Error\n"); 					// If there is an error
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Mission count sent\n");		//If it's done
	}
	memset(buf, 0, BUFFER_LENGTH);
}


/**
 * @brief      	Init the socket to receive datagram and support UDP protocol
 *
 * @param		sock	the socket
 * @param		locAddr		the structure of the local socket
 * @param		targetAddr	the structure of the target socket
 * @param		local_port	the listening port
 * @param		target_ip	the IP adress of the target
 *			   
 * @return     -1 if there is a problem, 0 else
 */
int initialize_UDP(int *sock, struct sockaddr_in *locAddr, struct sockaddr_in *targetAddr, int local_port, char *target_ip){
	
	struct sockaddr_in possibleTarget;						// The possible socket which will received our messages
  	socklen_t possibleTargetLen = sizeof(possibleTarget);
	uint8_t buf[BUFFER_LENGTH];
	
	int timeout = 10;							//Time in second for waiting an answer from the server
  	time_t currentTime;
  	time_t startTime = time(&currentTime);
  	double timeLeft = 0;
	
	*sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	locAddr->sin_family = AF_INET;
	locAddr->sin_addr.s_addr = INADDR_ANY;
	locAddr->sin_port = htons(local_port);
	memset (locAddr->sin_zero, 0, sizeof(locAddr->sin_zero));
	
	if (-1 == bind(*sock,(struct sockaddr *)locAddr, sizeof(struct sockaddr)))
	{
		perror("error bind failed");
		close(*sock);
    	return -1;
	}
	
	/* Initialization listenning done */
	printf("INIT listenning :\nUDPin: 0.0.0.0:%d\n", ntohs(locAddr->sin_port));
	
	/* Attempt to make it non blocking */
	#if (defined __QNX__) | (defined __QNXNTO__)
	if (fcntl(*sock, F_SETFL, O_NONBLOCK | FASYNC) < 0)
	#else
	if (fcntl(*sock, F_SETFL, O_NONBLOCK | O_ASYNC) < 0)
	#endif
	{
		fprintf(stderr, "error setting nonblocking: %s\n", strerror(errno));
		close(*sock);
    	return -1;
	}
	
	
  	/* While we don't find a packet which can indicate sending port we read all messages received on our port */
	while (recvfrom(*sock, buf, sizeof(buf), 0, (struct sockaddr*)(&possibleTarget), &possibleTargetLen)<=0 || possibleTarget.sin_addr.s_addr != inet_addr(target_ip)) 
	{
		
		memset(buf,0,256);
		time(&currentTime);
		timeLeft = difftime(currentTime, startTime);
		if (timeLeft > timeout)
		{
			perror("Connection time out");
			close(*sock);
			return -1;	
		}
	}

	/* Init the socket to send datagram and support UDP protocol */
	targetAddr->sin_family = AF_INET;
	targetAddr->sin_addr.s_addr = inet_addr(target_ip);
	targetAddr->sin_port = possibleTarget.sin_port;
	memset (targetAddr->sin_zero, 0, sizeof(targetAddr->sin_zero));

	/* Initialization sending done */
	printf("INIT target : UDPout : %s:%d\n",target_ip,ntohs(targetAddr->sin_port));

	return 0;
}


/**
 * @brief      Main
 *			   
 * @return     0
 */
int main(void)
{
	
	int sock;
	struct sockaddr_in targetAddr;				// The socket of the target address
	struct sockaddr_in locAddr;					// The socket of the local address
	uint8_t buf[BUFFER_LENGTH];
	ssize_t recsize;
	socklen_t fromlen;
	int bytes_sent;
	uint16_t len;
	
	mavlink_channel_t chan = MAVLINK_COMM_0;	// Variable of type mavlink channel 
	mavlink_gps_raw_int_t gps_raw_int;			// Variable of type mavlink gps_raw 
	mavlink_message_t msg;						// Variable of type mavlink message
	mavlink_status_t status;					// Variable of type mavlink status 
	
	char target_ip[100];
	strcpy(target_ip, "10.1.1.1");				//IP adress of the controler
	int local_port = 14550;		 				//Listening port
	
	int i;
	unsigned int temp = 0;
	

	/* Init the socket to receive and send datagram and support UDP protocol */
	if(initialize_UDP(&sock, &locAddr, &targetAddr, local_port, target_ip)==-1){
		return -1;
	}
	
	
	/* We need informations about GPS on the next step 
	Receive packets while we don't receive GPS_RAW_INT message */
	while (msg.msgid != MAVLINK_MSG_ID_GPS_RAW_INT)
	{	
		memset(buf, 0, BUFFER_LENGTH);
		recsize = recvfrom(sock, (void *)buf, BUFFER_LENGTH, 0, (struct sockaddr *)&targetAddr, &fromlen);	// reception
		/* Something received */
		if (recsize > 0)
		{
			
			/* For each part of the tram */
			for (i = 0; i < recsize; ++i)
			{	
				/* Parse the tram in order to get a mavlink message */
				if (mavlink_parse_char(chan, buf[i], &msg, &status))
				{
					/* Decoding message */
					if(msg.msgid == MAVLINK_MSG_ID_GPS_RAW_INT) //If the message is of type GPS_RAW_INT
					{	
						/* Decode informations of the message and put it in the variable */
						mavlink_msg_gps_raw_int_decode(&msg, &gps_raw_int);
					}				
				}
			}
		}
	}
	

	sendMissionCount(&sock, &targetAddr);
	
	sendWaypoint(&sock, &targetAddr, gps_raw_int);
	
	sendTakeoff(&sock, &targetAddr);
	
	sendSetmode(&sock, &targetAddr);
	
	armMotors(&sock, &targetAddr);
	
	startMission(&sock, &targetAddr);
	
	
	/* End */
	close(sock);	
	return 0;
}