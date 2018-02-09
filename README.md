# DNS-Domain
A client and a server application which communicates based on TCP/IP protocol. The server uses a text file fostering the domain and IP database. Different clients can add, delete, modify, request IP or URL from the database. A binary search tree has been used to implement search functionality and improve performance over a linked list server. 


The Application consists of two parts, a client and a server which communicate based on TCP/IP protocol. The server uses a text file fostering the domain and IP database. In the text file each row shows a domain name as well as number of times that the record has been requested followed by the IP address(es) which correspond to that domain. 
Sample content of the text file is as follow:
<br />
www.yahoo.com 5 98.138.253.109 206.190.36.45 98.139.183.24
www.google.com 0 64.233.169.99
www.umd.edu 15 128.8.237.77

When the server starts, it reads the data from the file into linked list. Server is capable of performing following tasks:
1. Return the IP address(es) for a domain
        a. If the IP address exists in the data file, it will be returned. In case of multiple addresses all must be returned.
        b. If the IP address does not exist, the server tries to find and add it to the database (hint: use gethostbyname()).
        c. If nothing is found, appropriate message is communicated
2. Keep track of number of requests for each record
3. Add new record to the list
4. Delete a record from the list
5. Reject inquiry if another inquiry has been received from the originated address in the last X seconds.

The server receives port number, data file name and accepted time gap between multiple requests (in seconds) as command line parameters.
The client uses command line arguments to communicate with the server. The first command line parameter is the server IP and the second parameter is the port number. The third parameter specifies the type of request from the server and can have the following values:

Request code   Action
           1   Find IP for a domain
           2   Add a record to the list
           3   Delete a record from the list
           4   Report the most requested record(s)
           5   Report the least requested record(s)
           6   Shut down
Server Side
--------------   Command Line argument format description   ---------
1. To compile the server                      : gcc -o server Server.c
2. To execute the server                      : ./server <port number> <file_name.txt> <timer_in_seconds*10)
                      
Some of the requests need extra parameters which will follow the third parameter. 
Here are some examples (assume that the .exe file name is client):
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

The last command (shut down) is only executed if the request is followed by a predetermined security code. Upon shut down changes to the list are written into the data file. Both client and server detect errors in the input and respond properly. Error checking is done either at the client or the server side. If you check for the input errors at the server side, the server transmits appropriate error message to the client should an error occur. The following are some errors that the application handles:

• Number of command line parameters is not enough for the request
• The record to be deleted does not exist
• The record to be added already exists
• The IP address format is wrong (e.g. 999.888.123.3 is not a valid IP address)
• Another inquiry had been made 6 seconds ago, wait 10 seconds before another submission

Note that the program is case insensitive (i.e. www.ebay.com and www.eBay.com are the same)

A binary search tree server is implemented to improve the performance over a linked list.

A log file is generated for all the events that occur on the server side. Each line in the log file starts with a date and time, and provides detailed information for the event. 
The following shows an example: 
2011/12/8 5:30 (“www.test.com” , “123.2.3.45”) added by client 64.2.1.56 
2011/12/8 5:32 request for shutting down the server from client 64.2.1.56 failed 
2011/12/8 18:32 Server shut down by the client 128.2.132.54

