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