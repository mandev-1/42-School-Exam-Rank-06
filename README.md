# MINI SERVER IN C -- the LAST CORE EXAM in 42 PRAGUE
[📚Dictionary for ya](#must-know-by-heart)

[subject.en.txt](subjects/subject.en.txt)

###### Goal:
1. implement a server
2. use nc (netcat) (open two instances of nc) to test communication between clients connected to it
3. ???
4. profit!


TLDR: This exam is about FD_SET stuff (sockets), binding, listenning, accepting, receiving or sending (also all related to sockets).
> **IF YOU DONT KNOW WHAT IS A SOCKET....** then dont worry, it is just a fancy way of giving clients their own **"parking place"** (=> assigning them a socket).
> Think of it as a File Descriptor.... which you know from `write()` C function:


Alternatively, think of it as an ID with extra steps. 

The extra steps are:
1. Special Creation (to fit IPv4 / IPv6 standard & TCP or UDP, etc. etc.)
2. Binding socket to a specific IP Adress and port
3. ...
4. (the rest is the "standard server package", which is handled serverside and clientside)(listen, connect, close, send, etc.)

###### Need to know:
- **fd_set** : data structure used to manage filedescriptors (sockets); you dont directly manipulate `fd_set`. Instead, you use __macros__ to add, validate or remove fd. from the set. 

1. FD_ZERO(&set): Clears all file descriptors from the set.
2. FD_SET(fd, &set): Adds a file descriptor to the set.
3. FD_CLR(fd, &set): Removes a file descriptor from the set.
4. FD_ISSET(fd, &set): Checks if a file descriptor is in the set.

- **struct sockaddr_in** : this is imported and we will use it. It will store address family, port and IPv4 [... continue](documentation/socket_creation.md). You need to LEARN how to config. these three attributes! 
> we will define attribute `len` to STORE sizeof sockaddr_in!! (helps `accept()`)  
> It will be stored in SPECIAL type `socklen_t` !! The len MUST BE STORED in SOCKLEN_T


- **struct sockaddr** : contains ONLY the family (`sa_family`) and address data. __We use it to typecast serveraddress only!__ Allows correctly `accept()` and `bind()` the serveraddress - accept and bind dont work with sockaddr_in 

[... continue](#must-know-by-heart)

> 💡 check out the code now to feel less lost

# IMPLEMENTATION

```c
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

// Define a structure to store client information
typedef struct s_client
{
    int     id;
    char    msg[100000];
}   t_client;

// Global variables for managing clients and file descriptors
// We're gonna be buffering the messages - improves efficiency, prevents blocking
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
```
###### notes:
> The above portion is setting up our structure, the client, socket variables, and int and char helpers  
> !Note the variables are global :)!

> The `send_to_all` function is checking if the fd IS in the set (prevents failure)  

> .. and then uses `send()` to send the full buffered string to the client connected on the specified socket
> (this loops)  
> 💡 The **except** helps us not send the message to ourselves  



### main function:
```c
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
>

###### Must know by heart:

**Learn about the code you use:**
- [fd_set](documentation/fd_set.md)
- [create sockets](documentation/bind_listen_socket.md)
- [bind and listen to sockets](documentation/socket_creation.md)

- [create server loop](documentation/implementing_server.md)

---------


# Expected File

[mini_serv.c](mini_serv.c)

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
