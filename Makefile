all:
	gcc client.c list.c -o client -lpthread
	gcc server.c list.c -o server -lpthread

clean:
	rm client server
