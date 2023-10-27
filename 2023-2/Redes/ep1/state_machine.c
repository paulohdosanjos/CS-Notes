#include "state_machine.h"
#include "utils.h"

const unsigned char DEFAULT_PROTOCOL_HEADER[] = "\x41\x4d\x51\x50\x00\x00\x09\x01";

int (*actions[NUM_STATES])(server_data*) = {
  do_WAIT_HEADER,
  do_RCVD_HEADER, 
  do_WAIT_START_OK,
  do_RCVD_START_OK,
  do_WAIT_TUNE_OK, 
  do_WAIT_CONNECTION_OPEN,
  do_RCVD_CONNECTION_OPEN, 
  do_WAIT_CHANNEL_OPEN, 
  do_RCVD_CHANNEL_OPEN, 
  do_WAIT_QUEUE_DECLARE, 
  do_RCVD_QUEUE_DECLARE, 
  do_WAIT_CHANNEL_CLOSE, 
  do_RCVD_CHANNEL_CLOSE, 
  do_WAIT_CONNECTION_CLOSE,
  do_RCVD_CONNECTION_CLOSE,
  do_FINAL
};

state transitions[NUM_STATES][MAX_TRANSITIONS] = {
  {RCVD_HEADER, WAIT_HEADER}, // WAIT_HEADER,
  {WAIT_START_OK}, // RCVD_HEADER, 
  {RCVD_START_OK}, // WAIT_START_OK,
  {WAIT_TUNE_OK}, // RCVD_START_OK,
  {WAIT_CONNECTION_OPEN, RCVD_CONNECTION_OPEN}, // WAIT_TUNE_OK, 
  {RCVD_CONNECTION_OPEN}, // WAIT_CONNECTION_OPEN,
  {WAIT_CHANNEL_OPEN}, // RCVD_CONNECTION_OPEN, 
  {RCVD_CHANNEL_OPEN}, // WAIT_CHANNEL_OPEN, 
  {WAIT_QUEUE_DECLARE}, // RCVD_CHANNEL_OPEN, 
  {RCVD_QUEUE_DECLARE, RCVD_QUEUE_DECLARE}, // WAIT_QUEUE_DECLARE, 
  {WAIT_CHANNEL_CLOSE}, // RCVD_QUEUE_DECLARE, 
  {RCVD_CHANNEL_CLOSE}, // WAIT_CHANNEL_CLOSE, 
  {WAIT_CONNECTION_CLOSE}, // RCVD_CHANNEL_CLOSE, 
  {RCVD_CONNECTION_CLOSE}, // WAIT_CONNECTION_CLOSE,
  {WAIT_HEADER}, // RCVD_CONNECTION_CLOSE
  {FINAL}        // FINAL
};

char* state_name[NUM_STATES] = {
  "WAIT_HEADER",
  "RCVD_HEADER", 
  "WAIT_START_OK",
  "RCVD_START_OK",
  "WAIT_TUNE_OK", 
  "WAIT_CONNECTION_OPEN",
  "RCVD_CONNECTION_OPEN", 
  "WAIT_CHANNEL_OPEN", 
  "RCVD_CHANNEL_OPEN", 
  "WAIT_QUEUE_DECLARE", 
  "RCVD_QUEUE_DECLARE", 
  "WAIT_CHANNEL_CLOSE", 
  "RCVD_CHANNEL_CLOSE", 
  "WAIT_CONNECTION_CLOSE",
  "RCVD_CONNECTION_CLOSE",
  "FINAL"
};

// Servidor espera o protocol header do cliente e o valida. Retorna 1 se tudo deu certo e o protocolo é válido, retorna 0 caso contrário
int do_WAIT_HEADER (server_data* data)
{
  unsigned char buf[MAXSIZE];
  read_header(data->connfd, buf);

  printf("Protocol Header recebido\n");

  if(memcmp(buf, DEFAULT_PROTOCOL_HEADER, sizeof(DEFAULT_PROTOCOL_HEADER)) == 0) return 0;
  else 
  { 
    printf("Protocolo inválido\n");
    return 1;
  }
}

// Servidor manda um Connection.Start para o cliente
int do_RCVD_HEADER (server_data* data)
{
    unsigned char buf[MAXSIZE];
    int n = connection_start(buf);
    write(data->connfd, buf, n);

    printf("Connection.Start enviado\n");

    return 0;
}

// Servidor espera um Connection.Start-Ok do cliente
int do_WAIT_START_OK (server_data* data)
{
  unsigned char buf[MAXSIZE];
  read_frame(data->connfd, buf);

  printf("Connection.Start-Ok recebido\n");
  return 0;
}

// Servidor manda um Connection.Tune para o cliente
int do_RCVD_START_OK (server_data* data)
{
  int n;
  unsigned char buf[MAXSIZE];
  n = connection_tune(buf);
  write(data->connfd, buf, n);

  printf("Connection.Tune enviado\n");

  return 0;
}

// Servidor espera um Connection.Tune-Ok do cliente. Como logo em seguida ele deverá receber um Connection.Open, é possível que essa ação leia os dois comandos de uma vez. Retorna 1 caso isso aconteça e 0 caso contrário
int do_WAIT_TUNE_OK (server_data* data)
{

  unsigned char buf[MAXSIZE];
  int bytes_read = read_frame(data->connfd, buf);

  printf("Connection.Tune-Ok recebido\n");

  int count = 0; // Contará quantos frames amqp foram enviados pelo cliente
  for(int i = 0; i < bytes_read; i++) if(buf[i] == FRAME_END) count++; 

  if(count == 2) 
    printf("Connection.Open recebido\n");

  return count - 1;
}

// Servidor recebe Connection.Open do cliente
int do_WAIT_CONNECTION_OPEN (server_data* data)
{
  unsigned char buf[MAXSIZE];
  int bytes_read = read_frame(data->connfd, buf);

  return 0;
}

// Servidor envia Connection.Open-Ok para o cliente
int do_RCVD_CONNECTION_OPEN (server_data* data)
{
  int n;
  unsigned char buf[MAXSIZE];
  n = connection_open_ok(buf);
  write(data->connfd, buf, n);

  printf("Connection.Open-Ok enviado\n");

  return 0;
}

// Servidor recebe Channel.Open do cliente
int do_WAIT_CHANNEL_OPEN (server_data* data)
{
  unsigned char buf[MAXSIZE];
  int bytes_read = read_frame(data->connfd, buf);

  printf("Channel.Open recebido\n");

  return 0;
}

// Servidor envia Channel.Open-Ok para o cliente
int do_RCVD_CHANNEL_OPEN(server_data* data)
{
  int n;
  unsigned char buf[MAXSIZE];
  n = channel_open_ok(buf);
  write(data->connfd, buf, n);

  printf("Channel.Open-Ok enviado\n");

  return 0;
}

// Servidor recebe Queue.Declare do cliente e faz a criação da nova fila. Retorna a quantidade de filas criadas
int do_WAIT_QUEUE_DECLARE (server_data* data)
{
  unsigned char buf[MAXSIZE];
  int bytes_read = read_frame(data->connfd, buf);
  
  printf("Queue.Declare recebido\n");

  // Extrai nome da fila
  int queue_name_size = buf[QUEUE_NAME_LENGTH_OFFSET];
  char queue_name[MAXLINE];
  memcpy(queue_name, buf + QUEUE_NAME_LENGTH_OFFSET + 1, queue_name_size);
  queue_name[queue_name_size] = 0;

  // Cria uma fila vazia com o nome requerido. Depois verificar se a fila existe
  int is_present = FALSE;

  for(int i = 0; i < data->queue_list_size; i++) 
    if(strcmp(data->queue_list[i]->name, queue_name) == 0) is_present = TRUE;
  
  if(!is_present)
  {
    queue* q = create_queue(queue_name); // Aloca nova fila vazia com nome queue_name
    data->queue_list_size++;
    data->queue_list[data->queue_list_size-1] = q;
    return 0;
  }

  else return 1; // Fila com esse nome já presente no servidor

}

// Servidor envia Queue.Declare-Ok para o cliente
int do_RCVD_QUEUE_DECLARE(server_data* data)
{
  int n;
  unsigned char buf[MAXSIZE];

  // Se o servidor chegou nesse estado, é porque o cliente criou uma fila. Elá está na última posição da lista de filas. Mas e se size for 0?
  char* queue_name = data->queue_list[data->queue_list_size-1]->name;
  n = queue_declare_ok(buf, queue_name);
  write(data->connfd, buf, n);

  printf("Queue.Declare-Ok enviado\n");

  return 0;
}

// Servidor recebe Channel.Close do cliente
int do_WAIT_CHANNEL_CLOSE (server_data* data)
{
  unsigned char buf[MAXSIZE];
  int bytes_read = read_frame(data->connfd, buf);

  printf("Channel.Close recebido\n");

  return 0;
}

// Servidor manda Channel.Close-Ok para o cliente
int do_RCVD_CHANNEL_CLOSE (server_data* data)
{
  int n;
  unsigned char buf[MAXSIZE];
  n = channel_close_ok(buf);
  write(data->connfd, buf, n);

  printf("Channel.Close-Ok enviado\n");

  return 0;
}

// Servidor recebe Connection.Close do cliente
int do_WAIT_CONNECTION_CLOSE (server_data* data)
{
  unsigned char buf[MAXSIZE];
  int bytes_read = read_frame(data->connfd, buf);
  
  printf("Connection.Close recebido\n");

  return 0;
}

// Servidor manda Channel.Close-Ok para o cliente
int do_RCVD_CONNECTION_CLOSE (server_data* data)
{
  int n;
  unsigned char buf[MAXSIZE];
  n = connection_close_ok(buf);
  write(data->connfd, buf, n);

  printf("Connection.Close-Ok enviado\n");

  return 0;
}

int do_FINAL (server_data* data){return 0;}
