#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

// struktura t_client
typedef struct s_client
{
    int id;
    char    msg[100000];
}   t_client;

//globalky pro clienty, sockety, inty a chary(pro send a rcv buffery)
t_client    clients[2048];
fd_set      read_set, write_set, current;
int			maxfd = 0, gid = 0;
char		send_buffer[120000], recv_buffer[120000];

void	err(char *msg)
{
	if (msg)
		write (2, msg, strlen(msg));
	else
		write (2, "Fatal error", 11);
	write (2, "\n", 1);
	exit(1);
}


//sending a message to all the clients, we will use the just implemented err function to throw fatal error if send() fails 
void	send_to_all(int except)
{
	for (int fd = 0; fd <= maxfd; fd++)
	{
		if (FD_ISSET(fd, &write_set) && fd != except)
			if (send(fd, send_buffer, strlen(send_buffer), 0) == -1)
				err(NULL);
	}
}

int	main(int argc, char **argv)
{
	// sanitize input - only 2 params allowed - error text in subject
	if (argc != 2)
		err("Wrong number of arguments");

	// lets setup the server socket
	struct sockaddr_in	serveraddr;
	socklen_t len = sizeof(struct sockaddr_in)

	
}