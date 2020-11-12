// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8000
char *str, arr[100][1000], *ptr, retstr[2000] = {0}, status[25] = {0};
size_t sz2 = 2002;
int tokenize_str(char *str2)
{
    int tot = 0;
    ptr = strtok(str2, " \t");
    while (ptr != NULL)
    {
        strcpy(arr[tot], ptr);
        tot++;
        ptr = strtok(NULL, " \t");
    }
    return tot;
}
int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    char nav_string[] = "##########################################################################";
    int nav_width = 75;
    struct sockaddr_in serv_addr;
    char *file_name = (char *)malloc(sizeof(int) * 2000);
    char buffer[1000008], sbuf[1007];
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                // which is meant to be, and rest is defined below

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converts an IP address in numbers-and-dots notation into either a
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    int k = 1;

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) // connect to the server address
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    else
    {
        printf("Connected to the server\n");
    }

    ptr = (char *)malloc(sizeof(int) * 2000);
    str = (char *)malloc(sizeof(int) * sz2);
    while (k == 1)
    {
        printf("Client~> ");

        int ret = getline(&str, &sz2, stdin);
        str[strlen(str) - 1] = '\0';
        if (str == NULL)
        {
            continue;
        }
        int no_arg = tokenize_str(str);
        // printf("%d\n", no_arg);
        if (no_arg == 0)
        {
            continue;
        }
        else if (no_arg == 1 && strcmp(arr[0], "exit") == 0)
        {
            strcpy(status, "exit");
            send(sock, status, strlen(status), 0);
            printf("Connection with server closed\n");
            break;
        }
        else if (no_arg >= 1 && strcmp(arr[0], "get") == 0)
        {
            for (int i = 1; i < no_arg; i++)
            {

                strcpy(file_name, arr[i]);
                int outp = send(sock, file_name, strlen(file_name), 0); // send the message.
                if (outp == -1)
                {
                    sprintf(retstr, "Error %s ", arr[i]);
                    perror(retstr);
                    continue;
                }
                else
                {
                    printf("Downloading %s \n", file_name);
                    bzero(sbuf, sizeof(sbuf));

                    valread = read(sock, sbuf, 1000); // receive message back from server, into the buffer

                    if (strcmp("missit", sbuf) == 0)
                    {
                        printf("File %s does not exist or client does not have permission to read/download\n", file_name);
                        printf("------------------------------------------------------------------------------------------------------------\n");
                        continue;
                    }
                    else
                    {
                        int file_sz = atoi(sbuf);
                        int offset = 0, offset2 = 0, k;
                        int fd2 = open(arr[i], O_WRONLY | O_CREAT | O_TRUNC, 0664);
                        strcpy(status, "begin");
                        send(sock, status, strlen(status), 0);
                        int cnt = 0;
                        while (1)
                        {
                            valread = read(sock, buffer, 1000000); // receive message back from server, into the buffer
                            k = write(fd2, buffer, valread);
                            offset += valread;
                            double k = (double)100;
                            if (file_sz > 0)
                                k = ((double)(offset)*100 / file_sz);
                            int val = (int)(k);
                            int lft = (int)(val * nav_width / 100);
                            int rt = nav_width - lft;
                            printf("\rPercentage written: %3.2lf%% [%.*s%*s]", k, lft, nav_string, rt, "");

                            fflush(stdout);
                            if (offset == file_sz)
                            {
                                break;
                            }
                        }
                        printf("\n%s Downloaded !!!\n", file_name);
                        printf("------------------------------------------------------------------------------------------------------------\n");
                    }
                }
            }
        }
    }
    return 0;
}