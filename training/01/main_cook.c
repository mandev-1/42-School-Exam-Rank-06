#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct s_client
{
    int     id;
    char    msg[100000];
}       t_client;

t_client    clients[2048];
fd_set      read_set, write_set, current;
int         maxfd = 0, gid = 0;
char        send_buffer[120000], recv_buffer[120000];

void    err(char *msg)
{
    if (msg)
        write (2, msg, strlen(msg));
    else
        write (2, "Fatal error", 11);
    write (2, "\n", 1);
    exit (1);
}

void    send_to_all(int except)
{
    for (int fd = 0; fd <= maxfd; fd++)
    {
        if (FD_ISSET(fd, &write_set) && fd != except)
            if (send(fd, send_buffer, strlen(send_buffer), 0) == -1)
                err(NULL);
    }
}