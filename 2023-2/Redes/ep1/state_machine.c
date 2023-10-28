#include "state_machine.h"
#include "utils.h"

const unsigned char DEFAULT_PROTOCOL_HEADER[] = "\x41\x4d\x51\x50\x00\x00\x09\x01";


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

// Servidor espera o protocol header do cliente e o valida. Retorna 1 se tudo deu certo e o protocolo é válido, retorna 0 caso contrário
int do_WAIT_HEADER (client_thread* data, server_data* _server_data)
{
  read_header(data->connfd, data->buf);

  printf("Protocol Header recebido\n");

  if(memcmp(data->buf, DEFAULT_PROTOCOL_HEADER, sizeof(DEFAULT_PROTOCOL_HEADER)) == 0) return 0;
  else 
  { 
    printf("Protocolo inválido\n");
    return 1;
  }
}

// Servidor manda um Connection.Start para o cliente
int do_RCVD_HEADER (client_thread* data, server_data* _server_data)
{
    int n = connection_start(data->buf);
    write(data->connfd, data->buf, n);

    printf("Connection.Start enviado\n");

    return 0;
}

// Servidor espera um Connection.Start-Ok do cliente
int do_WAIT_START_OK (client_thread* data, server_data* _server_data)
{
  read_frame(data->connfd, data->buf);

  printf("Connection.Start-Ok recebido\n");
  return 0;
}

// Servidor manda um Connection.Tune para o cliente
int do_RCVD_START_OK (client_thread* data, server_data* _server_data)
{
  int n;
  n = connection_tune(data->buf);
  write(data->connfd, data->buf, n);

  printf("Connection.Tune enviado\n");

  return 0;
}

// Servidor espera um Connection.Tune-Ok do cliente. Como logo em seguida ele deverá receber um Connection.Open, é possível que essa ação leia os dois comandos de uma vez. Retorna 1 caso isso aconteça e 0 caso contrário
int do_WAIT_TUNE_OK (client_thread* data, server_data* _server_data)
{

  int bytes_read = read_frame(data->connfd, data->buf);

  printf("Connection.Tune-Ok recebido\n");

  int count = 0; // Contará quantos frames amqp foram enviados pelo cliente
  for(int i = 0; i < bytes_read; i++) if(data->buf[i] == FRAME_END) count++; 

  if(count == 2) 
    printf("Connection.Open recebido\n");

  return count - 1;
}

// Servidor recebe Connection.Open do cliente
int do_WAIT_CONNECTION_OPEN (client_thread* data, server_data* _server_data)
{
  int bytes_read = read_frame(data->connfd, data->buf);

  return 0;
}

// Servidor envia Connection.Open-Ok para o cliente
int do_RCVD_CONNECTION_OPEN (client_thread* data, server_data* _server_data)
{
  int n;
  n = connection_open_ok(data->buf);
  write(data->connfd, data->buf, n);

  printf("Connection.Open-Ok enviado\n");

  return 0;
}

// Servidor recebe Channel.Open do cliente
int do_WAIT_CHANNEL_OPEN (client_thread* data, server_data* _server_data)
{
  int bytes_read = read_frame(data->connfd, data->buf);

  printf("Channel.Open recebido\n");

  return 0;
}

// Servidor envia Channel.Open-Ok para o cliente
int do_RCVD_CHANNEL_OPEN(client_thread* data, server_data* _server_data)
{
  int n;
  n = channel_open_ok(data->buf);
  write(data->connfd, data->buf, n);

  printf("Channel.Open-Ok enviado\n");

  return 0;
}

// Nesse estado, o servidor pode receber um declare_queue, publish ou consume
int do_WAIT_COMMAND (client_thread* data, server_data* _server_data) {
  int bytes_read = read_frame(data->connfd, data->buf);
  data->bytes_read = bytes_read;

  unsigned short int method_id = data->buf[METHOD_ID_POSITION];
  // Faz transição interna
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
      return 3;
      break;
  }
}

// Servidor cria nova fila e envia Queue.Declare-Ok para o cliente. Os dados da fila já estão em data->buf
int do_RCVD_QUEUE_DECLARE(client_thread* data, server_data* _server_data)
{
  // Extrai nome da fila
  int queue_name_size = data->buf[QUEUE_NAME_LENGTH_OFFSET];
  char queue_name[MAXLINE];
  memcpy(queue_name, data->buf + QUEUE_NAME_LENGTH_OFFSET + 1, queue_name_size);
  queue_name[queue_name_size] = 0;

  // Cria uma fila vazia com o nome requerido. Depois verificar se a fila existe
  int is_present = FALSE;

  for(int i = 0; i < _server_data->queue_list_size; i++) 
    if(strcmp(_server_data->queue_list[i]->name, queue_name) == 0) is_present = TRUE;
  
  if(!is_present)
  {
    queue* q = create_queue(queue_name); // Aloca nova fila vazia com nome queue_name
    _server_data->queue_list_size++;
    _server_data->queue_list[_server_data->queue_list_size-1] = q;

    printf("nova fila criada, num_filas = %d\n", _server_data->queue_list_size);
    print_queue(_server_data->queue_list[_server_data->queue_list_size-1]);
    //return 0;
  }

  //else return 1; // Fila com esse nome já presente no servidor

  int n;

  n = queue_declare_ok(data->buf, queue_name);
  write(data->connfd, data->buf, n);

  printf("Queue.Declare-Ok enviado\n");

  return 0;
}

// Servidor recebeu um Basic.Publish. Escreve mensagem na fila somente. Pode ter rebebido um Channel.Close também
int do_RCVD_BASIC_PUBLISH (client_thread* data, server_data* _server_data)
{
  int n;
  char* exchange_name = "foo"; // Não usamos exchanges. Se o cliente não especificar nenhum exchange, esse campo será 0x00 no frame. Estamos pressupondo esse padrão aqui.

  // Extrai nome da fila
  char routing_key[MAXLINE];
  int routing_key_size = data->buf[ROUTING_KEY_SIZE_POSITION];
  memcpy(routing_key, data->buf + ROUTING_KEY_SIZE_POSITION + 1, routing_key_size);
  routing_key[routing_key_size] = 0;

  printf("nome da fila extraído : ");
  puts(routing_key);

  // Extrai mensagem
  int count = 0;
  int i;
  for(i = 0; i < data->bytes_read && count < 2; i++) // Extrai Method frame e o content header, o restante é o content body
  {
    if(data->buf[i] == FRAME_END) count++;   
  }

  i+=3; // Começo do length do content body
  unsigned char a,b,c,d;
  a = data->buf[i++];
  b = data->buf[i++];
  c = data->buf[i++];
  d = data->buf[i++];
  printf("a = %d\n", a);
  printf("b = %d\n", b);
  printf("c = %d\n", c);
  printf("d = %d\n", d);
  int content_body_length = d + (c << 2) + (b << 4) + (a << 6);
  printf("content length = %d\n", content_body_length);

  // Recupera mensagem
  char msg[MAX_MSG_LENGTH];
  for(int j = 0; j < content_body_length; j++) 
    msg[j] = data->buf[i+j];
  
  msg[content_body_length] = 0;

  // Como só há 8 filas, posso percorrer a lista de filas para achar a fila correspondente
  for(i = 0; i < _server_data->queue_list_size; i++)
    if(strcmp(_server_data->queue_list[i]->name, routing_key) == 0) break;

  enqueue_queue(_server_data->queue_list[i], msg);

  printf("Mensagem publicada na fila %s\n", _server_data->queue_list[i]->name);

  count = 0;
  for(i = 0; i < data->bytes_read; i++) // Extrai Method frame e o content header, o restante é o content body
  {
    if(data->buf[i] == FRAME_END) count++;   
  }

  printf("count = %d\n", count);

  if(count == 4) // Recebeu Channel.Close junto
   return 1;
  else return 0;
}

// Servidor envia Basic.Consume-Ok e Basic.Deliver
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
  char* consumer_tag = "foo_consgumer";
  n = basic_consume_ok(data->buf, consumer_tag);
  write(data->connfd, data->buf, n);

  printf("Basic.Consume-Ok enviado\n");

  // Desempilha mensagem da fila requerida
  
  // Como só há 8 filas, posso percorrer a lista de filas para achar a fila correspondente

  int i;
  for(i = 0; i < _server_data->queue_list_size; i++)
    if(strcmp(_server_data->queue_list[i]->name, routing_key) == 0) break;
  char msg[MAXLINE];
  dequeue_queue(_server_data->queue_list[i], msg);

  printf("Mensagem resgatada na fila %s\n", msg);

  // Envia o Basic.Deliver
  n = basic_deliver(data->buf, (unsigned char*) msg, strlen(msg), consumer_tag, routing_key);
  write(data->connfd, data->buf, n);

  printf("Basic.Deliver enviado\n");

  return 0; 
}

// Servidor recebe Basic.Ack do cliente
int do_WAIT_BASIC_ACK (client_thread* data, server_data* _server_data)
{

}

// Servidor recebe Channel.Close do cliente
int do_WAIT_CHANNEL_CLOSE (client_thread* data, server_data* _server_data)
{
  int bytes_read = read_frame(data->connfd, data->buf);

  printf("Channel.Close recebido\n");

  return 0;
}

// Servidor manda Channel.Close-Ok para o cliente
int do_RCVD_CHANNEL_CLOSE (client_thread* data, server_data* _server_data)
{
  int n;
  n = channel_close_ok(data->buf);
  write(data->connfd, data->buf, n);

  printf("Channel.Close-Ok enviado\n");

  return 0;
}

// Servidor recebe Connection.Close do cliente
int do_WAIT_CONNECTION_CLOSE (client_thread* data, server_data* _server_data)
{
  int bytes_read = read_frame(data->connfd, data->buf);
  
  printf("Connection.Close recebido\n");

  return 0;
}

// Servidor manda Channel.Close-Ok para o cliente
int do_RCVD_CONNECTION_CLOSE (client_thread* data, server_data* _server_data)
{
  int n;
  n = connection_close_ok(data->buf);
  write(data->connfd, data->buf, n);

  printf("Connection.Close-Ok enviado\n");

  return 0;
}

int do_FINAL (client_thread* data, server_data* _server_data){return 0;}
