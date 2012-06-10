/*
 * DCCP client.
 */

#include <sys/types.h>
#include <sys/socket.h>

int
main(int argc, char *argv[])
{
	struct sockaddr_in sin;
	int sock;
	int optval;
	int error;
	socklen_t optlen;

	/* IPPROTO_DCCP 33 */
	sock = socket(2, 2, 33);
	if (sock == -1) {
		perror("socket");
		exit(1);
	}

	bzero(&sin, sizeof (sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(37000);
	sin.sin_addr.s_addr = INADDR_ANY;

	error = connect(sock, (struct sockaddr *)&sin, sizeof (sin));
	if (error == -1) {
		perror("connect");
		//exit(1);
	}

	
	return (0);
}
