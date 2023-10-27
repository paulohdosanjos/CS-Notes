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

#define QUEUE_NAME_LENGTH_OFFSET 13

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
