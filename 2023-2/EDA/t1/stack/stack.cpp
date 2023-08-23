#include <iostream>
#include <cmath>
#include "stack.h"

template<class T> 
Stack<T>::Stack(Node<T> *n) {top = n;}

template<class T>
Stack<T>::Stack(){ 
  Node<T>* u = new Node<T>(); 
  u->jump[0] = u;
  std::cout << "Criando nó da raiz\n";

  std::cout << "Valor da raiz: " << u->val << std::endl; //não se deve acessar o valor da raiz
  std::cout << "Profundidade da raiz: ";
  std::cout << u->depth << "\n";
  std::cout << "\n";
                                                      
  top = u;
  
}

template<class T>
Stack<T> Stack<T>::Push(const T x){ 
  std::cout << "Criando novo nó\n";
  //cria o nó "nu"
  unsigned int depth = top->depth + 1;
  Node<T>* v = new Node<T>(x,depth);
  std::cout << "Valor do nó: " << v->val << std::endl; //não se deve acessar o valor da raiz
  std::cout << "Profundidade do nó: " << v->depth << std::endl;
  unsigned int size =  (depth == 0 ? 2 : 2 + (int)(floor(log2(depth))));
  std::cout << "Tamanho da lista a ser criada do nó: " << size << std::endl;
  //constrói jump list
  BuildJumpList(v,top,size);
  std::cout << "\n";
  //PrintJumpList(v);
  return Stack(v);
}

template<class T>
void Stack<T>::PrintJumpList(Node<T> *u) const{
  unsigned int size =  (u->depth == 0 ? 2 : 2 + (int)(floor(log2(u->depth))));

  for(int i = 0; i < size; i++){
    std::cout << u->jump[i]->val << " ";
  }
  std::cout << "\n";
}

template<class T>
Stack<T> Stack<T>::Pop(){ 
  //tratar pop na pilha vazia
  if(IsEmpty()){}//erro ou só retornar a raiz?}
  else{ return Stack(top->parent());}
}

template<class T>
T Stack<T>::Top() const{ 
  if(IsEmpty()){} //throw error, acessar o valor da raiz não é permitido, pois seu valor é lixo}
  else{return top->val;}
}

template<class T>
bool Stack<T>::IsEmpty() const{
  //o campo depth sempre é inicializado
  return (top->depth == 0 ? true : false);
}

template<class T>
unsigned int Stack<T>::Size() const{ return top->depth; }

template<class T>
T Stack<T>::K(unsigned  int k) const { }

template<class T>
void Stack<T>::BuildJumpList(Node<T>* u, Node<T>* parent, unsigned int size){
  std::cout << "Construindo jump list do nó: " << std::endl;
  u->jump[0] = u;
  std::cout << "jump[" << 0 << "]: " << u->jump[0]->val << std::endl;
  u->jump[1] = parent;
  std::cout << "jump[" << 1 << "]: " << u->jump[1]->val << std::endl;

  for(int i = 2; i < size; i++){
    u->jump[i] = u->jump[i-1]->jump[i-1];
    std::cout << "jump[" << i << "]: " << u->jump[i]->val << std::endl;
  }

}


int main(){
  Stack<char> p0 = Stack<char>();
  Stack<char>p1 = p0.Push('a');
  Stack<char>p2 = p1.Push('b');
  Stack<char>p3 = p2.Push('c');
  Stack<char>p4 = p3.Push('d');
  Stack<char>p5 = p4.Push('e');
  Stack<char>p6 = p5.Push('f');
  Stack<char>p7 = p6.Push('g');
  Stack<char>p8 = p7.Push('h');
  Stack<char>p9 = p8.Push('i');
  
  //s.Push('c');
  //s.Push('d');
  //s.Push('e');
  //s.Push('f');
  //s.Push('g');
  //s.Push('h');

  //std::cout << s.Top() << std::endl;
}
