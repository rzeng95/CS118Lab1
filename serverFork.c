/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
*/
#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <strings.h>
#include <sys/wait.h>	/* for the waitpid() system call */
#include <signal.h>	/* signal name macros, and the kill() prototype */


void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

void dostuff(int); /* function prototype */
void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;
     struct sigaction sa;          // for signal SIGCHLD

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     
     listen(sockfd,5);
     
     clilen = sizeof(cli_addr);
     
     /****** Kill Zombie Processes ******/
     sa.sa_handler = sigchld_handler; // reap all dead processes
     sigemptyset(&sa.sa_mask);
     sa.sa_flags = SA_RESTART;
     if (sigaction(SIGCHLD, &sa, NULL) == -1) {
         perror("sigaction");
         exit(1);
     }
     /*********************************/
     
     while (1) {
         newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
         
         if (newsockfd < 0) 
             error("ERROR on accept");
         
         pid = fork(); //create a new process
         if (pid < 0)
             error("ERROR on fork");
         
         if (pid == 0)  { // fork() returns a value of 0 to the child process
             close(sockfd);
             dostuff(newsockfd);
             exit(0);
         }
         else //returns the process ID of the child process to the parent
             close(newsockfd); // parent doesn't need this 
     } /* end of while */
     return 0; /* we never get here */
}

/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void dostuff (int sock)
{
   int n;
   char buffer[256];
      
   bzero(buffer,256);
   n = read(sock,buffer,255);
//***********************
//char* filename = "index.html";

char filename[256];
bzero(filename,256);

// The message (contained inside "buffer") is GET /index.html HTTP/1.1 ......
// So we want to parse out "index.html"

char* begin = strstr(buffer, "GET /");
//puts(begin);
if (begin == buffer) {
	// We found GET /, hence start from the index=5 (the "i" in index) which is begin+5

	// Now find where the rest of the string (starting from HTTP/1.1) occurs
	char* end = strstr(buffer+5, " HTTP/1.1");
	// Subtract that index location from the beginning of the file name to give us the actual name of the file
	int filelength = end - (begin+5);
	//printf("%d\n",filelength);
	strncpy(filename, begin+5, filelength);
	//puts(filename);
	
	// Now we have the name of our file which we will open using fopen and fread
}
else {
	puts("error could not find");
}


// Open the file using the filename we parsed earlier
FILE* myFile = fopen(filename, "r");

// Get size of file (http://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c)
fseek(myFile, 0L, SEEK_END);
int sz = ftell(myFile);
fseek(myFile, 0L, SEEK_SET);

// Read the file's contents and put into myBuf 
char myBuf[sz+1];
fread(myBuf,1,sz,myFile);

write(sock,myBuf,sz);

puts("\n\n***BEGINNING OF MESSAGE***\n\n");

printf("Name of file : %s\n",filename);
printf("Size of file : %d\n",sz);
printf("Contents of file: %s\n",myBuf);

puts("\n\n***END OF MESSAGE***\n\n");

//***********************
   if (n < 0) error("ERROR reading from socket");
   printf("Here is the message: %s\n",buffer);
   //n = write(sock,"I got your message",18);
   if (n < 0) error("ERROR writing to socket");
}
