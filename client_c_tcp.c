#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <unistd.h>
#include <netdb.h>


void err(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char const *argv[])
{
    //structures for handling internet addresses
    struct sockaddr_in serverAddress;
    struct hostent *server;

    //for when we create the client socket
    int clientSocket;
    int num;
    int portNum;
        
    char buffer[129];

    //work referenced: http://www.linuxhowtos.org/data/6/client.c
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    portNum = atoi(argv[2]); 
    //modified version of code from GeeksForGeeks(citation in README)

    clientSocket = socket(AF_INET, SOCK_STREAM,0);
    //if socket failed to create a endpoint for communication
    if (clientSocket < 0) {
        printf("\nSocket creation error\n");
        return -1;
    }
    server = gethostbyname(argv[1]);
    if (server == NULL){
        err("ERROR, no such host");
    }
    
    bzero((char *) &serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serverAddress.sin_addr.s_addr, server->h_length);
    serverAddress.sin_port = htons(portNum); //converts unsigned short int to network byte order

    //now connect the socket to the serverAddress, if connect returns -1, connection failed
    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        err("ERROR connecting");
    }

    
    printf("Enter string: ");
    bzero(buffer,129);
    fgets(buffer,129, stdin);   //adds null terminating character when done reading in
    
    //check for more than 128 characters
    int found = 0;
    for (int i = 0; i < 129; i++) {
        if (buffer[i] == '\n') {
            found = 1;
        }
    }

    if (!found) {
        close(clientSocket);
        err("Too many characters! The max is 128\n");
    }

    num = write(clientSocket, buffer, strlen(buffer)); 

    if (num < 0) {
        err("ERROR writing to socket");
    }

    bzero(buffer, 129);
        
       
    while(1) {
        num = recv(clientSocket, buffer, 129,0);
        if (num == 0) {
            break;
        }
        printf("From server: ");
        printf("%s\n",buffer);
        
        bzero(buffer, 129);
          
        if (strlen(buffer) == 1) {
            //printf("in size == 1");
            //close(clientSocket);
            break;
        }
        
        
    }       
    //printf("outside the read while loop\n");
    if (num == -1) {
        err("ERROR reading from server");
    }
    close(clientSocket);
    
    return 0;

}
