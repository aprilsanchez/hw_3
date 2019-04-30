// UDP Server- based off sample code from linuxhowtos on DS-04-17 slides
// https://www.cs.rutgers.edu/~pxk/417/notes/sockets/udp.html (also a source)
// converting int to string from here: https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
	struct sockaddr_in server, from;
	char buf[129];
	socklen_t fromlen;
	if(argc<2)
	{
		fprintf(stderr, "No port provided\n"); 
		exit(1);
	}
	int sockid = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockid<0)
	{ perror("Cannot create socket!\n"); }
	int length = sizeof(server);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	// argv[1] should be user entered port num
	server.sin_port = htons(atoi(argv[1]));
	if( bind(sockid, (const struct sockaddr *)& server, length)<0)
	{ 
		perror("Bind failed!\n"); 
		exit(1);
	}
	fromlen = sizeof(from);

	int rec, sent;
	while(1)
	{
	   bzero(buf, 129);
	   rec = recvfrom(sockid, buf, 129, 0, (struct sockaddr *)& from, &fromlen);
	   if (rec<0)
	   { perror("Receive Error"); }
	   if(buf[0]=='Q')
	   {
		   printf("Received signal to close!\n");
		   close(sockid);
		   exit(0);
	   }
	   if(buf[0]=='\n')
	   {
		   sent = sendto(sockid,"Z",1, 0,(struct sockaddr *)&from,fromlen);
			   close(sockid);
			   exit(1);

	   }
	   // check that all are integers, form sum
	   int sum = 0, sendLen = 0;
       	   for(int i=0; i<rec-1; i++)
	   {
		   if(!isdigit(buf[i]))
		   {
			   sent = sendto(sockid,"Z",1, 0,(struct sockaddr *)&from,fromlen);
			   close(sockid);
			   exit(1);
		   }
		   else
		   { sum+= (int)(buf[i]-48); }
	   }
	   for(int i=sum; i>0; i/=10)
	   { sendLen+=1; }
	   char sendBuf[129];
           sprintf(sendBuf, "%d", sum);
           sendBuf[sendLen] = '\n';

	   sent = sendto(sockid, sendBuf, sendLen, 0, (struct sockaddr *)& from, fromlen);
	
	   if(sent<0)
	   {
		   perror("Send Error\n");
		   exit(1);
	   }

	   if (sum<10)
	   { bzero(buf,129);  }
	   else
	   {
		while(sum>9)
		{
			sum = 0;
			for(int i=0; i<sendLen; i++)
			{
				sum += (int)(sendBuf[i]-48);
			}
			bzero(sendBuf, 129);
			sprintf(sendBuf, "%d", sum);
			sendLen = 0;
	       	        for(int i=sum; i>0; i/=10)
	                { sendLen+=1; }

			sendBuf[sendLen] = '\n';
			sent = sendto(sockid, sendBuf, sendLen, 0, (struct sockaddr *)& from, fromlen);
			if(sent<0)
			{
				perror("Send Error\n");
				exit(1);
			}

		}
	   }
	}
	return (0);

}
