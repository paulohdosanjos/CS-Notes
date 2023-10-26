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

char* parse_protoocol_header(const char*);

// Retorna uma nova string com o parse do protocol header
char* parse_protocol_header (const char* protocol_header)
{
  int header_size = 8;
  char* s = (char*) malloc((header_size + 1) * sizeof(char)); 

  strcpy(s, protocol_header);
  s[4] = (char) ((int) '0' + protocol_header[4]);
  s[5] = (char) ((int) '0' + protocol_header[5]);
  s[6] = (char) ((int) '0' + protocol_header[6]);
  s[7] = (char) ((int) '0' + protocol_header[7]);

  s[8] = '\0';
  return s;
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

        break;

        close(listenfd);
    
        // Primeiro, a troca do protocol header
        
        // Recebe o protocol header
        char buf[MAXLINE]; 
        int offset = 0;
        int bytes_read = 0;

        while (bytes_read < 8) {
          n = read(connfd, buf + offset, 8 - bytes_read); 
          bytes_read += n;
          offset += n;
          //printf("bytes lidos = %ld\n", n);
          //recvline[n]=0;
          printf("[Cliente conectado no processo filho %d enviou o protocol header\n",getpid());
        }

        const char* parsed_protocol_header = parse_protocol_header(recvline);
        if( strcmp(parsed_protocol_header, "AMQP0901") ) 
        { 
          printf("Protocolo válido\n");

          unsigned char frame[MAXSIZE];
          int n = connection_start(frame);

          printf("frame : \n");
          for(int i = 0; i < n; i++) printf("%02x ", frame[i]);
          printf("\n");

          write(connfd, frame, n);
        }
        else 
        { 
          printf("Protocolo inválido\n");
          close(connfd);
        }

        close(connfd);
        printf("[Uma conexão fechada]\n");
        exit(0);

    }
    exit(0);
}
