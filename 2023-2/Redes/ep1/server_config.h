#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#define MAXSIZE 10000
#define PROTOCOL_HEADER_SIZE 8
#define FRAME_END 0xce
#define MAX_NUM_QUEUE 8

#define TRUE 1
#define FALSE 0

#define LISTENQ 1
#define MAXDATASIZE 100
#define MAXLINE 4096
#define INICIAL_STATE WAIT_HEADER
#define NUM_STATES 17

#define METHOD_ID_POSITION 10
#define DECLARE_ID 10
#define PUBLISH_ID 40
#define CONSUME_ID 20

#define MAX_CLIENTS 100

#include <pthread.h>

#include "queue.h"

// Estados da maquina de estados do servidor
typedef enum {
  WAIT_HEADER,
  RCVD_HEADER, 
  WAIT_START_OK,
  RCVD_START_OK,
  WAIT_TUNE_OK, 
  WAIT_CONNECTION_OPEN,
  RCVD_CONNECTION_OPEN, 
  WAIT_CHANNEL_OPEN, 
  RCVD_CHANNEL_OPEN, 
  WAIT_COMMAND,
  RCVD_QUEUE_DECLARE, 
  RCVD_BASIC_PUBLISH, 
  WAIT_CHANNEL_CLOSE, 
  RCVD_CHANNEL_CLOSE, 
  WAIT_CONNECTION_CLOSE,
  RCVD_CONNECTION_CLOSE,
  FINAL
} state;

typedef struct {
  int connfd;
  state current_state;
  pthread_t thread_id;
  unsigned char buf[MAXSIZE];
} client_thread;

typedef struct {
  queue* queue_list[MAX_NUM_QUEUE];
  int queue_list_size;
} server_data;

#endif

