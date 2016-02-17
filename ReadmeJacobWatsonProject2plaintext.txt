Project 2 Readme
Jacob Watson
JRWatson@wpi.edu
How to compile:
Type make
OR
g++ client.cpp -o client -lcrypt
g++ server.cpp -o server -lcrypt
How to run:
BOTH THE CLIENT AND SERVER USE THE DEFAULT PORT 9001
THE DEFAULT DIRECTORY FOR THE SERVER IS THE CURRENT WORKING DIRECTORY
To run the server:
./server
To run the server's help program:
./server -h
There are two options for the server, changing the port and directory
To change the port, use the argument -p #, where # is the port number to use
To change the directory, use the argument -d dir, where dir is the directory to use
To run the client:
./client -c command -s host, where command is the command to run and host is the host server
To run the client's help program:
./client -h
To run the client on a different port, use the argument -p #, where # is the port number to use
DEFAULT INFO:
The username for the client/server program is "Jake".
The password for the client/server program is "Drill".
System dependencies:
The programs were completely coded in the CCC machines and are completly compatible with them. They also run natively on the amazon ubuntu servers. If the programs are compiled manually, they must be compiled with the -lcrypt option. 

Examples:
RUNNING LS
SERVER:
-bash-3.2$ ./server
Server activating.
Port: 9001
Directory: /isihome/jrwatson/ser
Socket created! Accepting connections.

forked child
received: Jake
Password ok
command: ls
executing command...

CLIENT:
./client -s CCCWORK4.WPI.EDU -c "ls" -p 9001
Using Port 9001

 ~
client
client.cpp
client.cpp~
makefile
output
server
server.cpp
server.cpp~
test
Making a directory, LS to see that directory
SERVER:
-bash-3.2$ ./server
Server activating.
Port: 9001
Directory: /isihome/jrwatson/ser
Socket created! Accepting connections.

forked child
received: Jake
Password ok
command: mkdir testdir
executing command...

forked child
received: Jake
Password ok
command: ls
executing command...
CLIENT:
bash-3.2$ ./client -s CCCWORK4.WPI.EDU -c "mkdir testdir" -p 9001
Using Port 9001

-bash-3.2$ ./client -s CCCWORK4.WPI.EDU -c "ls" -p 9001 
Using Port 9001

 ~
client
client.cpp
client.cpp~
makefile
output
server
server.cpp
server.cpp~
test
testdir

CAT ON A FILE
SERVER:
-bash-3.2$ ./server
Server activating.
Port: 9001
Directory: /isihome/jrwatson/ser
Socket created! Accepting connections.

forked child
received: Jake
Password ok
command: cat catfile
executing command...

CLIENT:
./client -s CCCWORK4.WPI.EDU -c "cat catfile"

This is a file that was created to be read out to the client from the server using cat.
