/*
 * DCCP server.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>

int
main(int argc, char *argv[])
{
	struct sockaddr_in sin;
	struct sockaddr_in remote;
	int sock;
	int optval;
	int error;
	socklen_t optlen;
	socklen_t remotelen;
	ssize_t recv_len;
	char *recv_buf[128];

	/* IPPROTO_DCCP 33 */
	sock = socket(2, 6, 33);
	if (sock == -1) {
		perror("socket");
		exit(1);
	}

	optval = B_TRUE;
	optlen = sizeof (optval);
	error = setsockopt(sock, SOL_SOCKET, SO_DEBUG, &optval,
	    optlen);
	if (error == -1) {
		perror("setsockopt");
		exit(1);
	}

	optlen = sizeof (optval);
	error = getsockopt(sock, SOL_SOCKET, SO_DEBUG, &optval,
	    &optlen);
	if (error == -1) {
		perror("getsockopt");
		exit(1);
	}

	printf("option value: %d\n", optval);

	bzero(&sin, sizeof (sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(3700);
	sin.sin_addr.s_addr = INADDR_ANY;

	error = bind(sock, (struct sockaddr *)&sin, sizeof (sin));
	if (error == -1) {
		perror("bind");
		exit(1);
	}

	optlen = sizeof (optval);
	error = getsockopt(sock, SOL_SOCKET, SO_DEBUG, &optval,
	    &optlen);
	if (error == -1) {
		perror("getsockopt");
		exit(1);
	}

	error = listen(sock, 100);
	if (error == -1) {
		perror("listen");
		exit(1);
	}

	bzero(&sin, sizeof (sin));
	optlen = sizeof (sin);
	error = getsockname(sock, (struct sockaddr *)&sin, &optlen);
	if (error == -1) {
		perror("getsockname");
	}

	printf("port number %d\n", ntohs(sin.sin_port));

	bzero(&remote, sizeof (remote));
	remotelen = sizeof (remote);
	error = accept(sock, (struct sockaddr *)&remote, &remotelen);
	if (error == -1) {
		perror("accept");
	}

	recv_len = recv(sock, &recv_buf, 128, 0);
	if (recv_len == -1) {
		perror("recv");
		exit(1);
	}

	printf("received %d bytes", recv_len);

	return (0);
}
