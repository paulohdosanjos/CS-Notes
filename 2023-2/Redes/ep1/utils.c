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

// Retorna um buffer contendo a representação em longstr de uma string s. Recebe uma string s, aloca espaço e escreve num buffer o tamanho da string concatenado com a própria string. O tamanho da short string deve caber em 4 byte
unsigned char* to_longstr(char* s)
{
  unsigned long int n = strlen(s);
  unsigned char* s_longstr = (unsigned char*) malloc((n + 4) * sizeof (unsigned char)); 

  write_long_int(s_longstr, 0, n);
  memcpy(s_longstr + 4, s, n);

  return s_longstr;
}

// Retorna um buffer contendo a representação em shorstr de uma string s. Recebe uma string s, aloca espaço e escreve num buffer o tamanho da string concatenado com a própria string. O tamanho da short string deve caber em 1 byte
unsigned char* to_shortstr(char* s)
{
  int n = strlen(s);
  unsigned char* shortstr = (unsigned char*) malloc((n + 1) * sizeof (unsigned char)); 
  shortstr[0] = n; 
  memcpy(shortstr + 1, s, n);

  return shortstr;
}

// Escreve os dois bytes de n de forma contígua a partir da posição i de dst. Retorna a quantidade de bytes escritos 
int write_short_int(unsigned char* dst, int i, unsigned short int n)
{
  dst[i+1] = n & 0xFF; 
  n >>= 8;
  dst[i] = n & 0xFF; 

  return 2;
}

// Escreve os 4 bytes de n de forma contígua a partir da posição i de dst. Retorna a quantidade de bytes escritos 
int write_long_int(unsigned char* dst, int i, unsigned long int n)
{
  dst[i+3] = n & 0xFF; 
  n >>= 8;
  dst[i+2] = n & 0xFF; 
  n >>= 8;
  dst[i+1] = n & 0xFF; 
  n >>= 8;
  dst[i] = n & 0xFF; 

  return 4;
}

// Escreve os 8 bytes de n de forma contígua a partir da posição i de dst. Retorna a quantidade de bytes escritos 
int write_long_long_int(unsigned char* frame, int i, unsigned long long int n)
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

  return 8;
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

// Escreve em dst os frames do comando Basic.Deliver enviado pelo servidor. Recebe como parâmetros a mensagem a ser enviada, junto com seu tamanho, além do nome da fila onde a mensagem está armazenada e o consumer_tag do cliente
int basic_deliver(unsigned char* dst, unsigned char* msg, int msg_size, char* _consumer_tag, char* _queue_name)
{
  // Serão montados 3 frames (Method Frame + Content Frame + 1 Content Body) nessa ordem, e depois serão todos agrupados e escritos em dst
  
  // Primeiramente, monta argumentos do Method Frame  

  unsigned char arguments[MAXSIZE];
  unsigned long int arguments_offset = 0;

  unsigned char* consumer_tag = to_shortstr(_consumer_tag);
  unsigned long long int delivery_tag = 1;
  char redelivered = 0;
  char exchange = 0;
  unsigned char* routing_key = to_shortstr(_queue_name);

  int n = 1 + strlen(_consumer_tag);
  memcpy(arguments + arguments_offset, consumer_tag, n);
  arguments_offset += n;
  free(consumer_tag);

  arguments_offset += write_long_long_int(arguments, arguments_offset, delivery_tag);

  arguments[arguments_offset] = redelivered;
  arguments_offset += 1;

  arguments[arguments_offset] = exchange;
  arguments_offset += 1;

  n = 1 + strlen(_queue_name);
  memcpy(arguments + arguments_offset, routing_key, n);
  arguments_offset += n;
  free(routing_key);

  unsigned long int arguments_size = arguments_offset;

  // Arguments montado. Agora monta o payload do Method Frame

  unsigned char payload[MAXSIZE];
  unsigned long int payload_offset = 0;

  unsigned short int class_id = 60; // Class Basic
  unsigned short int method_id = 60; // Method Deliver
  
  unsigned long int payload_size = mount_method_frame_payload(payload, class_id, method_id, arguments, arguments_size);

  // Payload montado, agora monta o Method frame

  unsigned char type = 1; // Method frame
  unsigned short int channel = 1;

  unsigned char method_frame[MAXSIZE];
  unsigned long int method_frame_offset = mount_general_frame(method_frame, type, channel, payload, payload_size);

  // Method Frame do Basic.Deliver montado

  // Agora monta o Content header frame
  
  // Primeiramente, monta payload do Content Header 
  
  payload_offset = 0;

  class_id = 60; // Class Basic

  payload_offset += write_short_int(payload, payload_offset, class_id);

  unsigned short int weight = 0; // Deve ser sempre 0 de acordo com a especifição do protocolo
  
  payload_offset += write_short_int(payload, payload_offset, weight);

  unsigned long long int body_size = msg_size; // Tamanho da mensagem
  payload_offset += write_long_long_int(payload, payload_offset, body_size);

  unsigned short int property_flags = 0x1000;
  payload_offset += write_short_int(payload, payload_offset, property_flags);

  unsigned char delivery_mode = 1;
  payload[payload_offset] = delivery_mode;
  payload_offset += 1;

  // Payload montado, agora monta o Content Header

  type = 0x2; 
  channel = 0x1;
  
  unsigned char content_header[MAXSIZE];
  payload_size = payload_offset;
  unsigned long int content_header_offset = mount_general_frame(content_header, type, channel, payload, payload_size);

  // Content Header montado

  // Montagem do Content Body. Vou colocar toda a mensagem em um só frame. O certo seria analisar o tamanho máximo de um frame acordado pelo cliente e pelo servidor e dividir a mensagem se necessário. Como as mensagens do EP cabem em um só frame, faço isso.
  
  // Primeiro, monta o payload do Content Body
  // O payload de um Content Body é a própria mensagem

  payload_offset = 0;

  memcpy(payload + payload_offset, msg, msg_size);
  payload_offset += msg_size;

  // Payload montado, de volta ao Content Body

  type = 3; 
  channel = 1;

  unsigned char content_body[MAXSIZE];
  payload_size = payload_offset;
  unsigned long int content_body_offset = mount_general_frame(content_body, type, channel, payload, payload_size);

  // Content Body montado

  // Junta os três frames em um só 
    
  unsigned long int dst_offset = 0;

  unsigned long int method_frame_size = method_frame_offset;
  memcpy(dst + dst_offset, method_frame, method_frame_size);
  dst_offset += method_frame_size;

  unsigned long int content_header_size = content_header_offset;
  memcpy(dst + dst_offset, content_header, content_header_size);
  dst_offset += content_header_size;

  unsigned long int content_body_size = content_body_offset;
  memcpy(dst + dst_offset, content_body, content_body_size);
  dst_offset += content_body_size;

  // Devolve tamanho total do frame montado
  unsigned long int dst_size = dst_offset;
  return dst_size;
}

// Monta um general frame a partir dos parâmetros do frame (tipo, canal) e do payload e guarda em dst
unsigned long int mount_general_frame (unsigned char* dst, unsigned char type, unsigned short int channel, unsigned char* payload, unsigned long int payload_size)
{
  unsigned long int dst_offset = 0;

  dst[dst_offset] = type;
  dst_offset += 1;

  dst_offset += write_short_int(dst, dst_offset, channel);

  dst_offset += write_long_int(dst, dst_offset, payload_size);

  memcpy(dst + dst_offset, payload, payload_size);
  dst_offset += payload_size;

  dst[dst_offset] = FRAME_END;
  dst_offset += 1;

  return dst_offset;
}

// Monta o payload de um Method Frame a partir dos parâmtros do Method Frame (class_id, method_id, arguments) e guarda em dst
unsigned long int mount_method_frame_payload (unsigned char* dst, unsigned short int class_id, unsigned short int method_id, unsigned char* arguments, unsigned long int arguments_size)
{
  unsigned long int dst_offset = 0;

  dst_offset += write_short_int(dst, dst_offset, class_id);
  dst_offset += write_short_int(dst, dst_offset, method_id);

  memcpy(dst + dst_offset, arguments, arguments_size);
  dst_offset += arguments_size;

  return dst_offset;
}

