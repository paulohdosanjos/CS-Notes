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

#define LISTENQ 1
#define MAXDATASIZE 100
#define MAXLINE 4096

#define MAXSIZE 10000
#define PROTOCOL_HEADER_SIZE 8
#define FRAME_END 0xce

char* parse_protoocol_header(const char*);

// Faz o parse do protocol header enviado pelo cliente. Tamanho de header deve ser 8
void parse_protocol_header (unsigned char* header, unsigned char* buf)
{
  memcpy(header, buf, 4); // Copia 'AMQP' para header. Não verifica erro

  // Escreve os bytes literais no header
  header[4] = (int) '0' + buf[4];
  header[5] = (int) '0' + buf[5];
  header[6] = (int) '0' + buf[6];
  header[7] = (int) '0' + buf[7];

  //header[8] = '\0';
}

// Recebe uma string s e dá o "payload" correspondente
unsigned char* to_longstr(char* value)
{
  unsigned long int n = strlen(value);
  unsigned char* s = (unsigned char*) malloc((n + 4) * sizeof (unsigned char)); 
  s[3] = n & 0xFF; 
  n >>= 8;
  s[2] = n & 0xFF; 
  n >>= 8;
  s[1] = n & 0xFF;
  n >>= 8;
  s[0] = n & 0xFF; 

  for(int i = 0; i < strlen(value); i++) 
    s[i+4] = value[i];

  return s;
}

// Recebe uma string s e dá o "payload" correspondente
unsigned char* to_shortstr(char* value)
{
  unsigned char n = strlen(value);
  unsigned char* s = (unsigned char*) malloc((n + 1) * sizeof (unsigned char)); 
  s[0] = n; 

  for(int i = 0; i < strlen(value); i++) 
    s[i+1] = value[i];

  return s;
}

// Escreve o short int n na posição i do frame, ele ocupará dois bytes
void write_short_int(unsigned char* frame, int i, unsigned short int n)
{
  frame[i+1] = n & 0xFF; 
  n >>= 8;
  frame[i] = n & 0xFF; 
}

// Escreve o long int n na posição i do frame, ele ocupará dois bytes
void write_long_int(unsigned char* frame, int i, unsigned long int n)
{
  frame[i+3] = n & 0xFF; 
  n >>= 8;
  frame[i+2] = n & 0xFF; 
  n >>= 8;
  frame[i+1] = n & 0xFF; 
  n >>= 8;
  frame[i] = n & 0xFF; 
}

// Escreve n bytes de buffer a partir da posição i do frame
void write_stream(unsigned char* frame, unsigned char* buffer, int i, int n)
{
  for(int j = 0; j < n; j++) {
    frame[i+j] = buffer[j];
    //printf("frame[%d] = %c, %02x\n", i+j, frame[i+j], frame[i+j]);
  }
}

unsigned char* concatenate(unsigned char* buffer1, int n1, unsigned char* buffer2, int n2)
{
  unsigned char* s = (unsigned char*) malloc((n1+n2) * sizeof(unsigned char));
  for(int i = 0; i < n1; i++) s[i] = buffer1[i];
  for(int i = 0; i < n2; i++) s[i] = buffer2[n1 + i];

  return s;
}

int atribute_S_size(char* name, char* value)
{
  return 1 + strlen(name) + 1 + 4 + strlen(value);
}

void get_S_atribute(unsigned char* a_buffer, char* name, char* value)
{
  unsigned char* _name = to_shortstr(name);
  unsigned char* _value = to_longstr(value);

  int k = 0;
  for(int i = 0; i < 1 + strlen(name); i++) a_buffer[k++] = _name[i];

  a_buffer[k++] = 'S';

  for(int i = 0; i < 4 + strlen(value); i++) a_buffer[k++] = _value[i];
 
  free(_name);
  free(_value);
}

int atribute_t_size(char* name)
{
  return 1 + strlen(name) + 1 + 1;
}


void get_t_atribute(unsigned char* a_buffer, char* name, int value)
{
  unsigned char* _name = to_shortstr(name);

  int k = 0;
  for(int i = 0; i < 1 + strlen(name); i++) a_buffer[k++] = _name[i];

  a_buffer[k++] = 't';

  a_buffer[k++] = value;

  free(_name);
}

int atribute_s_size(char* name, char* value)
{
  return 1 + strlen(name) + 1 + 1 + strlen(value);
}

void get_s_atribute(unsigned char* a_buffer, char* name, char* value)
{
  unsigned char* _name = to_shortstr(name);
  unsigned char* _value = to_longstr(value);

  int k = 0;
  for(int i = 0; i < 1 + strlen(name); i++) a_buffer[k++] = _name[i];

  a_buffer[k++] = 's';

  for(int i = 0; i < 1 + strlen(value); i++) a_buffer[k++] = _value[i];
 
  free(_name);
  free(_value);
}

int write_S_atribute(unsigned char* table, char* a_name, char* a_value, int offset)
{
  int a_size = atribute_S_size(a_name, a_value);
  unsigned char* a_buffer = (unsigned char*) malloc(a_size * sizeof(unsigned char));
  get_S_atribute(a_buffer, a_name, a_value);
  write_stream(table, a_buffer, offset, a_size);

  //for(int i = offset; i < offset + a_size; i++) printf("%02x ", table[i]);
  //printf("\n********************************************************\n");

  free(a_buffer);
  return a_size;
}

int write_t_atribute(unsigned char* table, char* a_name, int a_value, int offset)
{
  int a_size = atribute_t_size(a_name);
  unsigned char* a_buffer = (unsigned char*) malloc(a_size * sizeof(unsigned char));
  get_t_atribute(a_buffer, a_name, a_value);
  write_stream(table, a_buffer, offset, a_size);

  //for(int i = offset; i < offset + a_size; i++) printf("%02x ", table[i]);
  //printf("\n********************************************************\n");

  free(a_buffer);
  return a_size;
}

int write_s_atribute(unsigned char* table, char* a_name, char* a_value, int offset)
{
  int a_size = atribute_s_size(a_name, a_value);
  unsigned char* a_buffer = (unsigned char*) malloc(a_size * sizeof(unsigned char));
  get_S_atribute(a_buffer, a_name, a_value);
  write_stream(table, a_buffer, offset, a_size);

  for(int i = offset; i < offset + a_size; i++) printf("%02x ", table[i]);
  printf("\n********************************************************\n");

  free(a_buffer);
  return a_size;
}

int connection_start (unsigned char* frame)
{
  //unsigned char frame[MAXSIZE];
  unsigned long int frame_length = 0;
  int frame_offset = 0;

  // General Frame
  unsigned char type = 0x1; // Method frame
  unsigned short int channel = 0x0;
  unsigned long int length = 499;

  frame[frame_offset] = type;
  frame_offset += 1;
  frame_length += 1;

  write_short_int(frame, frame_offset, channel);
  frame_offset += 2;
  frame_length += 2;


  // Method frame
  unsigned char method_frame[MAXSIZE];
  unsigned long int method_frame_length = 0;
  int method_frame_offset = 0;

  unsigned short int class_id = 10; // Class Connection
  unsigned short int method_id = 10; // Method Start
  
  write_short_int(method_frame, method_frame_offset, class_id);
  method_frame_offset += 2;
  method_frame_length += 2;

  write_short_int(method_frame, method_frame_offset, method_id);
  method_frame_offset += 2;
  method_frame_length += 2;

  // Argumentos
  unsigned char arguments[MAXSIZE];
  unsigned long int arguments_length = 0;
  int arguments_offset = 0;

  unsigned char version_major = 0;
  unsigned char version_minor = 9;

  arguments[arguments_offset] = version_major;
  arguments_offset += 1;
  arguments_length += 1;

  arguments[arguments_offset] = version_minor;
  arguments_offset += 1;
  arguments_length += 1;

  // Escrevendo a tabela server-properties ********************************************************************
  unsigned char global_table[MAXSIZE];
  unsigned long int global_table_length = 0;
  int global_offset = 4;

  // Atributos
  char* a_name;
  char* a_value;
  int a_size;
  unsigned char* a_buffer;

  unsigned char* _name = to_shortstr("capabilities");
  write_stream(global_table, _name, global_offset, 1 + strlen("capabilities"));
  global_offset += 1 + strlen("capabilities");
  global_table_length += 1 + strlen("capabilities");

  global_table[global_offset] = 'F';
  global_offset += 1; 
  global_table_length += 1;

  // Escrevendo a tabela capabilities ***********************************************************************************************
  unsigned char table[MAXSIZE];
  unsigned long int table_length = 0;
  int offset = 4;

  char* name_list[] = {"publisher_confirms", "exchange_exchange_bindings", "basic.nack", "consumer_cancel_notify", "connection.blocked", "consumer.priorities", "authentication_failure_close", "per_consumer_qos", "direct_reply_to"};
  int value_list[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};

  int n = (int) (sizeof(name_list) / sizeof(name_list[0]));

  for(int i = 0; i < n; i++)
  {
    a_size = write_t_atribute(table, name_list[i], value_list[i], offset);
    offset += a_size;
    table_length += a_size;
  }

  write_long_int(table, 0, table_length);
  table_length += 4;

  // Finalizada a escrita da tabela capabilities ***********************************************************************************************

  write_stream(global_table, table, global_offset, table_length);
  global_offset += table_length;
  global_table_length += table_length;

  char* name_list1[] = {"cluster_name", "copyright", "information", "platform", "product", "version"};
  char* value_list1[] = {"rabbit@rmq", "Copyright (c) 2007-2023 VMware, Inc. or its affiliates.", "Licensed under the MPL 2.0. Website: https://rabbitmq.com", "Erlang/OTP 25.3.2.7", "RabbitMQ", "3.12.7"};

  n = (int) (sizeof(name_list1) / sizeof(name_list1[0]));

  for(int i = 0; i < n; i++)
  {
    a_size = write_S_atribute(global_table, name_list1[i], value_list1[i], global_offset);
    global_offset += a_size;
    global_table_length += a_size;
  }

  write_long_int(global_table, 0, global_table_length);
  global_table_length += 4;

  // Finalizada a escrita da tabela server-properties ********************************************************************
  
  write_stream(arguments, global_table, arguments_offset, global_table_length);
  arguments_offset += global_table_length;
  arguments_length += global_table_length;

  unsigned char* mechanisms = to_longstr("PLAIN AMQPLAIN");
  unsigned char* locales = to_longstr("en_US");

  int h = 4 + strlen("PLAIN AMQPLAIN");
  write_stream(arguments, mechanisms, arguments_offset, h);
  arguments_offset += h;
  arguments_length += h;

  h = 4 + strlen("en_US");
  write_stream(arguments, locales, arguments_offset, h);
  arguments_offset += h;
  arguments_length += h;

  write_stream(method_frame, arguments, method_frame_offset, arguments_length);
  method_frame_offset += arguments_length;
  method_frame_length += arguments_length;

  write_long_int(frame, frame_offset, method_frame_length);
  frame_offset += 4;
  frame_length += 4;

  write_stream(frame, method_frame, frame_offset, method_frame_length);
  frame_offset += method_frame_length;
  frame_length += method_frame_length;

  frame[frame_offset] = 0xce;
  frame_offset += 1;
  frame_length += 1;

  //for(int i = 0; i < frame_length; i++) printf("%02x", frame[i]);
  //printf("\n");

  return frame_length;
}

int connection_tune (unsigned char* frame)
{
  unsigned long int frame_length = 0;
  int frame_offset = 0;

  // General Frame
  unsigned char type = 0x1; // Method frame
  unsigned short int channel = 0x0;

  frame[frame_offset] = type;
  frame_offset += 1;
  frame_length += 1;

  write_short_int(frame, frame_offset, channel);
  frame_offset += 2;
  frame_length += 2;


  // Method frame
  unsigned char method_frame[MAXSIZE];
  unsigned long int method_frame_length = 0;
  int method_frame_offset = 0;

  unsigned short int class_id = 10; // Class Connection
  unsigned short int method_id = 30; // Method Tune 
  
  write_short_int(method_frame, method_frame_offset, class_id);
  method_frame_offset += 2;
  method_frame_length += 2;

  write_short_int(method_frame, method_frame_offset, method_id);
  method_frame_offset += 2;
  method_frame_length += 2;

  // Argumentos
  unsigned char arguments[MAXSIZE];
  unsigned long int arguments_length = 0;
  int arguments_offset = 0;

  unsigned short int channel_max = 2047;
  unsigned long int frame_max = 131072;
  unsigned short int heartbeat = 60;

  write_short_int(arguments, arguments_offset, channel_max); 
  arguments_offset += 2;
  arguments_length += 2;

  write_long_int(arguments, arguments_offset, frame_max); 
  arguments_offset += 4;
  arguments_length += 4;

  write_short_int(arguments, arguments_offset, heartbeat); 
  arguments_offset += 2;
  arguments_length += 2;

  write_stream(method_frame, arguments, method_frame_offset, arguments_length);
  method_frame_offset += arguments_length;
  method_frame_length += arguments_length;

  write_long_int(frame, frame_offset, method_frame_length);
  frame_offset += 4;
  frame_length += 4;

  write_stream(frame, method_frame, frame_offset, method_frame_length);
  frame_offset += method_frame_length;
  frame_length += method_frame_length;

  frame[frame_offset] = 0xce;
  frame_offset += 1;
  frame_length += 1;

  return frame_length;
}

int connection_open_ok (unsigned char* frame)
{
  unsigned long int frame_length = 0;
  int frame_offset = 0;

  // General Frame
  unsigned char type = 0x1; // Method frame
  unsigned short int channel = 0x0;

  frame[frame_offset] = type;
  frame_offset += 1;
  frame_length += 1;

  write_short_int(frame, frame_offset, channel);
  frame_offset += 2;
  frame_length += 2;

  // Method frame
  unsigned char method_frame[MAXSIZE];
  unsigned long int method_frame_length = 0;
  int method_frame_offset = 0;

  unsigned short int class_id = 10; // Class Connection
  unsigned short int method_id = 41; // Method Open-Ok
  
  write_short_int(method_frame, method_frame_offset, class_id);
  method_frame_offset += 2;
  method_frame_length += 2;

  write_short_int(method_frame, method_frame_offset, method_id);
  method_frame_offset += 2;
  method_frame_length += 2;

  // Argumentos
  unsigned char arguments[MAXSIZE];
  unsigned long int arguments_length = 0;
  int arguments_offset = 0;

  unsigned char reserved_1 = 0x00;

  arguments[arguments_offset] = reserved_1;
  arguments_offset += 1;
  arguments_length += 1;

  write_stream(method_frame, arguments, method_frame_offset, arguments_length);
  method_frame_offset += arguments_length;
  method_frame_length += arguments_length;

  write_long_int(frame, frame_offset, method_frame_length);
  frame_offset += 4;
  frame_length += 4;

  write_stream(frame, method_frame, frame_offset, method_frame_length);
  frame_offset += method_frame_length;
  frame_length += method_frame_length;

  frame[frame_offset] = 0xce;
  frame_offset += 1;
  frame_length += 1;

  return frame_length;
}

int channel_open_ok (unsigned char* frame)
{
  unsigned long int frame_length = 0;
  int frame_offset = 0;

  // General Frame
  unsigned char type = 0x1; // Method frame
  unsigned short int channel = 0x1;

  frame[frame_offset] = type;
  frame_offset += 1;
  frame_length += 1;

  write_short_int(frame, frame_offset, channel);
  frame_offset += 2;
  frame_length += 2;

  // Method frame
  unsigned char method_frame[MAXSIZE];
  unsigned long int method_frame_length = 0;
  int method_frame_offset = 0;

  unsigned short int class_id = 20; // Class channel
  unsigned short int method_id = 11; // Method Open-Ok
  
  write_short_int(method_frame, method_frame_offset, class_id);
  method_frame_offset += 2;
  method_frame_length += 2;

  write_short_int(method_frame, method_frame_offset, method_id);
  method_frame_offset += 2;
  method_frame_length += 2;

  // Argumentos
  unsigned char arguments[MAXSIZE];
  unsigned long int arguments_length = 0;
  int arguments_offset = 0;

  unsigned long int reserved_1 = 0x00;

  write_long_int(arguments, arguments_offset, reserved_1);
  arguments_offset += 4;
  arguments_length += 4;

  write_stream(method_frame, arguments, method_frame_offset, arguments_length);
  method_frame_offset += arguments_length;
  method_frame_length += arguments_length;

  write_long_int(frame, frame_offset, method_frame_length);
  frame_offset += 4;
  frame_length += 4;

  write_stream(frame, method_frame, frame_offset, method_frame_length);
  frame_offset += method_frame_length;
  frame_length += method_frame_length;

  frame[frame_offset] = 0xce;
  frame_offset += 1;
  frame_length += 1;

  return frame_length;
}


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
    
        
        // Recebe Protocol header

        unsigned char protocol_header[PROTOCOL_HEADER_SIZE]; 
        int offset = 0;
        int bytes_read = 0;

        while (bytes_read < 8) {
          n = read(connfd, protocol_header + offset, 8 - bytes_read); 
          bytes_read += n;
          offset += n;
        }
        printf("Protocol Header recebido\n");

        // Envia Connection.Start

        unsigned char default_protocol_header[] = {'A', 'M', 'Q', 'P', 0x00, 0x00, 0x09, 0x01};
        if(memcmp(protocol_header, default_protocol_header, sizeof(default_protocol_header)) == 0) 
        { 
          printf("Connection.Start enviado\n");

          unsigned char connection_start_frame[MAXSIZE];
          int n = connection_start(connection_start_frame);
          write(connfd, connection_start_frame, n);
        }
        else 
        { 
          printf("Protocolo inválido\n");
          close(connfd);
        }

        // Recebe Connection.Start-Ok

        unsigned char connection_start_ok_frame[MAXSIZE]; 
        connection_start_ok_frame[0] = 0;
        offset = 0;
        bytes_read = 0;
        unsigned char last_byte_read = 0;

        while (last_byte_read != FRAME_END) {
          n = read(connfd, connection_start_ok_frame + offset, MAXLINE); 
          bytes_read += n;
          offset += n;
          last_byte_read = connection_start_ok_frame[offset-1];
        }
        printf("Connection.Start-Ok recebido\n");

        // Envia o Connection.Tune
        unsigned char connection_tune_frame[MAXSIZE];
        n = connection_tune(connection_tune_frame);
        write(connfd, connection_tune_frame, n);

        printf("Connection.Tune enviado\n");

        // Recebe o Connection.Tune-Ok
        // Agora o cliente tem a função de mandar dois comandos em sequência, mas não posso assumir que ele vai mandar um seguido do outro, então tenho que fazer as transições de forma separada ?

        unsigned char connection_tune_ok_frame[MAXSIZE]; 
        connection_tune_ok_frame[0] = 0;
        offset = 0;
        bytes_read = 0;
        last_byte_read = 0;

        while (last_byte_read != FRAME_END) {
          n = read(connfd, connection_tune_ok_frame + offset, MAXLINE); 
          bytes_read += n;
          offset += n;
          last_byte_read = connection_tune_ok_frame[offset-1];
        }
        printf("Connection.Tune-Ok recebido\n");

        int count = 0; // número de frames enviados
        for(int i = 0; i < bytes_read; i++) if(connection_tune_ok_frame[i] == 0xce) count++; 

        if(count == 1) 
        {
          // Recebe o Connection.Open 
          
          unsigned char connection_open_frame[MAXSIZE]; 
          connection_open_frame[0] = 0;
          offset = 0;
          bytes_read = 0;
          last_byte_read = 0;

          while (last_byte_read != FRAME_END) {
            n = read(connfd, connection_open_frame + offset, MAXLINE); 
            bytes_read += n;
            offset += n;
            last_byte_read = connection_open_frame[offset-1];
          }
        }

        printf("Connection.Open recebido\n");

        // Envia o Connection.Open-Ok

        unsigned char connection_open_ok_frame[MAXSIZE];
        n = connection_open_ok(connection_open_ok_frame);
        write(connfd, connection_open_ok_frame, n);

        printf("Connection.Open-Ok enviado\n");

        // Recebe Channel.Open
        unsigned char channel_open_frame[MAXSIZE]; 
        channel_open_frame[0] = 0;
        offset = 0;
        bytes_read = 0;
        last_byte_read = 0;

        while (last_byte_read != FRAME_END) {
          n = read(connfd, channel_open_frame + offset, MAXLINE); 
          bytes_read += n;
          offset += n;
          last_byte_read = channel_open_frame[offset-1];
        }

        printf("Channel.Open recebido\n");

        // Envia Channel.Open-Ok
        unsigned char channel_open_ok_frame[MAXSIZE];
        n = channel_open_ok(channel_open_ok_frame);
        write(connfd, channel_open_ok_frame, n);

        printf("Channel.Open-Ok enviado\n");

        // Agora cheguei no estado WAIT_FUNCTIONAL

        for(;;){}

        close(connfd);
        printf("[Uma conexão fechada]\n");
        //exit(0);

    }
    exit(0);
}
