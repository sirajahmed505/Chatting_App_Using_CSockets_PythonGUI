#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include<pthread.h>
#include<fcntl.h>
#include<errno.h>

struct client
{
    int sockfd;
    char name[50];
};


int server_fd; //server socket
struct sockaddr_in address;
int addressLength;
int maxNumberOfClients; //
int numberOfClients = 0; 
struct client *clients; //array of sockets fd for each client.
pthread_t *threads;

int is_valid_fd(int fd) /*If file descriptor is valid, returns 1, else 0*/
{
    return fcntl(fd, F_GETFL) != -1 || errno != EBADF;
}
void ServerInit(int max_num_clients, uint16_t port)
{
    maxNumberOfClients = max_num_clients;
    clients = (client *)malloc(max_num_clients * sizeof(client));
    threads = (pthread_t *)malloc(max_num_clients * sizeof(pthread_t));
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
    }

    int enable = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    addressLength = sizeof(address);

    /* This block of code can be used to set *socket options*. It might be needed later on. 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    {
         perror("setsockopt");
         exit(EXIT_FAILURE);
    }
    */
    if ((bind(server_fd, (struct sockaddr*)&address, sizeof(address))) < 0)
    {
            perror("Bind error");
            exit(EXIT_FAILURE);
    }
    if ((listen(server_fd, max_num_clients)) < 0)
    {
            perror("Listen error");
            exit(EXIT_FAILURE);
    }
}


void * HandleClient(void * _i)
{
        char buffer[1024] = { 0 };
        char toSend[1024] = { 0 };
        int curr = *((int *)_i);
        if ((clients[curr].sockfd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addressLength)) < 0) //accept returns client socket fd.
        {
            perror("Accept error");
            exit(EXIT_FAILURE);
        }

        //Registering name of the client here.
        while(1)
        {
            int unused = 1;
            int valread = read(clients[curr].sockfd, buffer, sizeof(buffer)); 
            if(valread == 0)
            {
                return NULL;
            }
            for(size_t i = 0; i < maxNumberOfClients; i++)
            {
                if(is_valid_fd(clients[curr].sockfd))
                {
                        if(i == curr) continue;
                        if(strcmp(buffer, clients[i].name) == 0)
                        {
                            unused = 0;
                            break;
                        }
                }
            }
            if(unused == 1)
            {
                strcpy(clients[curr].name, buffer);
                send(clients[curr].sockfd, "1", 2, 0);
                break;
            }
            send(clients[curr].sockfd, "0", 2, 0);
        }
        while(1)
        {
            int valread = read(clients[curr].sockfd, buffer, sizeof(buffer)); 
            if((strcmp(buffer, "$Exit") == 0) || valread == 0) /*if no bytes are received, it means that client process has been closed.*/
            {
                close(clients[curr].sockfd); //invalidates the client socket fd thus closing the connection.
                return NULL; 
            }
            strcpy(toSend, clients[curr].name);
            strcat(toSend, ": ");
            strcat(toSend, buffer);

            for (size_t i = 0; i < maxNumberOfClients; i++)
            {
                if(is_valid_fd(clients[curr].sockfd))
                {
                    int val=send(clients[i].sockfd, toSend, strlen(toSend) + 1, 0); //+1 for the null terminator
                }
            }
            printf("%s\n", toSend);
        }
        free(_i);
}

void ServerInitWorkerThreads()
{
    for (int i = 0; i < maxNumberOfClients; i++)
        {
            int *offset = (int*)malloc(sizeof(int)); //We have to dynamically allocate so that all threads read separate memory
            *offset = i;
            if((pthread_create(&threads[i], NULL, HandleClient, (void*)offset)) < 0)
            {
                perror("error while creation of thread.");
                exit(EXIT_FAILURE);
            }
        }
}

void ServerWaitForWorkerThreads()
{
    for (int i = 0; i < maxNumberOfClients; i++)
    {
        pthread_join(threads[i], NULL);
    }
}

void ServerDestroy()
{
    free(clients);
    free(threads);
}


int main(int argc, char *argv[])
{
    ServerInit(atoi(argv[1]), 8000); // specify number of clients through cmd line.
    ServerInitWorkerThreads();
    ServerWaitForWorkerThreads();
    ServerDestroy();
    //Gracefully exits
    return 0;
}
