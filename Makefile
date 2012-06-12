
all:
	cc -o server server.c -lsocket -lnsl -lxnet
	cc -o client client.c -lsocket -lnsl -lxnet

clean:
	rm -f *.o server client
