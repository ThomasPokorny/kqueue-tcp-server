#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <string.h>
#include <unistd.h>

#define ERROR_EXIT(...) { fprintf(stderr, "ERROR: " __VA_ARGS__); exit(EXIT_FAILURE); }

typedef void (*IntIntFunc)(int, int);

// Function that takes two int arguments and prints them
void printSum(int a, int b) {
  int sum = a +b;
  printf("sum of %d + %d = %d\n", a, b, sum);
}

void printMultiplication(int a, int b) {
  int multi = a  * b;
  printf("product of %d * %d = %d\n", a, b, multi);
}

int main()
{
    // Initialise all needed variables.
    int socket_listen_fd,
        portno = 1816,
        client_len,
        socket_connection_fd,
        kq,
        new_events,
        callbacks_len = 2;
    struct kevent change_event[4],
        event[4];
    struct sockaddr_in serv_addr,
        client_addr;


    // initialise our callback functions
    IntIntFunc* callbacks = malloc(callbacks_len * sizeof(IntIntFunc)); // 8
    callbacks[0] = &printSum;
    callbacks[1] = &printMultiplication;

    // Create socket
    if (((socket_listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0))
    {
      ERROR_EXIT("Error creating socket");
    }

    // Bind to ip address
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(socket_listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        ERROR_EXIT("Error binding socket");
    }

    // Mark socket as passive and start listening
    listen(socket_listen_fd, 3);
    client_len = sizeof(client_addr);

    // Create kqueue
    kq = kqueue();

    // Create filter for events that we want to monitor on given fd
    // one the EVFILT_READ event has occured, we add it to the event queue.
    EV_SET(change_event, socket_listen_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

    // Register events
    if (kevent(kq, change_event, 1, NULL, 0, NULL) == -1)
    {
        ERROR_EXIT("kevent: register socket_listen_fd");
    }

    // Event Loop ⚛️
    printf("Event loop up and running!🚀⚛️\nCallbacks registered!⚡️\nWaiting for incoming events📡...\n");
    for (;;)
    {
        // Poll for new events
        new_events = kevent(kq, NULL, 0, event, 1, NULL);
        if (new_events == -1)
        {
          ERROR_EXIT("kevent: poll for events");
        }

        for (int i = 0; new_events > i; i++)
        {
            int event_fd = event[i].ident;

            // Close fd when client disconnects
            if (event[i].flags & EV_EOF)
            {
               close(event_fd);
            }
            // In case the events fd is the same as our listening fd
            // we have an incoming connection
            else if (event_fd == socket_listen_fd)
            {
                printf("New connection coming in...\n");

                socket_connection_fd = accept(event_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_len);
                if (socket_connection_fd == -1)
                {
                  ERROR_EXIT("accept socket");
                }

                // Add the new connection to our interest list
                EV_SET(change_event, socket_connection_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, change_event, 1, NULL, 0, NULL) < 0)
                {
                    ERROR_EXIT("kevent: register socket_connection_fd");
                }
            }

            else if (event[i].filter & EVFILT_READ)
            {
              // Read bytes from socket
              char buf[1024];
              size_t bytes_read = recv(event_fd, buf, sizeof(buf), 0);

              int a = 0;
              int b = 0;
              sscanf(buf, "%d %d", &a, &b);

              // execute 'callbacks'
              for(int l = 0; l < callbacks_len; l++) {
                callbacks[l](a, b);
              }
            }
        }
    }

    return 0;
}
