/*
 * DCCP client.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/*
 * Connects to a DCCP server given on the cmd line.
 */

int
main(int argc, char *argv[])
{
	struct sockaddr_in sin;
	struct hostent	*hp;
	in_addr_t	addr;
	socklen_t	optlen;
	int		sock;
	int		optval;
	int		error;

	if (argc != 2) {
		(void) fprintf(stderr, "usage: %s <server-name>\n",
		    argv[0]);
		exit(1);
	}

	hp = gethostbyname(argv[1]);
	if (hp == NULL) {
		(void) fprintf(stderr, "gethostbyname failed.\n");
		exit(1);
	}

	/* IPPROTO_DCCP 33 */
	sock = socket(2, 6, 33);
	if (sock == -1) {
		perror("socket");
		exit(1);
	}

	bzero(&sin, sizeof (sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(37000);
	//memcpy(&(sin.sin_addr.s_addr), hp->h_addr, hp->h_length);
	inet_pton(AF_INET, "192.168.2.20", &(sin.sin_addr));

	error = connect(sock, (struct sockaddr *)&sin, sizeof (sin));
	if (error == -1) {
		perror("connect");
		exit(1);
	}

	return (0);
}
