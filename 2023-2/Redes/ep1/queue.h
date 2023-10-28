#ifndef QUEUE_H
#define QUEUE_H
// Implementação de uma fila usando lista ligada

#define MAX_QUEUE_NAME_LENGTH 256
#define MAX_MSG_LENGTH 1024

/*
 
    first                       last
      A   -->   B   -->   C  --> D
      B.next = C  B.past = A
*/

typedef struct node 
{
  struct node* next;
  struct node* past;
  char message[MAX_MSG_LENGTH];
} node;

typedef struct queue 
{
  char name[MAX_QUEUE_NAME_LENGTH];
  int size;
  node* first;
  node* last;
} queue;

// Cria nova fila e retorna um ponteiro para ela
queue* create_queue(char* queue_name);

// Empilha mensagem msg na fila q
void enqueue_queue(queue* q, char* msg);

// Desempilha primeira mensagem da fila q e salva em buf
void dequeue_queue(queue* q, char* buf);

// Deleta fila q
void remove_queue(queue* q);

// Imprime fila
void print_queue(queue* q);

#endif

