#include <stdlib.h>
#include <stdio.h>
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
#include <pthread.h>

#include "config.h"
#include "queue.h"

#define NUM_STATES 16
#define MAX_STATE_NAME_LENGTH 30
#define MAX_TRANSITIONS 2
#define QUEUE_NAME_LENGTH_OFFSET 13

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
  WAIT_QUEUE_DECLARE, 
  RCVD_QUEUE_DECLARE, 
  WAIT_CHANNEL_CLOSE, 
  RCVD_CHANNEL_CLOSE, 
  WAIT_CONNECTION_CLOSE,
  RCVD_CONNECTION_CLOSE,
  FINAL
} state;

extern int (*actions[NUM_STATES])(server_data*);
extern state transitions[NUM_STATES][MAX_TRANSITIONS];
extern char* state_name[NUM_STATES];
extern const unsigned char DEFAULT_PROTOCOL_HEADER[];

int do_WAIT_HEADER (server_data* data);

int do_RCVD_HEADER (server_data* data);

int do_WAIT_START_OK (server_data* data);

int do_RCVD_START_OK (server_data* data);

int do_WAIT_TUNE_OK (server_data* data);

int do_WAIT_CONNECTION_OPEN (server_data* data);

int do_RCVD_CONNECTION_OPEN (server_data* data);

int do_WAIT_CHANNEL_OPEN (server_data* data);

int do_RCVD_CHANNEL_OPEN (server_data* data);

int do_WAIT_QUEUE_DECLARE (server_data* data);

int do_RCVD_QUEUE_DECLARE (server_data* data);

int do_WAIT_CHANNEL_CLOSE (server_data* data);

int do_RCVD_CHANNEL_CLOSE (server_data* data);

int do_WAIT_CONNECTION_CLOSE (server_data* data);

int do_RCVD_CONNECTION_CLOSE (server_data* data);

int do_FINAL(server_data* data);
