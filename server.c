#include <stdio.h>
#include <unistd.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8000
int main()
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char status[1005];
    char send_buffer[1000008];
    char str[1005];

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;         // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc.
    address.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons(PORT);       // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    int yo = 0;
    int k = 0;

    // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("Connected with the client\n");
        }

        while (1)
        {
            char buffer[2010];
            bzero(buffer, sizeof(buffer));
            valread = read(new_socket, buffer, 2010); // read infromation received into the buffer
            if (strcmp(buffer, "exit") == 0)
            {
                break;
            }
            printf("Server~> Request for %s from the client\n", buffer);
            int fd = open(buffer, O_RDONLY);
            int sz1, sz = 0, max_sz = 0;

            if (fd == -1)
            {
                printf("Server~> Requested %s is missing or do not have read permissions\n", buffer);
                strcpy(str, "missit");

                send(new_socket, str, strlen(str), 0); // use sendto() and recvfrom() for DGRAM
            }
            else
            {
                printf("Server~> Sending %s to the client\n", buffer);
                max_sz = lseek(fd, 0, SEEK_END);
                lseek(fd, 0, SEEK_SET);
                sprintf(str, "%d", max_sz);
                send(new_socket, str, strlen(str), 0); // use sendto() and recvfrom() for DGRAM
                read(new_socket, status, 1000);
                while (sz < max_sz)
                {
                    if (sz + 1000000 > max_sz)
                    {
                        sz1 = read(fd, send_buffer, max_sz - sz);
                    }
                    else
                    {
                        sz1 = read(fd, send_buffer, 1000000);
                    }
                    send(new_socket, send_buffer, sz1, 0); // use sendto() and recvfrom() for DGRAM
                    sz += sz1;

                    if (sz == max_sz)
                    {
                        break;
                    }
                }
                printf("Server~> Sent %s to the server\n", buffer);
            }
        }
    }
    return 0;
}
