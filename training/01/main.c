// implement the incldues

#include <stdio.h>      // For input/output functions like printf
#include <stdlib.h>     // For memory allocation routines and exit()
#include <unistd.h>     // For miscellaneous functions such as close()
#include <string.h>     // For string manipulation functions like strcpy and strlen
#include <sys/socket.h> // For socket creation, binding, and other network functions
#include <netinet/in.h> // For structs and functions to handle internet addresses

// implement the key struct

typedef struct s_client
{
    int     id;
    char    message[100000];

}  t_client;


// define the global variables
t_client    client[2048];
fd_set      read_set, write_set, current;
int         maxfd = 0, gid = 0;
char        send_buffer[120000], recv_buffer[120000];

// error handling -- prints error message and exits
void    err(char *msg)
{
    if (msg)
        write(2, msg, strlen(msg));
    else
        write(2, "Fatal error", 11);
    write(1, "\n", 1);
    exit(1);
}


// send message

// main 