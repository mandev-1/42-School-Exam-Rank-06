#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h> // pridame sys/select
#include <sys/select.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct s_client
{
    int     id;
    char    msg[100000];
}       t_client;

//globalky pro clienty, fds, nejaky int helpery a buffery
t_client    clients[2048];
fd_set      write_set, read_set, current;
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

//send to all funkce bude checkovat jestli ten fd je v setu, a posle pres send vsem clientum 
// zaroven pouzijeme ten error ktery se stane kdyz send failne
void    send_to_all(int except)
{
    for (int fd = 0; fd <= maxfd; fd++)
    {
        if (FD_ISSET(fd, &write_set) && fd != except)
            if (send(fd, send_buffer, strlen(send_buffer), 0) == -1)
                err (NULL);
    }
}
//loopuju abych poslal VSEM clientum (vsechny fd az po registrovany maximum)
//zjistim, ze doopravdy ten FD je ve write_setu, a ze to neni vyjimka, ktere neposilam
//poslu, a zabalim to do ifu, ktery mi podchyti fatal errror 


