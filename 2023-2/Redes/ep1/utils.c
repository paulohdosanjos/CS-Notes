#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
#include "server_config.h"

// Lê um frame do socket e escreve em buf
int read_frame (int connfd, unsigned char* buf)
{
  int n;
  buf[0] = 0;
  int offset = 0;
  int bytes_read = 0;
  int last_byte_read = 0;

  while (last_byte_read != FRAME_END) {
    n = read(connfd, buf + offset, MAXLINE); 
    bytes_read += n;
    offset += n;
    last_byte_read = buf[offset-1];
  }

  return bytes_read;
}

// Lê um frame do socket e escreve em buf
int read_header (int connfd, unsigned char* buf)
{
  int n;
  buf[0] = 0;
  int offset = 0;
  int bytes_read = 0;
  int last_byte_read = 0;

  while (bytes_read < PROTOCOL_HEADER_SIZE) {
    n = read(connfd, buf + offset, MAXLINE); 
    bytes_read += n;
    offset += n;
    last_byte_read = buf[offset-1];
  }

  return bytes_read;
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

// Escreve o short int n na posição i do frame, ele ocupará 2 bytes
void write_short_int(unsigned char* frame, int i, unsigned short int n)
{
  frame[i+1] = n & 0xFF; 
  n >>= 8;
  frame[i] = n & 0xFF; 
}

// Escreve o long int n na posição i do frame, ele ocupará 4 bytes
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

// Escreve o long long int n na posição i do frame, ele ocupará 8 bytes
void write_long_long_int(unsigned char* frame, int i, unsigned long int n)
{
  frame[i+7] = n & 0xFF; 
  n >>= 8;
  frame[i+6] = n & 0xFF; 
  n >>= 8;
  frame[i+5] = n & 0xFF; 
  n >>= 8;
  frame[i+4] = n & 0xFF; 
  n >>= 8;
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

int connection_close_ok (unsigned char* frame)
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
  unsigned short int method_id = 51; // Method Close-Ok
  
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

int channel_close_ok (unsigned char* frame)
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
  unsigned short int method_id = 41; // Method Close-Ok
  
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

int queue_declare_ok(unsigned char* frame, char* _queue_name)
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

  unsigned short int class_id = 50; // Class Queue
  unsigned short int method_id = 11; // Method Declare-Ok
  
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

  unsigned char* queue_name = to_shortstr(_queue_name);
  unsigned long int message_count = 0;
  unsigned long int consumer_count = 0;

  int n = 1 + strlen(_queue_name);
  write_stream(arguments, queue_name, arguments_offset, 1 + strlen(_queue_name));
  arguments_offset += n;
  arguments_length += n;

  write_long_int(arguments, arguments_offset, message_count);
  arguments_offset += 4;
  arguments_length += 4;

  write_long_int(arguments, arguments_offset, consumer_count);
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


int basic_consume_ok(unsigned char* frame, char* _consumer_tag)
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

  unsigned short int class_id = 60; // Class Basic
  unsigned short int method_id = 21; // Method Consume-Ok
  
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

  unsigned char* consumer_tag = to_shortstr(_consumer_tag);
  int n = 1 + strlen(_consumer_tag);

  write_stream(arguments, consumer_tag, arguments_offset, n);
  arguments_offset += n;
  arguments_length += n;

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

int basic_deliver(unsigned char* frame, unsigned char* msg, int msg_length, char* _consumer_tag, char* _queue_name)
{

  // Montagem do Method frame Basic.Deliver
  unsigned char method_frame[MAXSIZE];
  unsigned long int method_frame_length = 0;
  int method_frame_offset = 0;

  // Parte do General Frame
  unsigned char type = 1; // Method frame
  unsigned short int channel = 1;

  method_frame[method_frame_offset] = type;
  method_frame_offset += 1;
  method_frame_length += 1;

  write_short_int(method_frame, method_frame_offset, channel);
  method_frame_offset += 2;
  method_frame_length += 2;

  // Para espercífica do Method frame
  unsigned char payload[MAXSIZE];
  unsigned long int payload_length = 0;
  int payload_offset = 0;

  unsigned short int class_id = 60; // Class Basic
  unsigned short int method_id = 60; // Method Deliver
  
  write_short_int(payload, payload_offset, class_id);
  payload_offset += 2;
  payload_length += 2;

  write_short_int(payload, payload_offset, method_id);
  payload_offset += 2;
  payload_length += 2;

  // Argumentos do payload
  unsigned char arguments[MAXSIZE];
  unsigned long int arguments_length = 0;
  int arguments_offset = 0;

  unsigned char* consumer_tag = to_shortstr(_consumer_tag);
  unsigned long long int delivery_tag = 1;
  char redelivered = 0;
  char exchange = 0;
  unsigned char* routing_key = to_shortstr(_queue_name);

  int n = 1 + strlen(_consumer_tag);
  write_stream(arguments, consumer_tag, arguments_offset, n);
  arguments_offset += n;
  arguments_length += n;

  write_long_long_int(arguments, arguments_offset, delivery_tag);
  arguments_offset += 8;
  arguments_length += 8;

  arguments[arguments_offset] = redelivered;
  arguments_offset += 1;
  arguments_length += 1;

  arguments[arguments_offset] = exchange;
  arguments_offset += 1;
  arguments_length += 1;

  n = 1 + strlen(_queue_name);
  write_stream(arguments, routing_key, arguments_offset, n);
  arguments_offset += n;
  arguments_length += n;

  write_stream(payload, arguments, payload_offset, arguments_length);
  payload_offset += arguments_length;
  payload_length += arguments_length;

  // Payload montado, agora escreve no Method frame

  write_long_int(method_frame, method_frame_offset, payload_length);
  method_frame_offset += 4;
  method_frame_length += 4;

  write_stream(method_frame, payload, method_frame_offset, payload_length);
  method_frame_offset += payload_length;
  method_frame_length += payload_length;

  method_frame[method_frame_offset] = 0xce;
  method_frame_offset += 1;
  method_frame_length += 1;

  // Method Frame do Basic.Deliver montado

  // Agora monta o Content header frame
  
  // Parte do General frame
  unsigned char content_header[MAXSIZE];
  unsigned long int content_header_length = 0;
  int content_header_offset = 0;

  type = 0x2; // Tipo : Content header
  channel = 0x1;

  content_header[content_header_offset] = type;
  content_header_offset += 1;
  content_header_length += 1;

  write_short_int(content_header, content_header_offset, channel);
  content_header_offset += 2;
  content_header_length += 2;

  // Parte específica do Content Header
  payload_length = 0;
  payload_offset = 0;

  class_id = 60; // Class Basic

  write_short_int(payload, payload_offset, class_id);
  payload_offset += 2;
  payload_length += 2;

  unsigned short int weight = 0; // Deve ser sempre 0
  
  write_short_int(payload, payload_offset, weight);
  payload_offset += 2;
  payload_length += 2;

  unsigned long long int body_size = msg_length; // Tamanho da mensagem
  write_long_long_int(payload, payload_offset, body_size);
  payload_offset += 8;
  payload_length += 8;

  unsigned short int property_flags = 0x1000;
  write_short_int(payload, payload_offset, property_flags);
  payload_offset += 2;
  payload_length += 2;

  unsigned char delivery_mode = 1;
  payload[payload_offset] = delivery_mode;
  payload_offset += 1;
  payload_length += 1;

  // Payload montado, escreve agora no Content Header

  write_long_int(content_header, content_header_offset, payload_length);
  content_header_offset += 4;
  content_header_length += 4;

  write_stream(content_header, payload, content_header_offset, payload_length);
  content_header_offset += payload_length;
  content_header_length += payload_length;

  content_header[content_header_offset] = FRAME_END;
  content_header_offset += 1;
  content_header_length += 1;
  
  // Content Header montado

  // Montagem do Content Body. 
  // Vou colocar toda a mensagem em um só frame. O certo seria analisar o tamanho máximo de um frame acordado pelo cliente e pelo servidor e dividir a mensagem se necessário. Como as mensagens aqui cabem em um só frame, farei isso.

  // General frame
  unsigned char content_body[MAXSIZE];
  unsigned long int content_body_length = 0;
  int content_body_offset = 0;

  type = 3; // Tipo : Content Body
  channel = 1;

  content_body[content_body_offset] = type;
  content_body_offset += 1;
  content_body_length += 1;

  write_short_int(content_body, content_body_offset, channel);
  content_body_offset += 2;
  content_body_length += 2;

  // Parte específica do Content Body

  payload_length = 0;
  payload_offset = 0;

  // O payload de um Content Body é a própria mensagem
  write_stream(payload, msg, payload_offset, msg_length);
  payload_offset += msg_length;
  payload_length += msg_length;

  write_long_int(content_body, content_body_offset, payload_length);
  content_body_offset += 4;
  content_body_length += 4;

  write_stream(content_body, payload, content_body_offset, payload_length);
  content_body_offset += payload_length;
  content_body_length += payload_length;

  content_body[content_body_offset] = FRAME_END;
  content_body_offset += 1;
  content_body_length += 1;
  
  // Content Body montado

  // Junta os três frames em um só 
    
  unsigned long int frame_length = 0;
  int frame_offset = 0;

  write_stream(frame, method_frame, frame_offset, method_frame_length);
  frame_offset += method_frame_length;
  frame_length += method_frame_length;

  write_stream(frame, content_header, frame_offset, content_header_length);
  frame_offset += content_header_length;
  frame_length += content_header_length;

  write_stream(frame, content_body, frame_offset, content_body_length);
  frame_offset += content_body_length;
  frame_length += content_body_length;

  return frame_length;
}



