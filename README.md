# MINI SERVER IN C -- the LAST CORE EXAM in 42 PRAGUE

Here's the code with added comments explaining the purpose of each section:

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Define a structure to store client information
typedef struct s_client
{
    int     id;
    char    msg[100000];
}   t_client;

// Global variables for managing clients and file descriptors
t_client    clients[2048];
fd_set      read_set, write_set, current;
int         maxfd = 0, gid = 0;
char        send_buffer[120000], recv_buffer[120000];

// Error handling function
void    err(char  *msg)
{
    if (msg)
        write(2, msg, strlen(msg));
    else
        write(2, "Fatal error", 11);
    write(2, "\n", 1);
    exit(1);
}

// Function to send a message to all clients except the specified one
void    send_to_all(int except)
{
    for (int fd = 0; fd <= maxfd; fd++)
    {
        if  (FD_ISSET(fd, &write_set) && fd != except)
            if (send(fd, send_buffer, strlen(send_buffer), 0) == -1)
                err(NULL);
    }
}

int     main(int ac, char **av)
{
    // Check for correct number of arguments
    if (ac != 2)
        err("Wrong number of arguments");

    // Set up server socket
    struct sockaddr_in  serveraddr;
    socklen_t len = sizeof(struct sockaddr_in);
    int serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1) err(NULL);
    maxfd = serverfd;

    // Initialize file descriptor sets and client array
    FD_ZERO(&current);
    FD_SET(serverfd, &current);
    bzero(clients, sizeof(clients));
    bzero(&serveraddr, sizeof(serveraddr));

    // Configure server address
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(atoi(av[1]));

    // Bind and listen on the server socket
    if (bind(serverfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1 || listen(serverfd, 100) == -1)
        err(NULL);

    // Main server loop
    while (1)
    {
        // Set up file descriptor sets for select()
        read_set = write_set = current;
        if (select(maxfd + 1, &read_set, &write_set, 0, 0) == -1) continue;

        // Check all file descriptors for activity
        for (int fd = 0; fd <= maxfd; fd++)
        {
            if (FD_ISSET(fd, &read_set))
            {
                if (fd == serverfd)
                {
                    // Accept new client connection
                    int clientfd = accept(serverfd, (struct sockaddr *)&serveraddr, &len);
                    
                    if (clientfd == -1) continue;
                    if (clientfd > maxfd) maxfd = clientfd;
                    clients[clientfd].id = gid++;
                    FD_SET(clientfd, &current);
                    sprintf(send_buffer, "server: client %d just arrived\n", clients[clientfd].id);
                    send_to_all(clientfd);
                    break;
                }
                else
                {
                    // Handle client message
                    int ret = recv(fd, recv_buffer, sizeof(recv_buffer), 0);
                    if (ret <= 0)
                    {
                        // Client disconnected
                        sprintf(send_buffer, "server: client %d just left\n", clients[fd].id);
                        send_to_all(fd);
                        FD_CLR(fd, &current);
                        close(fd);
                        bzero(clients[fd].msg, strlen(clients[fd].msg));
                        break;
                    }
                    else
                    {
                        // Process received message
                        for (int i = 0, j = strlen(clients[fd].msg); i < ret; i++, j++)
                        {
                            clients[fd].msg[j] = recv_buffer[i];
                            if (clients[fd].msg[j] == '\n')
                            {
                                clients[fd].msg[j] = '\0';
                                sprintf(send_buffer, "client %d: %s\n", clients[fd].id, clients[fd].msg);
                                send_to_all(fd);
                                bzero(clients[fd].msg, strlen(clients[fd].msg));
                                j = -1;
                            }
                        }
                    }
                }
            }
        }
    }
    return (0);
}
```

> These comments provide a high-level overview of the purpose of each section in the code, making it easier to understand the structure and functionality of the server program.

**Learn about the code you use:**
- [fd_set](documentation/fd_set.md)
- 

---------


# Expected File

mini_serv.c

# Allowed Functions

```cpp
#include <unistd.h>
write, close and select

#include <sys/socket.h>
socket, accept, listen, send, recv and bind

#include <string.h>
strstr, strlen, strcpy, strcat, memset and bzero

#include <stdlib.h>
malloc, realloc, free, calloc, atoi and exit

#include <stdio.h>
sprintf
```


# Subject Text

*Write a program that will listen for client to connect on a certain port on 127.0.0.1 and will let clients to speak with each other. This program will take as first argument the port to bind to.*

  - If no argument is given, it should write in stderr `"Wrong number of arguments"` followed by a \n and exit with status 1
  - If a System Calls returns an error before the program start accepting connection, it should write in stderr "Fatal error" followed by a \n and exit with status 1
  - If you cant allocate memory it should write in stderr "Fatal error" followed by a \n and exit with status 1

### Your Program

- Your program must be non-blocking but client can be lazy and if they don't read your message you must NOT disconnect them...
- Your program must not contains #define preproc
- Your program must only listen to 127.0.0.1

The fd that you will receive will already be set to make 'recv' or 'send' to block if select hasn't be called before calling them, but will not block otherwise. 

### When a client connect to the server:

- the client will be given an id. the first client will receive the id 0 and each new client will received the last client id + 1
- %d will be replace by this number
- a message is sent to all the client that was connected to the server: "server: client %d just arrived\n"

### Clients must be able to send messages to your program.

- message will only be printable characters, no need to check
- a single message can contains multiple \n
- when the server receive a message, it must resend it to all the other client with "client %d: " before every line!

### When a client disconnect from the server:

- a message is sent to all the client that was connected to the server: "server: client %d just left\n"

```
Memory or fd leaks are forbidden
```

# Exam Practice Tool
Practice the exam just like you would in the real exam using this tool - https://github.com/JCluzet/42_EXAM
    }
}
