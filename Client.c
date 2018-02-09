
/*
*********************   ENTS 689N     *********************
Aakash Aggarwal (UID:115586437)
Abhinav Sharma  (UID:115627965)

11 11111111   
11 11    11
11 11111111
11 11
11 11
11 11   & Domain Dossier System

*****************   Network Programming     *********************

TCP/IP Client to take command line arguments to send to the server to perform the following tasks:

Request code   Action
           1   Find IP for a domain
           2   Add a record to the list
           3   Delete a record from the list
           4   Report the most requested record(s)
           5   Report the least requested record(s)
           6   Shut down
Client Side
--------------   Command Line argument format description   ---------
1. To compile the client                      : gcc -o client Client.c
2. To execute the client                      : ./client <port number> <Request code> <Optional Argument>
3. To search for an IP against a URL          : ./client 1070 1 www.abcd.com
4. To add a new entry to file                 : ./client 127.0.0.1 1070 2 www.ebay.com 10.125.6.138
5. To delete a record fron the list           : ./client 127.0.0.1 1070 3 www.abcd.com
6. To get the most requested record           : ./client 127.0.0.1 1070 4
7. To get the least requested record          : ./client 127.0.0.1 1070 5
8. To shut down the server using code         : ./client 127.0.0.1 1070 6 <SECURITY_CODE>

Server Side
--------------   Command Line argument format description   ---------
1. To compile the server                      : gcc -o server Server.c
2. To execute the server                      : ./server <port number> <file_name.txt> <timer_in_seconds*10)


**Security code is hard coded in the server.

*/

//Header Files

#include <stdio.h>      // HEADER FILE for print functions like printf() and fprintf()
#include <sys/socket.h> // HEADER FILE for system socket functions like socket(), connect(), send(), and recv()
#include <arpa/inet.h>  // HEADER FILE for sockaddr_in and inet_addr(), also in <netinet/in.h>
#include <stdlib.h>     // HEADER FILE for atoi() and exit()
#include <string.h>     // HEADER FILE for string operation and memset()
#include <unistd.h>     // HEADER FILE for close()
#include <stdbool.h>    // HEADER FILE for checking ipAddress
#include <ctype.h>      // HEADER FILE for tolower()


// Function to validate the IP Address entered by Client, returns a boolean, used instead of CHECKMAC or CHECKIP we did for assignment

bool isValidIpAddress(char *ipAddress)
 {
 struct sockaddr_in sa;
 int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
 return result != 0;
 }


#define RCVBUFSIZE 100              // Size of buffer for receiving

//Function Declarations 

void DieWithError(char *errorMessage);  /* Error handling function */
char * toString(char str[], int num); 

//MAIN Method start

int main(int argc, char *argv[])
{
    int sock;                           // Socket descriptor
    struct sockaddr_in serverAddr;      // Echo server address
    unsigned short serverPort;          // Echo server port
    char *servIP;                       // Server IP address (dotted quad)
    char echoString[100];               // String to send to echo server
    char echoBuffer[RCVBUFSIZE];        // Buffer for echo string
    unsigned long echoStringLen;        // Length of string to echo
    long bytesRcvd, totalBytesRcvd;     // Bytes read in single recv() and total bytes read
    char * action;                      // Type of request from client to server
    char * domainName;                  // For Domain name argument
    char * ipToAdd;
    char str[2];                        //= argc;
    
    if ((argc < 4) || (argc > 6))       // Test for correct number of arguments
    {
        printf("\n\t\tIncorrect number of command line arguments for the request.");
        exit(1);
    }
    
    servIP = argv[1];                   // First argument: server IP address (dotted quad)
    serverPort = atoi(argv[2]);         // Second argument: Use given port, if any
    action = argv[3];                   // Third argument: to inform server about the requested action
    
    strcpy(echoString,toString(str,argc));
    strcat(echoString, "#");            // Formatting the string to be sent with "#" in between args
    strcat(echoString, action);
    strcat(echoString, "#");            // Check if valid action code is entered
    
    if (atoi(action)> 6 || atoi(action) <0)
        DieWithError("Incorrect request code entered. Please enter correct code.");
    
    switch (argc){                      // Check for the number of args entered by the Client
        case 5:     domainName = argv[4];
                    strcat(echoString,domainName);
                    strcat(echoString,"#");
                    printf("\nCommand Sent: %s %s %s %s",argv[1],argv[2],argv[3],argv[4] );
                    break;
 
        case 6:     //To validate the IP Address format
                    if (isValidIpAddress(argv[5])){
                        domainName = argv[4];
                        strcat(echoString,domainName);  // Concatenate the domain name to the string
                        strcat(echoString," ");
                        ipToAdd = argv[5];
                        strcat(echoString,ipToAdd);     // Concatenate the IP to the string
                        strcat(echoString,"#");
                        printf("\nCommand Sent to server: %s %s %s %s %s",argv[1],argv[2],argv[3],argv[4], argv[5]);
                        break;
                    }
                    else
                        DieWithError("Invalid IP Address entered by the client");
        
        default:    printf("\nCommand Sent: %s %s %s",argv[1],argv[2],argv[3]);
                    break;
            
    }
    
    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");
    
    /* Construct the server address structure */
    memset(&serverAddr, 0, sizeof(serverAddr));         // Zero out structure
    serverAddr.sin_family      = AF_INET;               // Internet address family
    serverAddr.sin_addr.s_addr = inet_addr(servIP);     // Server IP address
    serverAddr.sin_port        = htons(serverPort);     // Server port
    
    /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
        DieWithError("connect() failed");
    
    echoStringLen = strlen(echoString);                 // Determine input length
    
    /* Converting all arguments to lowercase */
    if (strcmp(action,"6") != 0 ){
        for(int  i = 0; echoString[i]; i++){
            if(echoString[i]!= '#')
                echoString[i] = tolower(echoString[i]);
            }
    }

    /* Send the string to the server */
    if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
        DieWithError("send() sent a different number of bytes than expected");
    
    /* Receive the same string back from the server */
    totalBytesRcvd = 0;
    printf("\nReceived: ");                             // Setup to print the echoed string */
    
    /* Receive up to the buffer size (minus 1 to leave space for
     a null terminator) bytes from the sender */
    while (totalBytesRcvd < RCVBUFSIZE)
    {
        /* Receive up to the buffer size (minus 1 to leave space for
         a null terminator) bytes from the sender */
        if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
            DieWithError("recv() failed or connection closed prematurely");
        
        totalBytesRcvd += bytesRcvd;                // Keep tally of total bytes
        echoBuffer[bytesRcvd] = '\0';               // Terminate the string!
        printf("%s", echoBuffer);                   // Print the echo buffer
    }
    
    printf("%s", echoBuffer);                       // Print the echo buffer
    printf("\n");                                   // Print a final linefeed
    
    close(sock);                                    // CLOSE SOCKET
    exit(0);
}
//End of Main Function


// Die with error - Error handling function, from book
void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

//toString function
char * toString(char * str, int num)
{
    int i, rem, len = 0, n;
    
    n = num;
    while (n != 0)              // Iterates over to count the number of digits
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)   // Converts each digit into a character in the string
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem +'0' ;
    }
    str[len] = '\0';
    
    return str;                 // return the integer converted to String
}