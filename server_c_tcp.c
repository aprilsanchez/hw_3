#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <netdb.h>
#include <stdbool.h>

void err(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {

    int serverSocket;
    int newSocket;
    int portNum;

    socklen_t clientLength;
    struct sockaddr_in serverAddress, clientAddress;
    int num;
    char buffer[129]; //increased buffer size from 128 to 129 to accommodate for \0 from fgets()
    
    //check for correct amount of arguments
    if (argc < 2) {
        fprintf(stderr, "ERROR, port was not provided");
        exit(1);
    }

    // create server's socket and bind port, check for errors
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0) {
        err("ERROR opening socket");
    }

    bzero( (char *) &serverAddress, sizeof(serverAddress));
    portNum = atoi(argv[1]);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(portNum);

    if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        err("ERROR, could not bind");
    }
    
    while(1) {
        listen(serverSocket,5);
        clientLength = sizeof(clientAddress);
        newSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientLength);
    
        if(newSocket < 0) {
            err("ERROR, could not accept");
        }

        bzero(buffer, 129);
        
        //read incoming message from the client
        bool letterDetected = false;      
        num = read(newSocket, buffer, 129);
        
        if (num == 0) {
            bzero(buffer, 129);
            close(newSocket);
            continue;
        }
        else if(num < 0) {
            err("ERROR receiving");
        }

        //check if there are any letters, if yes, break connection, else just start adding up all the integers into a sum
        int sum = 0;
        for (int i = 0; i < strlen(buffer)-1; i++) {
            //if there are letters
            if ( (buffer[i] < 48) || (buffer[i] > 57) ) {
                num = write(newSocket, "Sorry, cannot compute!",22);                
                if (num < 0) {
                    err("ERROR writing to socket");
                }
                close(newSocket);
                letterDetected = true;
                break;
            }    
            else {  //add up the numbers
                sum += (int)(buffer[i] - 48);
            }
        }
        if (letterDetected) {   //move on in the while loop, wait for next client            
            bzero(buffer, 129);
            //close(newSocket);
            continue;
        }
        //else send sum to client 
        bzero(buffer, 129);
        sprintf(buffer, "%d", sum);     
        num = write(newSocket, buffer, 129);
        
        //if sum is one digit, close connection 
        if (sum < 10) {
            bzero(buffer, 129);
            
        }
        else {  //keep calculating new sums and sending them to client until sum is just one digit long
                                
            while (sum > 9) {
                sum = 0;
                    
                for (int i = 0; i < strlen(buffer); i++) {
                    sum += (int)(buffer[i]-48);
                }

                bzero(buffer, 129);
                sprintf(buffer, "%d", sum);
                num = write(newSocket, buffer, 129);
                
            }
            bzero(buffer, 129); 
        }
        close(newSocket);    
    }

    close(serverSocket);
    return 0;
}
