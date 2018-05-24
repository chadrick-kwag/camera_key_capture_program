#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

//int argc, char *argv[]
int main(void)
{

    printf("start server");


    int listenfd = 0, client_sock = 0;
    struct sockaddr_in serv_addr; 

    char sendBuff[1025];
    char client_message[2000]={};
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);  

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

    printf("listen finished");

    // while(1)
    // {

    //     connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
    //     printf("%d\n",connfd); 


        
    //     printf("accept returned fd >0");
    //     ticks = time(NULL);
    //     snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
    //     write(connfd, sendBuff, strlen(sendBuff)); 

    //     close(connfd);

        

        
    //     sleep(1);
    // }


    //accept connection from an incoming client
    int read_size;
    client_sock = accept(listenfd, NULL, NULL);
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");
     
    //Receive a message from client
    while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
    {
        //Send the message back to client
        printf("sending back %s",client_message);
        write(client_sock , client_message , strlen(client_message));
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    return 0;
}