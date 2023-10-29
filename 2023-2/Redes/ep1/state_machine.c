#include "state_machine.h"
#include "utils.h"
#include "hardcoded.h"

int (*actions[NUM_STATES])(client_thread*, server_data*) = {
  do_WAIT_HEADER,
  do_RCVD_HEADER, 
  do_WAIT_START_OK,
  do_RCVD_START_OK,
  do_WAIT_TUNE_OK, 
  do_WAIT_CONNECTION_OPEN,
  do_RCVD_CONNECTION_OPEN, 
  do_WAIT_CHANNEL_OPEN, 
  do_RCVD_CHANNEL_OPEN, 
  do_WAIT_COMMAND,
  do_RCVD_QUEUE_DECLARE, 
  do_RCVD_BASIC_PUBLISH,
  do_RCVD_BASIC_CONSUME,
  do_WAIT_BASIC_ACK,
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
  {WAIT_COMMAND}, // RCVD_CHANNEL_OPEN, 
  {RCVD_QUEUE_DECLARE, RCVD_BASIC_PUBLISH, RCVD_BASIC_CONSUME}, // WAIT_COMMAND, 
  {WAIT_CHANNEL_CLOSE}, // RCVD_QUEUE_DECLARE, 
  {WAIT_CHANNEL_CLOSE, RCVD_CONNECTION_CLOSE}, // RCVD_BASIC_PUBLISH, 
  {WAIT_BASIC_ACK}, // RCVD_BASIC_CONSUME, 
  {FINAL}, // WAIT_BASIC_ACK, 
  {RCVD_CHANNEL_CLOSE}, // WAIT_CHANNEL_CLOSE, 
  {WAIT_CONNECTION_CLOSE}, // RCVD_CHANNEL_CLOSE, 
  {RCVD_CONNECTION_CLOSE}, // WAIT_CONNECTION_CLOSE,
  {FINAL}, // RCVD_CONNECTION_CLOSE
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
  "WAIT_COMMAND", 
  "RCVD_QUEUE_DECLARE", 
  "RCVD_BASIC_PUBLISH", 
  "RCVD_BASIC_CONSUME", 
  "WAIT_BASIC_ACK", 
  "WAIT_CHANNEL_CLOSE", 
  "RCVD_CHANNEL_CLOSE", 
  "WAIT_CONNECTION_CLOSE",
  "RCVD_CONNECTION_CLOSE",
  "FINAL"
};

// Servidor bloqueia até receber o protocol header do cliente. Não valida o protocolo.

int do_WAIT_HEADER (client_thread* data, server_data* _server_data)
{
  read_header(data->connfd, data->buf);

  printf("Protocol Header recebido\n");

  return 0;
}

// Servidor manda um Connection.Start para o cliente. HARDCODED.

int do_RCVD_HEADER (client_thread* data, server_data* _server_data)
{
    write(data->connfd, CONNECTION_START_FRAME, CONNECTION_START_FRAME_SIZE);

    printf("Connection.Start enviado\n");

    return 0;
}

// Servidor bloqueia até receber um Connection.Start-Ok do cliente. Não valida.

int do_WAIT_START_OK (client_thread* data, server_data* _server_data)
{
  read_frame(data->connfd, data->buf);

  printf("Connection.Start-Ok recebido\n");
  return 0;
}

// Servidor manda um Connection.Tune para o cliente. HARDCODED.

int do_RCVD_START_OK (client_thread* data, server_data* _server_data)
{
  write(data->connfd, CONNECTION_TUNE_FRAME, CONNECTION_TUNE_FRAME_SIZE);

  printf("Connection.Tune enviado\n");

  return 0;
}

// Servidor bloqueia até receber um Connection.Tune-Ok do cliente. Não valida. Como logo em seguida ele deverá receber um Connection.Open, é possível que o socketcontenha os dois. Retorna 1 nesse caso e 0 caso contrário. 

int do_WAIT_TUNE_OK (client_thread* data, server_data* _server_data)
{

  int bytes_read = read_frame(data->connfd, data->buf);

  printf("Connection.Tune-Ok recebido\n");

  int count = 0; // Quantos frames amqp foram enviados pelo cliente ?
  for(int i = 0; i < bytes_read; i++) if(data->buf[i] == FRAME_END) count++; 

  if(count == 2) 
    printf("Connection.Open recebido\n");

  return count - 1;
}

// Servidor bloqueia até receber um Connection.Open do cliente. Não valida.

int do_WAIT_CONNECTION_OPEN (client_thread* data, server_data* _server_data)
{
  int bytes_read = read_frame(data->connfd, data->buf);

  return 0;
}

// Servidor envia Connection.Open-Ok para o cliente. HARDCODED.

int do_RCVD_CONNECTION_OPEN (client_thread* data, server_data* _server_data)
{
  write(data->connfd, CONNECTION_OPEN_OK_FRAME, CONNECTION_OPEN_OK_FRAME_SIZE);

  printf("Connection.Open-Ok enviado\n");

  return 0;
}

// Servidor bloqueia até receber um Channel.Open do cliente. Não valida.

int do_WAIT_CHANNEL_OPEN (client_thread* data, server_data* _server_data)
{
  int bytes_read = read_frame(data->connfd, data->buf);

  printf("Channel.Open recebido\n");

  return 0;
}

// Servidor envia Channel.Open-Ok para o cliente. HARDCODED.

int do_RCVD_CHANNEL_OPEN (client_thread* data, server_data* _server_data)
{
  write(data->connfd, CHANNEL_OPEN_OK_FRAME, CHANNEL_OPEN_OK_FRAME_SIZE);

  printf("Channel.Open-Ok enviado\n");

  return 0;
}

// Nesse estado, o servidor bloqueia até receber um declare_queue, publish ou consume. Não trata outros comandos. 

int do_WAIT_COMMAND (client_thread* data, server_data* _server_data) 
{
  int bytes_read = read_frame(data->connfd, data->buf);
  data->bytes_read = bytes_read;

  // Extrai método do frame
  unsigned short int method_id = data->buf[METHOD_ID_POSITION]; 

  switch (method_id) 
  {
    case DECLARE_ID:
      printf("Queue.Declare recebido\n");
      return 0;
      break;

    case PUBLISH_ID:
      printf("Basic.Publish recebido\n");
      return 1;
      break;

    case CONSUME_ID:
      printf("Basic.Consume recebido\n");
      return 2;
      break;

    default:
      return 3; // O servidor deve morrer aqui. Não há tratamento de erros nesse EP
      break;
  }
}

// Servidor cria nova fila e envia Queue.Declare-Ok para o cliente. Se o servidor está nesse estado, os dados da fila estão em data->buf devido ao comando Queue.Declare enviado pelo cliente

int do_RCVD_QUEUE_DECLARE (client_thread* data, server_data* _server_data)
{
  // Extrai nome da fila
  int queue_name_size = data->buf[QUEUE_NAME_LENGTH_OFFSET];
  char queue_name[MAXLINE];
  memcpy(queue_name, data->buf + QUEUE_NAME_LENGTH_OFFSET + 1, queue_name_size);
  queue_name[queue_name_size] = 0;

  // Verifica se a fila já está presente. Como teremos no máximo 8 filas a qualquer instante, é razoável procurar por filas pelo seu nome

  int is_present = FALSE;
  for(int i = 0; i < _server_data->queue_list_size; i++) 
    if(strcmp(_server_data->queue_list[i]->name, queue_name) == 0) is_present = TRUE;
  
  if(!is_present)
  {
    queue* q = create_queue(queue_name); // Aloca nova fila vazia com nome queue_name
    _server_data->queue_list_size++;
    _server_data->queue_list[_server_data->queue_list_size-1] = q;

    printf("nova fila criada, num_filas = %d\n", _server_data->queue_list_size);
  }
  
  // Envia Queue.Declare-Ok para o cliente

  int n = queue_declare_ok(data->buf, queue_name);
  write(data->connfd, data->buf, n);

  printf("Queue.Declare-Ok enviado\n");

  return 0;
}

// Servidor recebeu um Basic.Publish. Escreve mensagem na fila somente. Pode ter rebebido um Channel.Close também. Não trata erros do tipo, fila inexistente, etc.Outro ponto importante é que aqui assumimos que a mensagem será enviada em somente um Content Body. Isso é verdade para o limite do tamanho das mensagens estabelecido. Mas um programa mais robusto deveria verificar isso.

int do_RCVD_BASIC_PUBLISH (client_thread* data, server_data* _server_data)
{
  int n;
  char* exchange_name = "foo"; // Não usamos exchanges. Se o cliente não especificar nenhum exchange, esse campo será 0x00 no frame. Estamos pressupondo isso aqui.

  // Extrai nome da fila 
  char routing_key[MAXLINE];
  int routing_key_size = data->buf[ROUTING_KEY_SIZE_POSITION];
  memcpy(routing_key, data->buf + ROUTING_KEY_SIZE_POSITION + 1, routing_key_size);
  routing_key[routing_key_size] = 0;

  printf("nome da fila extraído : ");
  puts(routing_key);

  // Extrai mensagem

  int count = 0;

  // Pula o Method frame e o Content header, o restante é o Content body
  int i;
  for(i = 0; i < data->bytes_read && count < 2; i++) 
    if(data->buf[i] == FRAME_END) count++;   
  
  i+=3; // i aponta para o começo do campo length do Content body

  // Extrai tamanho do Content body

  int content_body_length = 0;
  content_body_length += data->buf[i++] << 6;
  content_body_length += data->buf[i++] << 4;
  content_body_length += data->buf[i++] << 2;
  content_body_length += data->buf[i++];

  // Recupera mensagem
  char msg[MAX_MSG_LENGTH];
  memcpy(msg, data->buf + i, content_body_length);
  msg[content_body_length] = 0;

  for(i = 0; i < _server_data->queue_list_size; i++)
    if(strcmp(_server_data->queue_list[i]->name, routing_key) == 0) break;

  enqueue_queue(_server_data->queue_list[i], msg);

  printf("Mensagem publicada na fila %s\n", _server_data->queue_list[i]->name);

  // Agora, vamos verificar se o Channel.Close foi lido junto no socket. Supomos que o Basic.Publish veio em 3 frames
  count = 0;
  for(i = 0; i < data->bytes_read; i++)
    if(data->buf[i] == FRAME_END) count++;   

  if(count == 4) // Leu Channel.Close junto
   return 1;
  else return 0;
}

// Servidor envia Basic.Consume-Ok e Basic.Deliver com a mensagem a ser consumida pelo cliente.
int do_RCVD_BASIC_CONSUME (client_thread* data, server_data* _server_data)
{
  // Extrai nome da fila a partir do pacote do cliente Basic.Consume
  char routing_key[MAXLINE];
  int routing_key_size = data->buf[13];
  memcpy(routing_key, data->buf + 13 + 1, routing_key_size);
  routing_key[routing_key_size] = 0;

  printf("Nome da fila extraído : ");
  puts(routing_key);

  // Envia o Basic.Consume-Ok
  int n;
  char* consumer_tag = "foo_consumer";
  n = basic_consume_ok(data->buf, consumer_tag);
  write(data->connfd, data->buf, n);

  printf("Basic.Consume-Ok enviado\n");

  // Desempilha mensagem da fila
  
  int i;
  for(i = 0; i < _server_data->queue_list_size; i++)
    if(strcmp(_server_data->queue_list[i]->name, routing_key) == 0) break;
  char msg[MAXLINE];
  dequeue_queue(_server_data->queue_list[i], msg);

  // Envia o Basic.Deliver
  n = basic_deliver(data->buf, (unsigned char*) msg, strlen(msg), consumer_tag, routing_key);
  write(data->connfd, data->buf, n);

  printf("Basic.Deliver enviado\n");

  return 0; 
}

// Servidor bloqueia até receber um Basic.Ack do cliente. Não valida.
int do_WAIT_BASIC_ACK (client_thread* data, server_data* _server_data)
{
  int bytes_read = read_frame(data->connfd, data->buf);

  printf("Basic.Ack recebido\n");

  return 0;
}

// Servidor bloqueia até receber um Channel.Close do cliente. Não valida.
int do_WAIT_CHANNEL_CLOSE (client_thread* data, server_data* _server_data)
{
  int bytes_read = read_frame(data->connfd, data->buf);

  printf("Channel.Close recebido\n");

  return 0;
}

// Servidor manda Channel.Close-Ok para o cliente. HARDCODED.
int do_RCVD_CHANNEL_CLOSE (client_thread* data, server_data* _server_data)
{
  write(data->connfd, CHANNEL_CLOSE_OK_FRAME, CHANNEL_CLOSE_OK_FRAME_SIZE);

  printf("Channel.Close-Ok enviado\n");

  return 0;
}

// Servidor bloqueia até receber um Connection.Close do cliente. Não valida.
int do_WAIT_CONNECTION_CLOSE (client_thread* data, server_data* _server_data)
{
  int bytes_read = read_frame(data->connfd, data->buf);
  
  printf("Connection.Close recebido\n");

  return 0;
}

// Servidor manda Connection.Close-Ok para o cliente. HARDCODED.
int do_RCVD_CONNECTION_CLOSE (client_thread* data, server_data* _server_data)
{
  write(data->connfd, CONNECTION_CLOSE_OK_FRAME, CONNECTION_CLOSE_OK_FRAME_SIZE);

  printf("Connection.Close-Ok enviado\n");

  return 0;
}

int do_FINAL (client_thread* data, server_data* _server_data)
{
  return 0;
}
