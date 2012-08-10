
all:
	cc -o server server.c -lsocket -lnsl -lxnet
	cc -o client client.c -lsocket -lnsl -lxnet
	cc -o dccp_test dccp_test.c -lsocket

clean:
	rm -f *.o server client
