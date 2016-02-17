//JRWatson
//Jacob Watson
//Project 2
//Server.cpp
#define _XOPEN_SOURCE
#include <crypt.h>
#include <signal.h>
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

int main(int argc, char *argv[])
{
  char password[256]; //holds users expected password
  char passcrypt[256]; //holds users encrypted password
  int randomnum; //holds the randomly generated key
  char randomstr[256]; //holds the randomly generated key in string form
  int pid; //process id
  int hostport = 9001; //port to host on
  int sock; //socket
  int hcheck = 0; //help bool
  struct sockaddr_in cli_addr, serv_addr; //structures to manage the socket
  int i = 1; //iterator
  int status; //status indicator for child
  int bytes; //incoming bytes count
  char message[256]; //incoming message from socket
  char response[256]; //response
  int newsock; //socket that incoming messages are directed to
  int error; //used to tell if the child process should quit due to an error
  socklen_t clilen;
  char dir[256]; //holds home directory
 
  getcwd(dir, 256); //set home directory
  if(dir == NULL)
    {
      //couldn't get working directory
      perror("getcwd");
      exit(1);
    }

  while(i < (argc)){ //cycle through the arguments and check for -X arguments and set the values as approprate

    if(argv[i] != NULL)  
      {
	if(strcmp(argv[i], "-p") == 0) //user set port
	  {
	    i++;
	    if(argv[i] != NULL)
	      {
		if(atoi(argv[i]) == 0)
		  {
            	    cout << "Bad input. \n.";
		    hcheck = 1;
		    break;
		  }
		cout << "Using Port " << argv[i] << "\n";
		hostport = atoi(argv[i]);
	      } else
	      {
		cout << "Bad input. \n.";
		hcheck = 1;
		break;
	      }
	  } else
	  {
	    if(strcmp(argv[i], "-h") == 0) //user requests help
	      {
		hcheck = 1;
		//   cout << "HCHECK \n";
	      } else
	      {
		if(strcmp(argv[i], "-d") == 0) //user set dir
		  {
		    i++;
		    if(argv[i] != NULL)
		      {
			cout << "Using Directory " << argv[i] << "\n";
			strcpy(dir, argv[i]); //copy in new directory
		      }
		    else
		      {
			cout << "Bad input. \n.";
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
      } else
      {
	break; //end while
      }
    i++;
  }

  if(hcheck == 1) //display help
    {
      cout << "\n =======HELP====== \n usage: server [flags], where flags are: \n -p #    port to serve on (default is 4513) \n -d dir  directory to serve out of (default is working directory) \n -h      this help message\n";
      exit(0);
    }

  //set working directory
  if(chdir(dir) != 0)
    {
      perror("chdir");
      exit(1);
    }

  cout << "Server activating. \n";
  cout << "Port: " << hostport << "\n";
  cout << "Directory: " << dir << "\n";

  srand(time(NULL)); //set random seed
  strcpy(password, "Drill"); //set password

  /* create socket from which to read */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("creating socket");
    exit(1);
  }

  /* bind our local address so client can connect to us */
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(hostport);
  if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    perror("can't bind to local address");
    close(sock);
    exit(1);
  }

  /* mark socket as passive, with backlog num */
  if (listen(sock, 5) == -1) {
    perror("listen");
    close(sock);
    exit(1);
  }

  cout << "Socket created! Accepting connections.\n";
  while(1)
    {
  /* wait here (block) for connection */
  clilen = sizeof(cli_addr);
  newsock = accept(sock, (struct sockaddr *) &cli_addr, &clilen);
  if (newsock < 0) {
    perror("accepting connection");
    close(sock);
    close(newsock);
    exit(1);
  }
  cout << "\n";
  error = 0; //default error check
  pid = fork(); //fork to child
  if(pid == -1) //check for error
    {
      perror("Couldn't fork child: ");
    }
  //child section
  if(pid == 0)
    {
      close(sock); //close parent socket
      cout << "forked child \n";

      //read in name
  if ((bytes = read(newsock, message, 1024)) > 0) {
    message[bytes] = '\0';
    printf("received: %s \n", message);
  }
  if (bytes == -1)
    {
    perror("error in read");
    close(newsock);
    exit(1);
    }
  //check if name matches correct name
  if(strcmp("Jake", message) == 0 ){
    randomnum = rand();
    sprintf(randomstr, "%d",randomnum);
       strcpy(response, randomstr);
     } else
	 {
	   error = 1;
	   cout << "Bad name. \n";
	   strcpy(response, "0");
	 }
  //send back the proper resonse, random num if correct, 0 if not
  int len;
  len = strlen(response);
  if(sendall(newsock, response, &len) == -1)
    {
      perror("sendall");
      close(newsock);
      exit(1);
    } 
  if(error == 1)
    {
      close(newsock);
      exit(1);
    }

  strcpy(message, " "); //clear out message for the next incoming string

  //read in the password
  if ((bytes = read(newsock, message, 1024)) > 0) {
    message[bytes] = '\0';
    // printf("received: %s \n", message);
  }
  if (bytes == -1)
    {
    perror("error in read");
    close(newsock);
    exit(1);
    }

  strcpy(passcrypt, crypt(password, randomstr)); //generate the encrypted password to compare
  //cout << passcrypt << ": passcrypt \n";
  if(strcmp(passcrypt, message) == 0) //check if the password is correct, if so send 1, else 0
    {
      cout << "Password ok \n";
      strcpy(response, "1");
    } else
    {
      cout << "Password doesnt match. \n";
      strcpy(response, "0");
      error = 1;
    }
  //send response to password
  len = strlen(response);
  if(sendall(newsock, response, &len) == -1)
    {
      perror("sendall");
      close(newsock);
      exit(1);
    }
  if(error == 1)
    {
      close(newsock);
      exit(1);
    }

  //read in command
  if ((bytes = read(newsock, message, 1024)) > 0) {
    message[bytes] = '\0'; 
    printf("command: %s \n", message);
  }
  if (bytes == -1)
    {
      perror("error in read");
      close(newsock);
      exit(1);
    }

  cout << "executing command... \n";

  //prepare the args array to hold the command arguments and command
      char* args[11];
      char inp[256]; //input 
      args[0] = new char[256];
      args[1] = new char[256];
      args[2] = new char[256];
      args[3] = new char[256];
      args[4] = new char[256];
      args[5] = new char[256];
      args[6] = new char[256];
      args[7] = new char[256];
      args[8] = new char[256];
      args[9] = new char[256];
      args[10] = new char[256];
      args[11] = NULL;

      char *token; //used to iterate through the message string and pull out arguments 
      int k = 1; //iterator
      int j; //iterator
      int l = 0;//iterator
      
      //default all values of args
      for(j = 0; j < 11; j++)
	{
	  strcpy(args[j], " ");
	}
      //put message into inp for processing
      strcpy(inp, message);
      //use strtok to tokenize the command
      token = strtok(inp, " ");
      //get the command
      strcpy(args[0], token);
      //go through the remainder of the message pulling out arguments
      while(token != NULL)
	{
	  // cout << "Its running. \n";
	  token = strtok(NULL, " ");
	  if(token != NULL)
	    {
	      // cout << token << " token \n";
	  strcpy(args[k], token);
	  k++;
	    }
	}
      // cout << "Args set \n";
      char* finalarg[k]; //transfer the contents of args to the final argument array
      for(l = 0; l < k; l++)
	{
	  finalarg[l] = new char[256];
	  strcpy(finalarg[l], args[l]);
	}
      //set the last value of finalarg to NULL
      finalarg[k] = NULL;
      //set stdout to go to the client
      if(dup2(newsock, 1) < 0)
	{
	  perror("dup2:stdout");
	  close(newsock);
	  exit(1);
	}
      //set stderr to go to the client
      if(dup2(newsock, 2) < 0)
        {
          perror("dup2:stderr");
          close(newsock);
          exit(1);
        }
      int exereturn = 0; //error check for execl
      //run execvp
      exereturn = execvp(finalarg[0], finalarg);
     if (exereturn == -1)
       {
	 perror("execvp error");
	 close(newsock);
	 exit(1);
       }
    } else
    {
  //parent section
      // cout << "parent process \n";
      close(newsock); //close the child socket
      // cout << "closed newsock \n";
      status = 1;
      waitpid(pid, &status, 0); //wait for the process to finish
    }
    }
  close(sock); //close the parent socket
}
