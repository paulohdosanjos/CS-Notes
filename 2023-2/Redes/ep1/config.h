#ifndef CONFIG_H
#define CONFIG_H

#define MAXSIZE 10000
#define PROTOCOL_HEADER_SIZE 8
#define FRAME_END 0xce
#define MAX_NUM_QUEUE 8

#define TRUE 1
#define FALSE 0

#define LISTENQ 1
#define MAXDATASIZE 100
#define MAXLINE 4096

#include "queue.h"

typedef struct {
  int connfd;
  queue* queue_list[MAX_NUM_QUEUE];
  int queue_list_size;

} server_data;

#endif
