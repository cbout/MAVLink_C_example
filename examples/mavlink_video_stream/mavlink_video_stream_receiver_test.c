#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define BUFFER_LENGTH 2041


/**
 * @brief      Receive video stream data from the solo 3DR drone
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
	
	
	/* Create a UDP server to get video stream*/
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