#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while(0);

int main(int argc, char **argv)
{
	int s_fd;
	int c_fd;
	struct sockaddr_in s_addr;
	struct sockaddr_in c_addr;


	// 1. init socket
	// int socket(int domiain, int type, int protocol);
	s_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(s_fd == -1)
		handle_error("socket");

	// 2. blind socket
	// int bind(int sockfd, const struct sockaddr &addr, socklen_t addrlen)
	s_addr.sin_family = AF_INET;
	//s_addr.sin_port = htons(atoi(argv[1]));
	//s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_aton(argv[1], &s_addr.sin_addr);
	inet_aton("127.0.0.1", &s_addr.sin_addr);
	int isBind = bind(s_fd, (struct sockaddr *) &s_addr, sizeof(s_addr));

	// 3. listen
	listen(s_fd, 10);

	// 4. accept
	unsigned int clen = sizeof(struct sockaddr_in);

	char *readBuf = (char *)malloc(128);
	char *msg = (char *) malloc(128);
	int n_read;

	while(1)
	{
		printf("wait for new connection: \n");
		c_fd = accept(s_fd, (struct sockaddr *)&c_addr, &clen);
		if(c_fd == -1)
			handle_error("accept");
		printf("Accept connection: %s\n", inet_ntoa(c_addr.sin_addr));

		if(fork() == 0)
		{
			if(fork() == 0)
			{
				// 5. read
				while(1)
				{
					memset(readBuf, '\0', 128);
					n_read = read(c_fd, readBuf, 128);
					if(n_read == -1) {
						handle_error("read");
					} else {
						printf("ReadBuf: %s\n", readBuf);
					}
				}
			}
			else
			{
				// 6. write
				while(1)
				{
					memset(msg, '\0', 128);
					scanf("%s", msg);
					write(c_fd, msg, strlen(msg));
				}
			}
		}

		printf("Connection complete.\n");
	}

	close(s_fd);
	close(c_fd);
	return 0;
}
