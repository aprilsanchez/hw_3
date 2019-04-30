//Jacqueline Mai
//Parts pulled from linuxhowtos example code on slides

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
	if(argc!=3)
	{
		perror("No IP address, or no port num!\n");
		exit(1);
	}
	int sockid = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockid<0)
	{ 
		perror("Error opening socket\n");
		exit(1);
	}
	
	struct sockaddr_in server, from;
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[2]));
	struct hostent *hp;
	hp = gethostbyname(argv[1]);
	
	if (hp==0)
	{ 
		perror("Unknown host!\n"); 
		exit(1);
	}
	
	bcopy((char*)hp->h_addr, (char*)&server.sin_addr, hp->h_length);
	unsigned int length = sizeof(struct sockaddr_in);
	printf("Enter string: \n");

	char buffer[129];
	bzero(buffer, 129);
	fgets(buffer, 129, stdin);
	int sent;
	if(buffer[0]=='Q')
	{
		sent = sendto(sockid, buffer, 129, 0, (const struct sockaddr*)& server, length);
		printf("Closing client socket. Goodbye!\n");
		close(sockid);
		exit(0);
	}
	if(buffer[strlen(buffer)-1]!='\n')
	{
		perror("Too many symbols!\n");
		bzero(buffer, 129);
		buffer[0] = 'Q';
		sent = sendto(sockid, buffer, 129, 0, (const struct sockaddr*)& server, length);
		close(sockid);
		exit(1);
	}
	sent = sendto(sockid, buffer, strlen(buffer), 0, (const struct sockaddr*)& server, length);
	if(sent<0)
	{ 
		perror("Error sending!\n");
	  	exit(1);
			
	}
	int rec = recvfrom(sockid, buffer, 129, 0, (struct sockaddr*)& from, &length);

	while(rec>=0)
	{
		write(1, "From server: ", 12);
		if(rec<0)
		{ 
			perror("Error receiving!\n"); 
			exit(1);
		}
		if(rec==1 && buffer[0]=='Z')
		{
			perror("Sorry, cannot compute!\n");
			close(sockid);
			exit(1);
		}
		else
		{ write(1, buffer, rec); }
		putchar('\n');

		if(rec==1)
		{
			printf("Only 1 digit!\n");
			printf("Enter string!\n");
			bzero(buffer, 129);
			fgets(buffer, 129, stdin);
			if(buffer[0]=='Q')
			{
				sent = sendto(sockid, buffer, 129, 0, (const struct sockaddr*)& server, length);
				printf("Closing client socket. Goodbye!\n");
				close(sockid);
				exit(0);
			}

			sent = sendto(sockid, buffer, strlen(buffer), 0, (const struct sockaddr*)& server, length);
		}
		putchar('\n');
		rec = recvfrom(sockid, buffer, strlen(buffer), 0, (struct sockaddr*)& from, &length);
	}
	close(sockid);
	return 0;

}
