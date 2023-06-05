#include <arpa/inet.h>
#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include<fcntl.h>
#include<errno.h>
#include <sys/time.h>
#include <signal.h>
using namespace std;

#define WRITE 1
#define READ 0
#define GRAPH 2

int sockfd; //server socket fd
struct sockaddr_in serv_addr;
pthread_t threads[3];
int p1[2];
int p2[2];
int child;
int is_valid_fd(int fd) 
{
    return fcntl(fd, F_GETFL) != -1 || errno != EBADF;
}

void ClientInit(char * ip, uint16_t port)
{
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    if ((inet_pton(AF_INET, ip, &serv_addr.sin_addr)) <= 0)
    {
        printf("Invalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }
    if ((connect(sockfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)))< 0)
    {
        printf("Connection Failed \n");
        exit(EXIT_FAILURE);
    }
    pipe(p1);
    pipe(p2);
}
void * Send(void *)
{
    char temp[1000];
    while(1)
    {
        int valRead = read(p2[READ], temp, sizeof(temp));
        // fgets(temp, sizeof(temp), stdin);
        // temp[strlen(temp) - 1] = '\0';
        // printf("%s : %d\n",temp, valRead);
        temp[valRead] = '\0';
        if(is_valid_fd(sockfd))
        {
            send(sockfd, temp, valRead + 1, 0);
        }
        else
        {
            printf("Server has shutdown.\n");
            return NULL;
        }
    }    
}
void * Receive(void *)
{
    char buffer[1024];
    while(1)
    {
        int valRead = read(sockfd, buffer, sizeof(buffer));
        if(valRead <= 0) //This means that server is no longer connected
        {
            return NULL;
        }
        write(p1[WRITE], buffer, valRead + 1);
        printf("%s\n", buffer);
    }
}
void ClientRegisterToServer()
{
    char buffer[50];
    while(1)
    {
        printf("Please enter username:");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strlen(buffer) - 1] = '\0';
        if(is_valid_fd(sockfd))
        {
            send(sockfd, buffer, strlen(buffer) + 1, 0);
        }
        else
        {
            printf("Server has shutdown.\n");
            exit(EXIT_FAILURE);
        }
        int valRead = read(sockfd, buffer, sizeof(buffer));
        if(valRead <= 0) //This means that server is no longer connected.
        {
            printf("Error while Receiving!\n");
            exit(EXIT_FAILURE);
        }
        //1 means valid
        //0 means already used usrname
        if(strcmp(buffer, "1") == 0) //valid username
        {
            break;
        }
        printf("This username is already in use. Please enter another.\n");
    }
}
void ClientThreadsInit()
{
    child = fork();
    if(child == 0)
    {
        char r1[10], w1[10], r2[10], w2[10];
        sprintf(r1, "%d", p1[READ]);
        sprintf(w1, "%d", p1[WRITE]);  
        sprintf(r2, "%d", p2[READ]);
        sprintf(w2, "%d", p2[WRITE]);
        char * args[] = {"python3", "gui.py", r1, w1, r2, w2, NULL};
        execv("/usr/bin/python3", args);
    }
    if(child > 0)
    {
        pthread_create(&threads[READ], NULL, Receive, NULL);
        pthread_create(&threads[WRITE], NULL, Send, NULL);
    }
}
void ClientWaitForThreads()
{
    for(int i = 0 ; i < 2; i++)
    {
        pthread_join(threads[i], NULL);
    }
}

int main(int argc, char const* argv[])
{
    ClientInit("10.0.2.15", 8000); //Edit this ip to match that of the server. Ports have to be the same on both client and server.
    ClientRegisterToServer();
    ClientThreadsInit();
    ClientWaitForThreads();
    kill(child, SIGKILL); //kills the python program
    //Graceful exit
    return 0;
}
