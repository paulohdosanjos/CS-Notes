#include <string>

#ifndef STACK_H
#define STACK_H

template<class T> class Node {
  public:
    T val;
    unsigned int depth;
    Node* parent;
    Node* jump;
    
    Node(const T val, unsigned int depth_new_node): val(val), depth(depth_new_node) {};
    
    // Construtor para a raiz
    Node(){
      depth = 0;
      parent = nullptr;
      jump = nullptr;
    }

};

template<class T> class Stack{

  public:

    Stack();

    Stack(Node<T> *n); 

    Stack Push(const T x);
        
    void BuildJumpPointer(Node<T>* u, Node<T>* parent);

    Stack Pop();

    T Top() const; 
    
    std::string  Show();

    std::string  Show_Int();

    unsigned int Size() const;

    bool IsEmpty() const;

    T K(unsigned int k) const;

    std::string PrintJumpPointers() const;

  private:

    Node<T> *top;

};

#endif
