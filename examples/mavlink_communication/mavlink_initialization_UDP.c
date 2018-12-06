/**********************************************************************************************************************************************************
 Copyright  ETIS — ENSEA, Université de Cergy-Pontoise, CNRS (1991-2018)
 promethe@ensea.fr


 Short program to see how to initialize UDP connection for listen and send packets.
 This program is the entry to understand how mavlink communication works.
 Here we first initialize the socket to receive datagram and support UDP protocol.
 Then we search the target port by listening to incoming packets.
 When a packet comes from the target ip, we initialize the socket to send datagram and so support UDP protocol.

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
 gcc mavlink_initialization_UDP.c -o mavlink_initialization_UDP
 
 Execution : 
 ./mavlink_initialization_UDP

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


/**
 * @brief      Main
 *
 * @return     0
 */
int main(void)
{
	
	int sock;
	struct sockaddr_in targetAddr;			// The socket of the target address
	struct sockaddr_in locAddr;				// The socket of the local address
	struct sockaddr_in possibleTarget;		// The possible socket which will received our messages
  	socklen_t possibleTargetLen = sizeof(possibleTarget);
	uint8_t buf[256];
	
	char target_ip[100];
	strcpy(target_ip, "10.1.1.1");			// IP adress of the controler
	int local_port = 14550;		 			// Listening port
	
	int timeout = 10;						// Time in second for waiting an answer from the server
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

	
	/* End */
	close(sock);	
	return 0;
}