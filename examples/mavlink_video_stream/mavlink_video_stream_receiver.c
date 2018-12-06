/**********************************************************************************************************************************************************
 Copyright  ETIS — ENSEA, Université de Cergy-Pontoise, CNRS (1991-2018)
 promethe@ensea.fr


 Short program to receive video stream data from the solo 3DR drone.
 This program is an entry to understand how mavlink video stream works.
 To do that we previously initialize a TCP connection in order to request video stream.
 Then we create a UDP server to get video stream and we print frames

 This software is governed by the CeCILL v2.1 license under French law and abiding by the rules of distribution of free software.
 You can use, modify and/ or redistribute the software under the terms of the CeCILL v2.1 license as circulated by CEA, CNRS and INRIA at the following URL "http://www.cecill.info".
 As a counterpart to the access to the source code and  rights to copy, modify and redistribute granted by the license,
 users are provided only with a limited warranty and the software's author, the holder of the economic rights,  and the successive licensors have only limited liability.
 In this respect, the user's attention is drawn to the risks associated with loading, using, modifying and/or developing or reproducing the software by the user in light of its specific status of free software,
 that may mean  that it is complicated to manipulate, and that also therefore means that it is reserved for developers and experienced professionals having in-depth computer knowledge.
 Users are therefore encouraged to load and test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data to be ensured
 and, more generally, to use and operate it in the same conditions as regards security.
 The fact that you are presently reading this means that you have had knowledge of the CeCILL v2.1 license and that you accept its terms.
 

 See more details about this program in the file : documentation_video_stream.
 
 
 Compilation : 
 cd /mavlink_video_stream
 gcc mavlink_video_stream_receiver.c -o mavlink_video_stream_receiver
 
 Execution : 
 ./mavlink_video_stream_receiver

**********************************************************************************************************************************************************/
/*********************************************************************************************************************************************************
 Authors: Raphael Bergoin, Clement Bout
 Created: 10/2018
**********************************************************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define BUFFER_LENGTH 2041


/**
 * @brief      Main
 *
 * @return     0
 */
int main(void)
{

	int s, s2, i;
	struct sockaddr_in serv_addr, locAddr;
	
	uint8_t buf[BUFFER_LENGTH];
	ssize_t recsize;
	socklen_t fromlen;
	unsigned int temp = 0;
	
	
	/* Initialize a TCP connection in order to request video stream */
	s = socket(PF_INET, SOCK_STREAM, 0);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr ("10.1.1.1");	// Address of the server
	serv_addr.sin_port = htons (5502);	// Port of the server
	memset (&serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero));
	
	connect (s, (struct sockaddr *)&serv_addr, sizeof serv_addr);
	
	
	/* Create a UDP server to get video stream */
	s2 = socket (PF_INET, SOCK_DGRAM, 0);
	
	locAddr.sin_family = AF_INET;
	locAddr.sin_addr.s_addr = INADDR_ANY;
	locAddr.sin_port = htons (5600);	// Listening port
	memset (&locAddr.sin_zero, 0, sizeof(locAddr.sin_zero));
	
	bind (s2, (struct sockaddr *)&locAddr, sizeof locAddr);

	/* Receive each tram */
	while(1)
	{
		memset(buf, 0, BUFFER_LENGTH);

		recsize = recvfrom(s2, (void *)buf, BUFFER_LENGTH, 0, (struct sockaddr *)&serv_addr, &fromlen);
		if (recsize > 0)
		{
			printf("\n\nBytes Received: %d\nDatagram: ", (int)recsize);
			for (i = 0; i < recsize; ++i)
			{
				temp = buf[i];
				
				/* Only keep datas without RTP header */
				if(i>=12)
				{
					printf("%02x ", (unsigned char)temp);
				}
			}
		}
		
		sleep(1);
	}

	close(s2);
	close(s);
	
	return 0;
}