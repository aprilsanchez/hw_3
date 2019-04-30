Name: April Sanchez
Date: 4/29/2019

I consulted https://www.geeksforgeeks.org/socket-programming-cc/ and https://bitwordblog.wordpress.com/2017/01/17/socket-programming-in-c/ to learn more
about socket programming in C. I also referenced the sample server and client code the cs176a section slides.

TCP server: remains open indefinitely unless an error is encountered. Otherwise, sends messages back to the client
containing the sum. Does this until sum is one digit, then closes the connection to the client and waits for a new
client to send a message.
TCP client: Sends a message to the server. If the message contains a letter, an error message is returned and the
connection is close. Otherwise, client reads in sums until the last sum is one digit, then the connection to the server
is closed.
UCP client: Similar to TCP, except UDP client can continue to send messages to the server. To stop sending messages to
the server, enter 'Q' when prompted with "Enter string: "
UDP server: Performs similar to TCP server, but if the server receives just the letter 'Q', it closes the connection.
