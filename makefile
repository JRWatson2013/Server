all:
	g++ client.cpp -o client -lcrypt
	g++ server.cpp -o server -lcrypt

clean:
	rm client
	rm server
