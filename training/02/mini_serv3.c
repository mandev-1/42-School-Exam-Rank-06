#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct s_cliet
{
    int id;
    char msg[100000];
}   t_client;

t_client clients[2048];
fd_set  read_set, write_set, current;
int     maxfd = 0, gid = 0;
char    send_buffer[120000], recv_buffer[120000];

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
                err (NULL);
    }
}

int main(int ac, char **av)
{
    if (ac != 2)
        err("Wrong number of arguments");

    struct sockaddr_in  serveraddr;
    socklen_t len = sizeof(struct sockaddr_in);
    int serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1) err(NULL);
    maxfd = serverfd;

    FD_ZERO(&current);
    FD_SET(serverfd, &current);
    bzero(clients, sizeof(clients));
    bzero(&serveraddr, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(atoi(av[1]));

    if (bind(serverfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1 || listen(serverfd, 100) == -1)
        err(NULL);

    while (1)
    {
        read_set = write_set = current;
        if (select(maxfd + 1, &read_set, &write_set, 0, 0) == -1) continue;

        for (int fd = 0; fd <= maxfd; fd++)
        {
            if (FD_ISSET(fd, &read_set))
            {

            }
        }
    }
}