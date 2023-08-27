#include <string>
#include "../stack_skew/stack_skew.h"

#ifndef QUEUE_H
#define QUEUE_H


template<class T> class Queue {
  public:
    Queue();

    Queue<T> Enqueue(const T x);

    Queue<T> Dequeue();

    unsigned int Size();

    bool IsEmpty();

    T First();

    T K(unsigned int k);

    std::string  Show();

    std::string  ShowStack() ;

  private:
    Stack<T> s;
    unsigned int number_of_removed_elements;
    Queue(Stack<T> stack, unsigned int x);
};

#endif
