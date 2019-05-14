#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <time.h>
#include <netdb.h>


int main(int argc, char const *argv[] ) {

    if (argc < 3) {
        fprintf(stderr, "format: host port\n");
        exit(1);
    }
    
    int clientSocket, n, sent;
    int bufferSize= 256;
    char buffer[bufferSize];

    //send message format: PING seqNum time (i.e the time the client sends the message)
    //to append one string to another, use strcat(base, add_on)

    //structures for handling internet addresses
    struct hostent *hp;
    struct sockaddr_in server, from;
    unsigned int length = sizeof(struct sockaddr_in);

    //Create socket file descriptor, check for error in creating socket
    if ( (clientSocket = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 ) {
        perror("Failed to create socket\n");
        exit(1);
    }

    server.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);

    if(hp == 0) {
        perror("Could not find host\n");
        exit(1);
    }

    bcopy( (char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length );
    server.sin_port = htons(atoi(argv[2]));

    int seqNum = 1;
    int count = 0;
    int transmitted = 0;
    int received = 0;
    float RTTs[10];
    char temp[20];
    char space[2] = " ";
    struct timespec time_sent, time_recv;
   
    
    struct timeval timeout = {1,0};    //set timeout for 1 second
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval));

    while (seqNum <= 10) {
        //prepare message to send
        bzero(buffer, bufferSize);
        sprintf(buffer, "PING seq#=");
        bzero(temp,20);    
        sprintf(temp, "%d", seqNum);    //add seq#
        strcat(buffer, temp);
        strcat(buffer, space);
        strcat(buffer, " time=");

        clock_gettime(CLOCK_MONOTONIC, &time_sent);
        //time_sent.tv_nsec = time_sent.tv_nsec * (0.000001); //convert time from nanosec to millisec
        bzero(temp, 20);
        sprintf(temp, "%-2f", time_sent.tv_nsec*0.000001); 
        strcat(buffer, temp);   //add time in milliseconds
        strcat(buffer, " ms");

        //send message to server
        sent = sendto(clientSocket, buffer, bufferSize, 0, (struct sockaddr *)&server, length);
        
        //if sent
        if (sent >= 0 ) {
            transmitted = transmitted + 1;
        }
        else {  //if failed to send, send again
            continue;
        }

        //wait to receive message back from server, timeout after 1 second
        n = recvfrom(clientSocket, buffer, bufferSize, 0, (struct sockaddr *)&from, &length); 
        clock_gettime(CLOCK_MONOTONIC, &time_recv);
 

        //if received message
        if (n >= 0) {
            received = received + 1;
            RTTs[count] = (time_recv.tv_nsec - time_sent.tv_nsec) * 0.000001;  //calculate RTT
            printf("PING received from %s", argv[1]);
            printf(": seq#=%d", seqNum);
            printf(" time=");
            printf("%-2f", RTTs[count]);
            printf(" ms\n");
    
            seqNum = seqNum + 1;
            count = count + 1;

        }
        //if timeout after 1 second
        else {
            printf("Request timeout for icmp_seq %d",seqNum);
            printf("\n");
            RTTs[count] = -1.0; //negative one just indicates that this RTT is "null"
            count = count + 1;
            seqNum = seqNum + 1;
            continue;
        }

        //delay for one second before proceeding
        sleep(1);
    }
    
    //calculate percentage of packet loss
    int Z = (10 - received) * 10;

    //calculate MIN, AVG, and MAX rtt
    //find index we can arbitrarily assign max and min rtt to (it can't be an index where the value is -1)
    int index = 0;
    while (RTTs[index] == -1.0) {
        index = index + 1;
    }

    float max_rtt = RTTs[index]; //careful, there could be a case where the first value is "null" (i.e RTTs[0] = -1) -> edge case resolved
    float min_rtt = RTTs[index];
    float ave_rtt = 0;
    
    for (int i = 0; i < 10; i++) {
        if (RTTs[i] >= 0) {
            ave_rtt = ave_rtt +  RTTs[i];
        
            if (RTTs[i] > max_rtt) {
                max_rtt = RTTs[i];
            }

            if (RTTs[i] < min_rtt) {
                min_rtt = RTTs[i];
            }
        }
    }

    ave_rtt = ave_rtt / received;
    printf("--- ping statistics --- %d", transmitted);
    printf(" packets transmitted, %d", received);
    printf(" received, %d", Z);
    printf("%% packet loss rtt min/ave/max = %-2f %-2f %-2f ms\n", min_rtt, ave_rtt, max_rtt);


}
