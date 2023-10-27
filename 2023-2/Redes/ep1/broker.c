#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>

#include "config.h"
#include "state_machine.h"
#include "utils.h"


int main (int argc, char **argv) {
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    pid_t childpid;
    char recvline[MAXLINE + 1];
    ssize_t n;
   
    if (argc != 2) {
        fprintf(stderr,"Uso: %s <Porta>\n",argv[0]);
        fprintf(stderr,"Vai rodar um servidor de echo na porta <Porta> TCP\n");
        exit(1);
    }

   if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket :(\n");
        exit(2);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(atoi(argv[1]));

    int option = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)); // Para não precisar esperar o TIME_WAIT

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind :(\n");
        exit(3);
    }

   if (listen(listenfd, LISTENQ) == -1) {
        perror("listen :(\n");
        exit(4);
    }

    printf("[Servidor no ar. Aguardando conexões na porta %s]\n",argv[1]);
    printf("[Para finalizar, pressione CTRL+c ou rode um kill ou killall]\n");
   
    for (;;) {
   
        if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1 ) {
            perror("accept :(\n");
            exit(5);
        }

        close(listenfd);

        //int (*actions[])()
    
        // Recebe Protocol header

        server_data* data = (server_data*) malloc(sizeof(server_data));
        data->connfd = connfd;
        data->queue_list_size = 0;

        do_WAIT_HEADER(data);

        // Envia Connection.Start

        do_RCVD_HEADER(data);

        // Recebe Connection.Start-Ok

        do_WAIT_START_OK(data);

        // Envia o Connection.Tune

        do_RCVD_START_OK(data);

        // Recebe o Connection.Tune-Ok

        int n = do_WAIT_TUNE_OK(data);

        if(n == 1) do_WAIT_CONNECTION_OPEN(data);

        // Envia o Connection.Open-Ok

        do_RCVD_CONNECTION_OPEN(data);

        // Recebe Channel.Open
        
        do_WAIT_CHANNEL_OPEN(data);
        
        // Envia Channel.Open-Ok

        do_RCVD_CHANNEL_OPEN(data);

        // Agora cheguei no estado WAIT_FUNCTIONAL

        // Recebe o Queue.Declare 

        do_WAIT_QUEUE_DECLARE(data);

        // Envia o Queue.Declare-Ok 

        do_RCVD_QUEUE_DECLARE(data);
        
        // Recebe o Channel.Close 

        do_WAIT_CHANNEL_CLOSE(data);
        
        // Envia Channel.Close-Ok

        do_RCVD_CHANNEL_CLOSE(data);

        // Recebe o Connection.Close 
        
        do_WAIT_CONNECTION_CLOSE(data);

        // Envia Connection.Close-Ok

        do_RCVD_CONNECTION_CLOSE(data);
        
        for(;;){}

        close(connfd);
        printf("[Uma conexão fechada]\n");
        //exit(0);

    }
    exit(0);
}
