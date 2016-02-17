//JRWatson
//Jacob Watson
//Project 2
//Client.cpp
#define _XOPEN_SOURCE
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/time.h>
#include <crypt.h>
using namespace std;

//sendall routine taken from Beej's guide to network programming
int sendall(int s, char *buf, int *len)
{
  int total = 0;        // how many bytes we've sent
  int bytesleft = *len; // how many we have left to send
  int n;

  while(total < *len) {
    n = send(s, buf+total, bytesleft, 0);
    if (n == -1) { break; }
    total += n;
    bytesleft -= n;
  }

  *len = total; // return number actually sent here

  return n==-1?-1:0; // return -1 on failure, 0 on success
} 
 
int main(int argc, char *argv[]) {
  char cryptpass[256]; //encrypted password
  char password[256]; //users password
  int bytes; //bytes recieved
  int len; //length of message
  int hostset = 0; //check if host was entered
  int commset = 0; //check if command was entered
  char command[256]; //command to send to server
  char host[256]; //host address
  int port = 9001; //port on server
  int hcheck = 0; //bool for help
  int i = 1; //iterator for command line arguments
  unsigned long int inaddr;
  int sock; //socket to communicate with server
  struct sockaddr_in serv_addr;
  struct hostent *hp;
  char name[256]; //username
  char response[1024]; //response to first connection request to server
  while(i < (argc)) //cycle through the arguments and check for -X arguments and set the values as approprate
    {
      if(argv[i] != NULL)
	{
	  if(strcmp(argv[i], "-p") == 0) //user set port
	    {
	      i++;
	      if(argv[i] != NULL)
		{
		  if(atoi(argv[i]) == 0)
		    {
		      cout << "Bad port. \n.";
		      hcheck = 1;
		      break;
		    }
		  cout << "Using Port " << argv[i] << "\n";
		  port = atoi(argv[i]);
		} else
		{
		  cout << "Bad port. \n.";
		  hcheck = 1;
		  break;
		}
	    } else
	    {
	      if(strcmp(argv[i], "-c") == 0) //user set command
		{
		  i++;
		  if(argv[i] != NULL)
		    {
		      strcpy(command, argv[i]); //set command
		      commset = 1;
		    } else
		    {
		      cout << "Bad command. \n.";
		      hcheck = 1;
		      break;
		    }
		} else
		{
		  if(strcmp(argv[i], "-h") == 0) //user requests help
		    {
		      hcheck = 1;
		    } else
		    {
		      if(strcmp(argv[i], "-s") == 0) //user set host
			{
			  i++;
			  if(argv[i] != NULL)
			    {
			      strcpy(host, argv[i]); //copy in host
			      hostset = 1;
			    } else
			    {
			      cout << "Bad host. \n.";
			      hcheck = 1;
			      break;
			    }
			} else
			{
			  cout << "Bad input. \n";
			  hcheck = 1;
			  break; //Bad input, should go straight to help.
			}
		    }
		}
	    }
	} else
	{ 
	  break;
	}
      i++;
    }
  //if no command entered
 if(commset == 0)
   {
     hcheck = 1;
     cout << "A command must be entered. \n";
   }
 //if no host entered
 if(hostset == 0)
   {
     hcheck - 1;
     cout << "A host must be entered. \n";
   }

 if(argc < 3) //if not enough arguments were entered, no point in continuing
   {
     hcheck = 1;
   }

if(hcheck == 1) //display help
  {
    cout << "\n =======HELP====== \n distributed shell client\n usage: dsh [flags] {-c command}, where flags are:\n {-c command}    command to execute remotely\n {-s host}       host server is on\n [-p #]          port server is on (default is 9001)\n [-h]            this help message \n";
    exit(0);
  }

//set up the username string
 strcpy(name, "Jake")
;
 //set up the password string
 strcpy(password, "Drill");

//locate the host name
 bzero((void *) &serv_addr, sizeof(serv_addr)); //prepare serv_addr
 if ((hp = gethostbyname(host)) == NULL) { //get the host address
   perror("host name error"); //bad host name
   exit(1);
 }
 bcopy(hp->h_addr, (char *) &serv_addr.sin_addr, hp->h_length); //copy over the host address

 serv_addr.sin_family = AF_INET;
 serv_addr.sin_port = htons(port); //set port

 //set up the socket
 if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
   perror("creating socket");
   exit(1);
 }

 //try to connect to the server
 if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
   perror("can't connect");
   close(sock);
   exit(1);
 }

 //send username to server
 len = strlen(name);
 if(sendall(sock, name, &len) == -1)
   {
     perror("sendall");
     close(sock);
     exit(1);
   }

 //read in username confirmation
 if ((bytes = read(sock, response, 1024)) > 0) {
   response[bytes] = '\0'; /* do this just so we can print as string */
   // printf("received: %s \n", response);
 }
 //check for errors
 if(bytes == -1)
   {
     perror("error in read");
     close(sock);
     exit(1);
   }

 if (strcmp(response, "0") == 0) //User name did not match
   {
     cout << "Username not accepted. \n";
     close(sock);
     exit(0);
   }

 //encrypt password
 strcpy(cryptpass, crypt(password, response));  

 //cout << cryptpass << ": cryptpass \n";

 //send encrypted password
 len = strlen(cryptpass);
 if(sendall(sock, cryptpass, &len) == -1)
   {
     perror("sendall");
     close(sock);
     exit(1);
   }

 //read in password confirmation
 if ((bytes = read(sock, response, 1024)) > 0) {
   response[bytes] = '\0';
   // printf("received: %s \n", response);
 }
 //if there was an error
 if(bytes == -1)
   {
     perror("error in read");
     close(sock);
     exit(1);
   }

 //if the password was good, send the command. If not, exit.
 if(strcmp(response, "1") == 0)
   {
     len = strlen(command);
     if(sendall(sock, command, &len) == -1)
       {
	 perror("sendall");
	 close(sock);
	 exit(1);
       }
   } else
   {
     cout << "Password not accepted. \n";
     close(sock);
     exit(0);
   }

 //print out the results
 while ((bytes = read(sock, response, 1024)) > 0) {
   response[bytes] = '\0'; /* do this just so we can print as string */
   printf("%s \n", response);
 }
 //if there was an error
 if (bytes == -1)
   {
     perror("error in read");
     close(sock);
     exit(1);
   }

 //all done, exit socket
 close(sock);
}
