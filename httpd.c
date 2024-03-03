#include "httpd.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_CONNECTIONS 1000
#define BUF_SIZE 65535
#define QUEUE_SIZE 1000000

int* clients;
static char* buf;
static int listenfd;
static void start_server(const char *);
static void respond(int, const char*);

// Client request
char* method; // "GET" or "POST"
char* uri;     // "/index.html" things before '?'
char* prot;    // "HTTP/1.1"

void serve_forever(const char* PORT, const char* REDIRECT_TARGET)
{
    struct sockaddr_in clientaddr;
    socklen_t addrlen;

    int slot = 0;

    printf("Server started %shttp://127.0.0.1:%s%s\n", "\033[92m", PORT, "\033[0m");

    // create shared memory for client slot array
    clients = mmap(NULL, sizeof(*clients) * MAX_CONNECTIONS, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    // Setting all elements to -1: signifies there is no client connected
    int i;

    for (i = 0; i < MAX_CONNECTIONS; i++)
    {
        clients[i] = -1;
    }

    start_server(PORT);

    // Ignore SIGCHLD to avoid zombie threads
    signal(SIGCHLD, SIG_IGN);

    // ACCEPT connections
    while (1) 
    {
        addrlen = sizeof(clientaddr);
        clients[slot] = accept(listenfd, (struct sockaddr*)&clientaddr, &addrlen);

        if (clients[slot] < 0)
        {
            perror("accept() error");
            exit(1);
        }
        else 
        {
            if (fork() == 0)
            {
                close(listenfd);

                respond(slot, REDIRECT_TARGET);
                close(clients[slot]);
                clients[slot] = -1;
                
                exit(0);
            }
            else 
            {
                close(clients[slot]);
            }
        }

        while (clients[slot] != -1)
        {
            slot = (slot + 1) % MAX_CONNECTIONS;
        }
    }
}

// start server
void start_server(const char* port)
{
    struct addrinfo hints, * res, * p;

    // getaddrinfo for host
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    if (getaddrinfo(NULL, port, &hints, &res) != 0) 
    {
        perror("getaddrinfo() error");
        exit(1);
    }

    // socket and bind
    for (p = res; p != NULL; p = p->ai_next)
    {
        int option = 1;

        listenfd = socket(p->ai_family, p->ai_socktype, 0);
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

        if (listenfd == -1)
            continue;

        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break;
    }

    if (p == NULL)
    {
        perror("socket() or bind()");
        exit(1);
    }

    freeaddrinfo(res);

    // listen for incoming connections
    if (listen(listenfd, QUEUE_SIZE) != 0)
    {
        perror("listen() error");
        exit(1);
    }
}

// client connection
void respond(int slot, const char* redirect_target)
{
    int rcvd;

    buf = malloc(BUF_SIZE);
    rcvd = recv(clients[slot], buf, BUF_SIZE, 0);

    if (rcvd < 0) // receive error
        fprintf(stderr, ("recv() error\n"));
    else if (rcvd == 0) // receive socket closed
        fprintf(stderr, "Client disconnected unexpectedly.\n");
    else // message received
    {
        buf[rcvd] = '\0';

        method = strtok(buf, " \t\r\n");
        uri = strtok(NULL, " \t");
        prot = strtok(NULL, " \t\r\n");

        fprintf(stderr, "\x1b[32m + [%s] %s\x1b[0m\n", method, uri);

        // bind clientfd to stdout, making it easier to write
        int clientfd = clients[slot];
        dup2(clientfd, STDOUT_FILENO);
        close(clientfd);

        // call router
        if (strcmp("GET", method) == 0)
        {
            printf("%s 302 Found\n", prot);
            printf("Content-Length: 0\n");
            printf("Location: %s%s\n\n", redirect_target, uri);
        }
        else
            printf("%s 403 Forbidden\n\n", prot);

        // tidy up
        fflush(stdout);
        shutdown(STDOUT_FILENO, SHUT_WR);
        close(STDOUT_FILENO);
    }

    free(buf);
}
