
all:
	cc -o server server.c -lsocket -lnsl -lxnet

clean:
	rm -f *.o server
