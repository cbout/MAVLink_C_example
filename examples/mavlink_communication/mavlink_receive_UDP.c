/**********************************************************************************************************************************************************
 Copyright  ETIS — ENSEA, Université de Cergy-Pontoise, CNRS (1991-2018)
 promethe@ensea.fr


 Short program to see how to listen and receive mavlink packets in UDP.
 This program is an extension of the program mavlink_initialization_UDP.c.
 After performing actions of the previous program to initialize socket.
 We receive packets infinitely.
 To do that, we listen on the previously initialized socket, receive datagram into a buffer, 
 print frames and parse them in order to have a mavlink_message that we can interpret.

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
 gcc -std=c99 -I ../../include/c_library_v1/ardupilotmega mavlink_receive_UDP.c -o mavlink_receive_UDP
 
 Execution : 
 ./mavlink_receive_UDP

**********************************************************************************************************************************************************/
/*********************************************************************************************************************************************************
 Authors: Raphael Bergoin, Clement Bout
 Created: 10/2018
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
 * @brief      Main
 *			   
 * @return     0
 */
int main(void)
{
	
	int sock;
	struct sockaddr_in locAddr;
	struct sockaddr_in targetAddr;
	uint8_t buf[BUFFER_LENGTH];
	ssize_t recsize;
	socklen_t fromlen;
	int local_port = 14550;		 //Listening port
	
	int i;
	unsigned int temp = 0;
	
	mavlink_channel_t chan = MAVLINK_COMM_0;	// Variable of type mavlink channel 
	mavlink_message_t msg;						// Variable of type mavlink message 
	mavlink_status_t status;					// Variable of type mavlink status 
	

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
	while (1)
	{
		
		memset(buf, 0, BUFFER_LENGTH);
		recsize = recvfrom(sock, (void *)buf, BUFFER_LENGTH, 0, (struct sockaddr *)&targetAddr, &fromlen);	// reception
		/* Something received */
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
					printf("\nReceived packet: SYS: %d, COMP: %d, LEN: %d, MSG ID: %d\n\n", msg.sysid, msg.compid, msg.len, msg.msgid);
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