#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

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

#include "server_config.h"
#include "queue.h"

#define MAX_TRANSITIONS 2
#define QUEUE_NAME_LENGTH_OFFSET 13
#define ROUTING_KEY_SIZE_POSITION 14

extern int (*actions[NUM_STATES])(client_thread*, server_data*);
extern state transitions[NUM_STATES][MAX_TRANSITIONS];
extern char* state_name[NUM_STATES];
extern const unsigned char DEFAULT_PROTOCOL_HEADER[];

int do_WAIT_HEADER (client_thread* data, server_data*);

int do_RCVD_HEADER (client_thread* data, server_data*);

int do_WAIT_START_OK (client_thread* data, server_data*);

int do_RCVD_START_OK (client_thread* data, server_data*);

int do_WAIT_TUNE_OK (client_thread* data, server_data*);

int do_WAIT_CONNECTION_OPEN (client_thread* data, server_data*);

int do_RCVD_CONNECTION_OPEN (client_thread* data, server_data*);

int do_WAIT_CHANNEL_OPEN (client_thread* data, server_data*);

int do_RCVD_CHANNEL_OPEN (client_thread* data, server_data*);

int do_WAIT_COMMAND (client_thread* data, server_data*);

int do_RCVD_QUEUE_DECLARE (client_thread* data, server_data*);

int do_RCVD_BASIC_PUBLISH (client_thread* data, server_data*);

int do_WAIT_CHANNEL_CLOSE (client_thread* data, server_data*);

int do_RCVD_CHANNEL_CLOSE (client_thread* data, server_data*);

int do_WAIT_CONNECTION_CLOSE (client_thread* data, server_data*);

int do_RCVD_CONNECTION_CLOSE (client_thread* data, server_data*);

int do_FINAL(client_thread* data, server_data*);

#endif
