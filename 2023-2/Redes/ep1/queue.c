#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

// Cria nova fila e retorna um ponteiro para ela
queue* create_queue(char* queue_name)
{
  queue* q = malloc(sizeof(queue));
  strcpy(q->name, queue_name);
  q->size = 0;
  q->first = NULL;
  q->last = NULL;
  return q;
}

// Empilha mensagem msg na fila q
void enqueue_queue(queue* q, char* msg)
{
  node* new_node = malloc(sizeof(node));
  new_node->next = NULL;
  new_node->past = q->last;
  strcpy(new_node->message, msg);

  if(q->size == 0)
    q->first = new_node;
  
  else q->last->next = new_node;

  q->last = new_node;
  q->size += 1;
}

// Desempilha primeira mensagem da fila q e salva em buf
void dequeue_queue(queue* q, char* buf)
{
  if(q->size == 0) return; // Quero isso ?

  node* new_first = q->first->next;

  strcpy(buf, q->first->message);
  free(q->first);
  if(new_first != NULL) new_first->past = NULL;

  q->first = new_first;
  q->size--;
}

// Deleta fila q
void remove_queue(queue* q)
{
  node* x = q->first;
  while(x != NULL)
  {
    node* tmp = x;
    x = x->next;
    free(tmp);
  }
  free(q);
}
