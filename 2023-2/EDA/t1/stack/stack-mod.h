#include <cmath>

#ifndef STACK_H
#define STACK_H

template<class T> class Node {
  public:
    T val;
    unsigned int depth;
    Node** jump;
    
    //o construtor faz somente duas coisas: inicializa o valor guardado no nó e aloca espaço para a jump list, de acordo com o tamanho requerido 
    Node(const T val, unsigned int depth_new_node): val(val), depth(depth_new_node){
      unsigned int size =  (depth_new_node == 0 ? 0 : 1 + (int)(floor(log2(depth_new_node))));
      jump = new Node*[size];
      //jump[0] = this;
    };
    
    //construtor para a raiz
    Node(){
      depth = 0;
      jump = nullptr;
      //jump[0] = this;
    }

    //tamanho da lista sempre é pelo menos dois. Como deixar isso mais seguro, para evitar erros em tempo de execução?
    Node* parent(){ return jump[0];}

};

template<class T> class Stack{

 
  public:

    //inicializa pilha vazia
    Stack();

    Stack(Node<T> *n); 

    Stack Push(const T x);
        
    void BuildJumpList(Node<T>* u, Node<T>* parent, unsigned int size);

    Stack Pop();

    T Top() const; 
    
    void  Show();

    unsigned int Size() const;

    bool IsEmpty() const;

    T K(unsigned int k) const;

  private:
    Node<T> *top;
    
    //somente para teste
    void PrintJumpList(Node<T> *u) const;


};



#endif
