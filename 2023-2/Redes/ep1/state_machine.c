#include "state_machine.h"
#include "utils.h"

const unsigned char DEFAULT_PROTOCOL_HEADER[] = "\x41\x4d\x51\x50\x00\x00\x09\x01";

// Estados da maquina de estados do servidor
typedef enum {
  WAIT_HEADER,
  RCVD_HEADER,
  WAIT_START_OK,
  RCVD_START_OK
} state;

// Servidor espera o protocol header do cliente e o valida. Retorna 1 se tudo deu certo e o protocolo é válido, retorna 0 caso contrário
int do_WAIT_HEADER (int connfd)
{
  unsigned char buf[MAXSIZE];
  int n;
  buf[0] = 0;
  int offset = 0;
  int bytes_read = 0;
  unsigned char last_byte_read = 0;

  while (bytes_read != 8) {
    n = read(connfd, buf + offset, MAXLINE); 
    bytes_read += n;
    offset += n;
    last_byte_read = buf[offset-1];
  }

  printf("Protocol Header recebido\n");

  if(memcmp(buf, DEFAULT_PROTOCOL_HEADER, sizeof(DEFAULT_PROTOCOL_HEADER)) == 0) return 1;
  else 
  { 
    printf("Protocolo inválido\n");
    return 0;
  }
}

// Servidor manda um Connection.Start para o cliente
int do_RCVD_HEADER (int connfd)
{
    unsigned char connection_start_frame[MAXSIZE];
    int n = connection_start(connection_start_frame);
    write(connfd, connection_start_frame, n);

    printf("Connection.Start enviado\n");

    return 1;
}

// Servidor espera um Connection.Start-Ok do cliente
int do_WAIT_START_OK (int connfd)
{
  int n;
  unsigned char connection_start_ok_frame[MAXSIZE]; 
  connection_start_ok_frame[0] = 0;
  int offset = 0;
  int bytes_read = 0;
  int last_byte_read = 0;

  while (last_byte_read != FRAME_END) {
    n = read(connfd, connection_start_ok_frame + offset, MAXLINE); 
    bytes_read += n;
    offset += n;
    last_byte_read = connection_start_ok_frame[offset-1];
  }
  printf("Connection.Start-Ok recebido\n");
  return 1;
}

// Servidor manda um Connection.Tune para o cliente
int do_RCVD_START_OK (int connfd)
{
  int n;
  unsigned char connection_tune_frame[MAXSIZE];
  n = connection_tune(connection_tune_frame);
  write(connfd, connection_tune_frame, n);

  printf("Connection.Tune enviado\n");

  return 1;
}

// Servidor espera um Connection.Tune-Ok do cliente. Como logo em seguida ele deverá receber um Connection.Open, é possível que essa ação leia os dois comandos de uma vez. Retorna 2 caso isso aconteça e 1 caso contrário
int do_WAIT_TUNE_OK (int connfd)
{
  int n;
  unsigned char connection_tune_ok_frame[MAXSIZE]; 
  connection_tune_ok_frame[0] = 0;
  int offset = 0;
  int bytes_read = 0;
  int last_byte_read = 0;

  while (last_byte_read != FRAME_END) {
    n = read(connfd, connection_tune_ok_frame + offset, MAXLINE); 
    bytes_read += n;
    offset += n;
    last_byte_read = connection_tune_ok_frame[offset-1];
  }

  printf("Connection.Tune-Ok recebido\n");

  int count = 0; // Contará quantos frames amqp foram enviados pelo cliente
  for(int i = 0; i < bytes_read; i++) if(connection_tune_ok_frame[i] == FRAME_END) count++; 

  if(count == 2) 
    printf("Connection.Open recebido\n");

  return count;
}

// Servidor recebe Connection.Open do cliente
int do_WAIT_CONNECTION_OPEN (int connfd)
{
  int n;
  unsigned char connection_open_frame[MAXSIZE]; 
  connection_open_frame[0] = 0;
  int offset = 0;
  int bytes_read = 0;
  int last_byte_read = 0;

  while (last_byte_read != FRAME_END) {
    n = read(connfd, connection_open_frame + offset, MAXLINE); 
    bytes_read += n;
    offset += n;
    last_byte_read = connection_open_frame[offset-1];
  }

  return 1;
}

// Servidor envia Connection.Open-Ok para o cliente
int do_RCVD_CONNECTION_OPEN (int connfd)
{
  int n;
  unsigned char connection_open_ok_frame[MAXSIZE];
  n = connection_open_ok(connection_open_ok_frame);
  write(connfd, connection_open_ok_frame, n);

  printf("Connection.Open-Ok enviado\n");

  return 1;
}

// Servidor recebe Channel.Open do cliente
int do_WAIT_CHANNEL_OPEN (int connfd)
{
  int n;
  unsigned char channel_open_frame[MAXSIZE]; 
  channel_open_frame[0] = 0;
  int offset = 0;
  int bytes_read = 0;
  int last_byte_read = 0;

  while (last_byte_read != FRAME_END) {
    n = read(connfd, channel_open_frame + offset, MAXLINE); 
    bytes_read += n;
    offset += n;
    last_byte_read = channel_open_frame[offset-1];
  }

  printf("Channel.Open recebido\n");

  return 1;
}

// Servidor envia Channel.Open-Ok para o cliente
int do_RCVD_CHANNEL_OPEN(int connfd)
{
  int n;
  unsigned char channel_open_ok_frame[MAXSIZE];
  n = channel_open_ok(channel_open_ok_frame);
  write(connfd, channel_open_ok_frame, n);

  printf("Channel.Open-Ok enviado\n");

  return 1;
}

// Servidor recebe Queue.Declare do cliente e faz a criação da nova fila. Retorna a quantidade de filas criadas
int do_WAIT_QUEUE_DECLARE (int connfd, queue* queue_list[], int tam)
{
  int n;
  unsigned char queue_declare_frame[MAXSIZE]; 
  queue_declare_frame[0] = 0;
  int offset = 0;
  int bytes_read = 0;
  int last_byte_read = 0;

  while (last_byte_read != FRAME_END) {
    n = read(connfd, queue_declare_frame + offset, MAXLINE); 
    bytes_read += n;
    offset += n;
    last_byte_read = queue_declare_frame[offset-1];
  }

  printf("Queue.Declare recebido\n");

  // Extrai nome da fila
  int queue_name_size = queue_declare_frame[QUEUE_NAME_LENGTH_OFFSET];
  char queue_name[MAXLINE];
  memcpy(queue_name, queue_declare_frame + QUEUE_NAME_LENGTH_OFFSET + 1, queue_name_size);
  queue_name[queue_name_size] = 0;

  // Cria uma fila vazia com o nome requerido. Depois verificar se a fila existe
  int is_present = FALSE;

  for(int i = 0; i < tam; i++) 
    if(strcmp(queue_list[i]->name, queue_name) == 0) is_present = TRUE;
  
  if(!is_present)
  {
    queue* q = create_queue(queue_name); // Aloca nova fila vazia com nome queue_name
    queue_list[tam] = q;
    return 1;
  }

  else return 0; // Fila com esse nome já presente no servidor

}

// Servidor envia Queue.Declare-Ok para o cliente
int do_RCVD_QUEUE_DECLARE(int connfd, char* queue_name)
{
  int n;
  unsigned char queue_declare_ok_frame[MAXSIZE];
  n = queue_declare_ok(queue_declare_ok_frame, queue_name);
  write(connfd, queue_declare_ok_frame, n);

  printf("Queue.Declare-Ok enviado\n");

  return 1;
}

// Servidor recebe Channel.Close do cliente
int do_WAIT_CHANNEL_CLOSE (int connfd)
{
  int n;
  unsigned char channel_close_frame[MAXSIZE]; 
  channel_close_frame[0] = 0;
  int offset = 0;
  int bytes_read = 0;
  int last_byte_read = 0;

  while (last_byte_read != FRAME_END) {
    n = read(connfd, channel_close_frame + offset, MAXLINE); 
    bytes_read += n;
    offset += n;
    last_byte_read = channel_close_frame[offset-1];
  }

  printf("Channel.Close recebido\n");

  return 1;
}

// Servidor manda Channel.Close-Ok para o cliente
int do_RCVD_CHANNEL_CLOSE (int connfd)
{
  int n;
  unsigned char channel_close_ok_frame[MAXSIZE];
  n = channel_close_ok(channel_close_ok_frame);
  write(connfd, channel_close_ok_frame, n);

  printf("Channel.Close-Ok enviado\n");

  return 1;
}

// Servidor recebe Connection.Close do cliente
int do_WAIT_CONNECTION_CLOSE (int connfd)
{
  int n;
  unsigned char connection_close_frame[MAXSIZE]; 
  connection_close_frame[0] = 0;
  int offset = 0;
  int bytes_read = 0;
  int last_byte_read = 0;

  while (last_byte_read != FRAME_END) {
    n = read(connfd, connection_close_frame + offset, MAXLINE); 
    bytes_read += n;
    offset += n;
    last_byte_read = connection_close_frame[offset-1];
  }

  printf("Connection.Close recebido\n");

  return 1;
}

// Servidor manda Channel.Close-Ok para o cliente
int do_RCVD_CONNECTION_CLOSE (int connfd)
{
  int n;
  unsigned char connection_close_ok_frame[MAXSIZE];
  n = connection_close_ok(connection_close_ok_frame);
  write(connfd, connection_close_ok_frame, n);

  printf("Connection.Close-Ok enviado\n");

  return 1;
}



















