#include <iostream>
#include <string>

#ifndef DEQUE_H
#define DEQUE_H

template<class T> class Node {
  public:
    T val;
    const unsigned int depth;
    Node* parent;
    Node* jump;
    
    // Os ponteiros parent e jump são inicializados pela classe deque
    Node(const T val, unsigned int depth_new_node) : val(val), depth(depth_new_node) {}
    
    // Construtor para a raiz
    Node() : depth(0) {
      parent = nullptr;
      jump = nullptr;
    }

};


template<class T> class Deque {

  private:

    Node<T>* first;
    Node<T>* last;

    Node<T>* LA(Node<T>* u, unsigned int k);

    Node<T>* LCA(Node<T>* u, Node<T>* v);

    Deque<T> Swap();

    Deque(Node<T>* new_first, Node<T>* new_last);

    void BuildJumpPointer(Node<T>* u, Node<T>* u_parent);

  public:

    Deque();

    Deque<T> PushFront(T x);

    Deque<T> PushBack(T x);

    Deque<T> PopFront();

    Deque<T> PopBack();

    T Front() const;

    T Back() const;

    T K() const;

    T K(unsigned int k) const; 

    std::string Print() const;


};

#endif
