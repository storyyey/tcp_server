all:
	gcc client.c -o client -lpthread
	gcc server.c -o server -lpthread

clean:
	rm client server
