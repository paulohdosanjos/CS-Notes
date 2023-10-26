#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXSIZE 10000

// Recebe uma string s e dá o "payload" correspondente
unsigned char* to_longstr(char* value)
{
  int n = strlen(value);
  unsigned char* s = (unsigned char*) malloc((n + 5) * sizeof (unsigned char)); 
  unsigned char buffer_length[4];
  s[3] = n & 0xFF; 
  n >>= 8;
  s[2] = n & 0xFF; 
  n >>= 8;
  s[1] = n & 0xFF;
  n >>= 8;
  s[0] = n & 0xFF; 

  for(int i = 0; i < strlen(value); i++) 
    s[i+4] = value[i];

  s[strlen(value)+4] = '\0';
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
void write_stream(unsigned char* frame, unsigned char* buffer, int n, int i)
{
  for(int j = 0; j < n; j++) {
    frame[i+j] = buffer[j];
    //printf("frame[%d] = %c, %02x\n", i+j, frame[i+j], frame[i+j]);
  }
}

void connection_start ()
{
  unsigned char frame[MAXSIZE];

  // General Frame
  unsigned char type = 0x1; // Method frame
  unsigned short int channel = 0x0;
  unsigned long int length = 499;


  // Method frame
  unsigned short int class_id = 10; // Class Connection
  unsigned short int method_id = 10; // Method Start
  
  // Connection.Start
  unsigned char version_major = 0;
  unsigned char version_minor = 9;

  unsigned char* mechanisms = to_longstr("PLAIN AMQPLAIN");
  unsigned char* locales = to_longstr("en_US");

  frame[0] = type;
  write_short_int(frame, 1, channel);
  write_long_int(frame, 3, length);
  write_short_int(frame, 7, class_id);
  write_short_int(frame, 9, method_id); 
  frame[11] = version_major;
  frame[12] = version_minor;
  // OK

  write_stream(frame, mechanisms, 18, 13);
  write_stream(frame, locales, 9, 31);

  frame[31+9] = 0xce;

  for(int i = 31; i < 31 + 10; i++) printf("%02x ", frame[i]);
  printf("\n");

}



int main(int argc, char *argv[])
{
  connection_start();

 // char* s = "paulo henrique";

 // unsigned char* representation_of_s = to_longstr(s);

 // for(int i = 0; i < strlen(s) + 5 ; i++) printf("%02X ", representation_of_s[i]);

 // printf("\n");

}
