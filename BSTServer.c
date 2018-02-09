
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

BST Server Side
--------------   Command Line argument format description   ---------
1. To compile the server                      : gcc -o server Server.c
2. To execute the server                      : ./server <port number> <file_name.txt> <timer_in_seconds*10)

This server implements a Binary search tree and reduces the total searches by half. 
Also, it provides logs with timestamp at the end. 

For the binary search tree portion, the linked list has been modified into a binary search tree where nodes are inserted and deleted using the following logic:
Since domain name for every entry is unique , its corresponding ASCII value is used as the value of the node.
Any value greater than the root node value is stored to the right and a lesser value is stored to the left.
This way the number of searches are reduced by half.

**Security code is hard coded in the server.

*/

//Header Files

#include <stdio.h>                          // Standard input and output
#include <sys/socket.h>                     // For socket(), connect(), send(), and recv()
#include <arpa/inet.h>                      // For sockaddr_in and inet_addr()
#include <stdlib.h>                         // For atoi() and exit()
#include <string.h>                         // For memset()
#include <unistd.h>                         // For close()
#include <netdb.h>                          // For gethostbyname()
#include <time.h>                           // For time_t
#define MAXCONNECTION 5                     // Maximum outstanding connection requests
#define RCVBUFSIZE 100                      // Size of receive buffer
#define SECURITYCODE "Abhinav"               // Pre-shared  security code


void DieWithError(char *errorMessage);  // Error handling function
										//void HandleTCPClient(int clntSocket);   /* TCP client handling function */

typedef struct lueue {
	char clientip[30];
	long int clk;
	struct lueue *next;

}l;
// rootnode to keep a track of the head poitner to l linked list 
typedef struct Roottime {
	l *head;
	int count_time;

}roottime;
//typedef roottime *rt;

int searchDomainNametime(char clientip[], long int t, roottime *rt, int thresh)/*searchDomainName by time Function to check if the request mafe is within thresh seconds by the same client  */
{
	l *temp = (l *)malloc(sizeof(l));
	temp = rt->head;
	while (temp != NULL)
	{
		if (strncmp(clientip, temp->clientip, strlen(clientip)) == 0)/* If the same client makes multiple requests */
		{

			long int diff = (t - temp->clk);
			if ((diff)>thresh) {
				temp->clk = t;// update to current time period

				return 1;
			}
			else if ((t - temp->clk) < thresh) {
				printf("\n multiple requests within %d s \n", thresh);
				DieWithError("\n cannot make multiple requests withn the threshold time  \n");
				//exit(0);
				return 1;

			}
		}
		else { temp = temp->next; }
	}

	return (0);

}


// to obtaing timestamp for the logfile
char * timestamp()
{
	char *timest = malloc(sizeof(char) * 30);
	time_t ltime; /* calendar time */
	ltime = time(NULL); /* get current cal time */
						//printf("%s", asctime(localtime(&ltime)));
	strcpy(timest, asctime(localtime(&ltime)));
	return timest;
}

struct bin_tree {
	char dns[20];
	char ip[128];
	char num[5];
	int val;
	struct bin_tree * right, *left;
};
typedef struct bin_tree node;

void add_domain(node ** tree, char dns[],char ip[],char num[])
{
	node *temp = NULL;
	if (!(*tree))
	{
		temp = (node *)malloc(sizeof(node));
		temp->left = temp->right = NULL;
		strcpy(temp->dns,dns);
		strcpy(temp->ip, ip);
		strcpy(temp->num, num);
		int j;
		temp->val = 0;
		for(j=0;j<strlen(temp->dns);j++)
		{
			int digit = temp->dns[j];
			temp->val = temp->val + digit;
		}
	
		//printf("the value of dns %d \n",temp->val);
		*tree = temp;
		
	}
	int val = 0;
	int i;
	for (i = 0; i<strlen(dns); i++)
	{
		int digit = dns[i];
		val = val + digit;
	}

	if (val < (*tree)->val)
	{
		add_domain(&(*tree)->left, dns,ip,num);
	}
	else if (val >(*tree)->val)
	{
		add_domain(&(*tree)->right, dns,ip,num);
	}

}




node* searchDomainName(node ** tree, char dns[])
{
	if (!(*tree))
	{
		return NULL;
	}
	int val=0;
	int i=0;
	for (i=0;i<strlen(dns);i++)
	{
		int digit = dns[i];
		val = val + digit;
	}
	if (val < (*tree)->val)
	{
		searchDomainName(&((*tree)->left), dns);
	}
	else if (val >(*tree)->val)
	{
		searchDomainName(&((*tree)->right), dns);
	}
	else if (val == (*tree)->val)
	{
		return *tree;
	}
}

/*returns the address of the node to be deleted, address of its parent and
whether the node is found or not */

void delete_domain(node **root, char dns[], node **par, node **x, int *found)
{
	node *q;

	q = *root;
	*found = 0;
	*par = NULL;
	int val = 0;
	int i = 0;
	for (i = 0; i<strlen(dns); i++)
	{
		int digit = dns[i];
		val = val + digit;
	}
	while (q != NULL)
	{
		/* if the node to be deleted is found */
		if (q->val == val)
		{
			*found = 1;
			*x = q;
			return;
		}

		*par = q;

		if (q->val > val)
			q = q->left;
		else
			q = q->right;
	}
}


/* deletes a node from the binary searchDomainName tree */

void delete (node ** tree, char dns[])
{
	int found;
	node *parent, *x, *xsucc;

	/* if tree is empty */
	if (*tree == NULL)
	{
		printf("\nTree is empty");
		return;
	}

	
	parent = x = NULL;
	/* call to searchDomainName function to find the node to be deleted */
	//printf("this is happening");

	delete_domain(tree, dns, &parent, &x, &found);
	
	//printf("found ip %s", found->ip);
	
	/* if the node to deleted is not found */
	if (found == 0)
	{
		printf("\nData to be deleted, not found");
		return;
	}

	/* if the node to be deleted has two children */
	if (x->left != NULL && x->right != NULL)
	{
		parent = x;
		xsucc = x->right;

		while (xsucc->left != NULL)
		{
			parent = xsucc;
			xsucc = xsucc->left;
		}
		strcpy(x->dns, xsucc->dns);
		strcpy(x->ip, xsucc->ip);
		strcpy(x->num, xsucc->num);
		
		x = xsucc;
	}

	/* if the node to be deleted has no child */
	if (x->left == NULL && x->right == NULL)
	{
		//printf("this is happening also");
		if (parent->right == found)
			parent->right = NULL;
		else
			parent->left = NULL;
			
		free(x);
		return;
	}

	/* if the node to be deleted has only rightchild */
	if (x->left == NULL && x->right != NULL)
	{
		if (parent->left == x)
			parent->left = x->right;
		else
			parent->right = x->right;

		free(x);
		return;
	}

	/* if the node to be deleted has only left child */
	if (x->left != NULL && x->right == NULL)
	{
		if (parent->left == x)
			parent->left = x->left;
		else
			parent->right = x->left;

		free(x);
		return;
	}
}


void print_preorder(node * tree)
{
	if (tree)
	{
		printf("%s %s %s \n", tree->dns, tree->ip, tree->num);

		print_preorder(tree->left);
		print_preorder(tree->right);
	}

}

void print_inorder(node * tree)
{
	if (tree)
	{
		print_inorder(tree->left);
		printf("%s %s %s \n", tree->dns, tree->ip, tree->num);
		print_inorder(tree->right);
	}
}

void print_postorder(node * tree)
{
	if (tree)
	{
		print_postorder(tree->left);
		print_postorder(tree->right);
		printf("%s %s %s \n", tree->dns, tree->ip, tree->num);
	}
}
void main(int argc, char *argv[])
{
	node *root;
	node *tmp;
	//int i;

	root = NULL;
	int servSock;                    /* Socket descriptor for server */
	int clntSock;                    /* Socket descriptor for client */
	struct sockaddr_in echoServAddr; /* Local address */
	struct sockaddr_in echoClntAddr; /* Client address */
	unsigned short echoServPort;     /* Server port */
	unsigned int clntLen;            /* Length of client address data structure */

	roottime *rt = calloc(1, sizeof(roottime));
	l *timea = malloc(sizeof(l));
	rt->head = NULL;
	//struct tm *tm;
	//time_t now;
	if (argc <4)     /* Test for correct number of arguments */
	{
		fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
		exit(1);
	}

	echoServPort = atoi(argv[1]);  /* first arg:  Local port */

	char *filename;
	filename = (argv[2]);/* second arg:  Path for the text file  */
	int thresh = (argv[3]);/* third arg:  threshhold value for multiple requests */

	/* Create socket for incoming connections */
	if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("socket() failed");

	/* Construct local address structure */
	memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
	echoServAddr.sin_family = AF_INET;                /* Internet address family */
	echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
	echoServAddr.sin_port = htons(echoServPort);      /* Local port */

													  /* Bind to the local address */
	if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
		DieWithError("bind() failed");

	/* Mark the socket so it will listen for incoming connections */
	if (listen(servSock, MAXCONNECTION) < 0)
		DieWithError("listen() failed");
	FILE *fp, *f1;
	fp = fopen(filename, "r");
	
	f1 = fopen(filename, "r");
	
	if (fp == NULL) {
		printf("Error: file pointer is null.");
		exit(1);
	}
	int maximumLineLength = 128;
	char *lineBuffer = (char *)malloc(sizeof(char) * maximumLineLength);
	if (lineBuffer == NULL) {
		printf("Error allocating memory for line buffer.");
		exit(1);
	}
	char ch;
	int count = 0;
	while (fgets(lineBuffer, maximumLineLength, fp) != NULL)/* Reading the text file and storing into linked list */
	{
		char *line = (char *)malloc(sizeof(char) * maximumLineLength);
		char ch = getc(f1);
		int count = 0;

		while ((ch != '\n') && (ch != EOF))
		{
			if (count == maximumLineLength)
			{
				maximumLineLength += 128;
				line = realloc(line, maximumLineLength);
				if (line == NULL) {
					printf("Error reallocating space for line buffer.");
					exit(1);
				}
			}
			line[count] = ch;
		
			count++;
			ch = getc(f1);
		}
		line[count] = '\0';
		char dns[30] = "";
		char num[5] = "";
		char(*ip) = calloc(128, sizeof(char));

		int j = 0; int k = 0, cnt = 0, i = 0, b = 0;
		while (i < strlen(line)) {

			//for(i=0;i<strlen(line);i++){
			if (line[i] == ' ') {

				cnt++;
				i++;
			}
			if (line[i] != ' ' && cnt == 0) {
				dns[j] = line[i];
				//printf("%c",dns[j]);
				j++;
				i++;
				continue;
			}
			else if (line[i] != ' ' && cnt == 1) {

				num[k] = line[i];
				k++;
				i++;
				continue;
			}
			else if (line[i] != '/0' && cnt >= 2)
			{
				int n;
				int a = 0;
				while (line[i] != '\0')
				{
					ip[a] = line[i];
					a++;
					i++;
				}

				
			}
			
		}
		//printf("%s \n",num);
		//printf("value of b is %d ",b);
		add_domain(&root, dns, ip,num);

		free(line);
	}
	printf("inorder Order Display\n");
	print_inorder(root);
	//Run in an infinite loop-accept  client socket	
	time_t clk = NULL;

	for (;;) /* Run forever */
	{
		/* Set the size of the in-out parameter */
		clntLen = sizeof(echoClntAddr);

		/* Wait for a client to connect */
		if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
			DieWithError("accept() failed");
		

		if (rt->head == NULL)
		{
			strcpy(timea->clientip, inet_ntoa(echoClntAddr.sin_addr));
			timea->clk = (unsigned long)time(NULL);

			//timea->next = NULL;
			timea->next = rt->head;
			rt->head = timea;
			//printf(" this is the new time at the header  %20d", rt->head->clk);
			rt->head->next = NULL;
			rt->count_time = 1;

		}
		else
		{
			l *timeb = malloc(sizeof(l));
			strcpy(timeb->clientip, inet_ntoa(echoClntAddr.sin_addr));
			timeb->clk = (unsigned long)time(NULL);
			//printf(" this is the new time %20d", timeb->clk);
			//printf("The root node time %ld", rt->head->clk);
			int temptime = searchDomainNametime(timeb->clientip, timeb->clk, rt, thresh);
			if (temptime == 0)
			{
				timeb->next = rt->head;
				rt->head = timeb;
				rt->count_time = rt->count_time + 1;
			}
		}

		/* clntSock is connected to a client! */

		printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
		char echoBuffer[32] = "";        /* Buffer for echo string */
		int recvMsgSize;                    /* Size of received message */
											/* Receive message from client */
		if ((recvMsgSize = recv(clntSock, echoBuffer, 32, 0)) < 0)
			DieWithError("recv() failed");

		printf("this is the echo buffer %s \n", echoBuffer);

		// extracting from the echo buffer
		char *token;
		char *next_token1 = NULL;
		//get the first token
		token = strtok(echoBuffer, " ");
		int com = atoi(token);
		node * temp1;
		node *temp2;
		char dns_searchDomainName[30] = "";
		char dns_add[30] = "";
		char ip_add[30] = "";
		char dns_del[30] = "";
		int max = 0;
		int min = 0;
		
		FILE *logfile = fopen("logfile.txt", "a");
		switch (com)   /* Perform different functions depending on te command code */
		{
		case 1:   /* serach for IP address for a given domain name */
			token = strtok(NULL, "\0");
			int d = 0;
			
			while (token[d])
			{
				token[d] = putchar(tolower(token[d]));
				d++;
			}

			strcpy(dns_searchDomainName, token);
			printf("this is the domain name %s", dns_searchDomainName);
			
			temp1 = searchDomainName(&root, dns_searchDomainName);
			char *ip = malloc(sizeof(char) * 25);
			//strcpy(echoBuffer, temp1->ip);
			if (temp1 == NULL)/* If tthe domain name is not in the linked list */
			{
				char *valid_ip = gethostbyname(dns_searchDomainName);

				printf("%s", valid_ip);
				if (send(clntSock, valid_ip, strlen(valid_ip), 0) != strlen(valid_ip))
					DieWithError("send() failed");
				strcpy(ip, valid_ip);

			}
			else {
				int num = atoi(temp1->num);
				num = num + 1;
				itoa(num, temp1->num, 10);
				if (send(clntSock, temp1->ip, strlen(temp1->ip), 0) != strlen(temp1->ip)) /* found the ip using gethostbyname function */
					DieWithError("send() failed");
				strcpy(ip, temp1->ip);
				//printf("%s", temp1->ip);
			}
			if (ip != NULL)
				/* Storing the contents into a logfile along with time stamp */
				fprintf(logfile, "%s  the ip adress for %s is  %s requested by %s \n", timestamp(), dns_searchDomainName, ip, inet_ntoa(echoClntAddr.sin_addr));
			fclose(logfile);
			break;
		case 2:   /* add_domain the domain name and ip address into the linked list  */
			token = strtok(NULL, " ");
			d = 0;
			while (token[d])
			{
				putchar(tolower(token[d])); // convert to lower case
				d++;
			}
			strcpy(dns_add, token);
			token = strtok(NULL, "\0");
			strcpy(ip_add, token);
			//node *par = NULL;
			temp1 = searchDomainName(&root, dns_add);
			if (temp1 != NULL) { printf("Already exists"); break; }
			else {
				add_domain(&root, dns_add, ip_add,"0");
			}
			/* Storing the contents into a logfile along with time stamp */
			fprintf(logfile, "%s %s  %s record added by client %s  \n", timestamp(), dns_add, ip_add, inet_ntoa(echoClntAddr.sin_addr));
			fclose(logfile);
			printf("\n \n");
			printf("Pre Order Display\n");
			print_preorder(root);
			break;
		case 3:/* deleting the record if it exists  */
			token = strtok(NULL, "\0");
			d = 0;
			while (token[d])
			{
				putchar(tolower(token[d]));
				d++;
			}
			strcpy(dns_del, token);
			printf("the DNS to be deleted %s \n", dns_del);
			//node *par = NULL;
			temp1 = searchDomainName(&root, dns_del);
			if (temp1 != NULL) {
				delete(&root, dns_del); 
				fprintf(logfile, "%s  the record deleted  is  %s requested by %s \n", timestamp(), dns_del, inet_ntoa(echoClntAddr.sin_addr));
				fclose(logfile); 
				printf("in Order Display\n");
				print_inorder(root);
			}
			else { printf("Record does not exist"); }
			break;
		
			}

		}

	}


// Error handling function, from book
void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}
