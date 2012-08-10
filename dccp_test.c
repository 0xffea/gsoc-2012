/*
 * This is take from the DCCP conformace test suite
 *
 * Server is called this way: dccp_test -H DCCP_NUT_NET0_ADDR
 * -P DCCP_NUT0_PORT -c code -l -m count -s size
 */

//
// DCCP protocol Conformance Test Suite
//
// Wei Yongjun <yjwei@cn.fujitsu.com>
//
// THIS PROGRAM IS FREE SOFTWARE; YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT 
// UNDER THE TERMS OF THE GNU GENERAL PUBLIC LICENSE AS PUBLISHED BY THE 
// FREE SOFTWARE FOUNDATION; EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR
// OPTION) ANY LATER VERSION.
//
// $Name: REL_1_0_0 $
//
// $Id: dccp_test.c,v 1.2 2008/07/07 06:54:23 WeiYJ Exp $
//

#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <netdb.h>

#define SOCK_DCCP	6
#define SOL_DCCP	269
#define IPPROTO_DCCP	33

#define DCCP_SOCKOPT_PACKET_SIZE	1 /* XXX deprecated, without effect */
#define DCCP_SOCKOPT_SERVICE		2
#define DCCP_SOCKOPT_CHANGE_L		3
#define DCCP_SOCKOPT_CHANGE_R		4
#define DCCP_SOCKOPT_GET_CUR_MPS	5
#define DCCP_SOCKOPT_SERVER_TIMEWAIT	6
#define DCCP_SOCKOPT_SEND_CSCOV		10
#define DCCP_SOCKOPT_RECV_CSCOV		11
#define DCCP_SOCKOPT_CCID_RX_INFO	128
#define DCCP_SOCKOPT_CCID_TX_INFO	192

#define SPORT 		5000
#define BACKLOG		50

FILE * fp = NULL;
int backlog = BACKLOG;
int echo = 0;
int size = 1024;
int count = -1;
int delay = 0;
int debug = 0;
int local_port = SPORT, remote_port = SPORT;
char *local_host = NULL, *remote_host = NULL;
struct sockaddr_in addr4;
struct sockaddr_in6 addr6;
struct sockaddr *saddr, *caddr;
socklen_t sockaddr_size;

void usage(void);
void server(int sk);
void client(int sk);

int main(int argc,char **argv) 
{
	extern char *optarg;
	int ch;
	
	long temp, code = 100;
	int sockfd, family = AF_INET, role = 0, error;
	struct addrinfo *res;
	char *logfile = NULL;

	while((ch = getopt(argc, argv, "H:P:h:p:w:c:m:s:b:d:elv")) != -1) {
		switch(ch) {
		case 'H':
			local_host = optarg;
			break;
		case 'P':
			errno = 0;
			temp = (long)strtol(optarg, (char **)NULL, 10);
			if(errno || temp > 65535 || temp <= 0) {
				fprintf(stderr, "Wrong port: %s.\n", optarg);
				exit(1);
			}
			local_port = temp;
			break;
		case 'h':
			remote_host = optarg;
			break;
		case 'p':
			errno = 0;
			temp = (long)strtol(optarg, (char **)NULL, 10);
			if(errno || temp > 65535 || temp <= 0) {
				fprintf(stderr, "Wrong port: %s.\n", optarg);
				exit(1);
			}
			remote_port = temp;
			break;
		case 'c':
			errno = 0;
			temp = (long)strtol(optarg, (char **)NULL, 10);
			if(errno || temp <= 0) {
				fprintf(stderr, "Wrong service code: %s.\n", optarg);
				exit(1);
			}
			code = temp;
			break;
		case 'm':
			errno = 0;
			count = (long)strtol(optarg, (char **)NULL, 10);
			if(errno || count > 2147483647 || count < 0) {
				fprintf(stderr, "Wrong max packet number: %s.\n", optarg);
				exit(1);
			}
			break;
		case 's':
			errno = 0;
			size = (long)strtol(optarg, (char **)NULL, 10);
			if(errno || size > 2147483647 || size <= 0) {
				fprintf(stderr, "Wrong  max packet size: %s.\n", optarg);
				exit(1);
			}
			break;
		case 'b':
			errno = 0;
			temp = (long)strtol(optarg, (char **)NULL, 10);
			if(errno || temp > 65535 || temp < 0) {
				fprintf(stderr, "Wrong backlog: %s.\n", optarg);
				exit(1);
			}
			backlog = temp;
			break;
		case 'd':
			errno = 0;
			temp = (long)strtol(optarg, (char **)NULL, 10);
			if(errno || temp > 65535 || temp <= 0) {
				fprintf(stderr, "Wrong delay time: %s.\n", optarg);
				exit(1);
			}
			delay = temp;
			break;
		case 'w':
			logfile = optarg;
			break;
		case 'e':
			echo = 1;
			break;
		case 'l':
			role = 1;
			break;
		case 'v':
			debug++;
			break;
		case '?':
		default:
			usage();
		}
	}

	if (local_host == NULL) {
		fprintf(stderr, "You must set local ip address for server\n");
		exit(1);
	}

	if (!role && remote_host == NULL) {
		fprintf(stderr, "You must set remote ip address for client\n");
		exit(1);
	}

	error = getaddrinfo(local_host, 0, NULL, &res);
	if (error) {
		printf("%s\n", gai_strerror(error));
		usage();
	}

	if (res->ai_family != AF_INET && res->ai_family != AF_INET6) {
		fprintf(stderr, "Wrong address family: %d\n", res->ai_family);
		exit(1);
	}

	if (logfile != NULL) {
		 fp = fopen(logfile, "w");
	}
	
	if (fp == NULL) {
		fp = stdout;
	}

	((struct sockaddr_in *)(res->ai_addr))->sin_port = htons(local_port);

	fprintf(fp, "socket()\n");
	if((sockfd = socket(res->ai_family, SOCK_DCCP, IPPROTO_DCCP)) == -1) {
		perror("socket");
		exit(1);
	}

	code = htonl(code);
	setsockopt(sockfd, SOL_DCCP, DCCP_SOCKOPT_SERVICE, &code, sizeof(code));
	
	fprintf(fp, "bind(sk=%d)\n", sockfd);
	if(bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		perror("bind");
		exit(1);
	}

	freeaddrinfo(res);

	if (role) {
		server(sockfd);
	} else {
		client(sockfd);
	}

	fflush(fp);
	if (fp != stdout)
		fclose(fp);

	return 0;
}

void usage() 
{ 
	fprintf(stderr,
"Usage: dccp_test -H local_host -P local_port [-h remote_host] [-p remote_port] [-m segs] [-s pktlens]]\n"
"                 [-h]\n");
	exit(1);
}


void server(int sockfd) {
	char *buffer = NULL;
	int newfd, used = 0, cnt;

	fprintf(fp, "listen(sk=%d, backlog=%d)\n", sockfd, backlog);
	if(listen(sockfd, backlog) == -1) {
		perror("listen");
		exit(1);
	}

	if((buffer = (char *)malloc(size)) == NULL) {
		perror("malloc");
		exit(1);
	}

	fprintf(fp, "accept(sk=%d, ...)\n", sockfd);
	fflush(fp);
	if((newfd = accept(sockfd, caddr, &sockaddr_size)) == -1) {
		perror("accept");
		exit(1);
	}

	while(count == -1 || used < count) {
		fprintf(fp, "recv(sk=%d, size=%d) %d/%d\n", newfd, size, used + 1, count);
		fflush(fp);
		if(recv(newfd, buffer, size, 0) <= 0) {
			perror("recv");
			break;
		}

		if (echo) {
			fprintf(fp, "send(sk=%d, size=%d", newfd, size);
			fflush(fp);
			cnt = send(newfd, buffer, size, 0);
			if (cnt < 0) {
				fprintf(fp, ") ... fail\n");
				fflush(fp);
				perror("send"); 
				break;
			}
			fprintf(fp, "/%d) %d/%d\n", cnt, used + 1, count);
			fflush(fp);
		}
		used++;
	}

	if (delay) {
		fprintf(fp, "sleep(%d)\n", delay);
		fflush(fp);
		sleep(delay);
	}

	fprintf(fp, "close(sk=%d)\n", newfd);
	close(newfd);

	fprintf(fp, "close(sk=%d)\n", sockfd);
	close(sockfd);
	free(buffer);
}

void client(int sockfd) {
	struct addrinfo *res;
	char *buffer = NULL;
	int error, used = 0, cnt;

	error = getaddrinfo(remote_host, 0, NULL, &res);
	if (error) {
		fprintf(fp, "%s\n", gai_strerror(error));
		usage();
	}

	if (res->ai_family != AF_INET && res->ai_family != AF_INET6) {
		fprintf(stderr, "Wrong address family: %d\n", res->ai_family);
		exit(1);
	}

	((struct sockaddr_in *)(res->ai_addr))->sin_port = htons(remote_port);

	fprintf(fp, "connect(sk=%d)\n", sockfd);
	fflush(fp);
	if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) { 
		perror("connect"); 
		exit(1); 
	} 
	
	freeaddrinfo(res);

	if((buffer = (char *)malloc(size)) == NULL) {
		perror("malloc");
		exit(1);
	}

	while(count == -1 || used < count) {
		// send the date ceaselessly if count == -1
		bzero(buffer, size);
		fprintf(fp, "send(sk=%d, size=%d) %d/%d\n", sockfd, size, used + 1, count);
		fflush(fp);
		if (send(sockfd, buffer, size, 0) == -1) {
			perror("send"); 
			exit(0); 
		}

		if (echo) {
			fprintf(fp, "recv(sk=%d, size=%d", sockfd, size);
			fflush(fp);
			cnt = recv(sockfd, buffer, size, 0);
			if (cnt == -1) {
				fprintf(fp, ") ... fail\n");
				fflush(fp);
				perror("send"); 
				break;
			}
			fprintf(fp, "/%d) %d/%d\n", cnt, used + 1, count);
			fflush(fp);
		}

		used++;
	}
	
	if (delay) {
		fprintf(fp, "sleep(%d)\n", delay);
		fflush(fp);
		sleep(delay);
	}

	fprintf(fp, "close(sk=%d)\n", sockfd);
	close(sockfd);
}

