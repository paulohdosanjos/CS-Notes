#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

void print_queue(queue* q)
{
  printf("***************\n");
  printf("%s (%d) :\n", q->name, q->size);
  node* x = q->first;
  while(x != NULL)
  {
    puts(x->message);
    x = x->next;
  }
  printf("***************\n");
}

// Teste inicial simples
void Teste1()
{
  queue* q = create_queue("fila1");
  enqueue_queue(q, "a");
  enqueue_queue(q, "b");
  enqueue_queue(q, "f");
  enqueue_queue(q, "a");
  enqueue_queue(q, "d");

  print_queue(q); // a b f a d

  char dst[MAX_MSG_LENGTH];
  dequeue_queue(q, dst);
  puts(dst); // a

  dequeue_queue(q, dst);
  puts(dst); // b

  dequeue_queue(q, dst);
  puts(dst); // f

  enqueue_queue(q, "g");
  enqueue_queue(q, "h");
  dequeue_queue(q, dst);
  puts(dst); // a

  print_queue(q); // d g h
  remove_queue(q);
}

// Desempilha de fila vazia
void Teste2()
{
  queue* q = create_queue("fila1");
  dequeue_queue(q, NULL);
}

int main(int argc, char *argv[])
{
  Teste1();
  //Teste2();
}
