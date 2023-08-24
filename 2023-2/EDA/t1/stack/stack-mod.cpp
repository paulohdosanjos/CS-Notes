#include <iostream>
#include <cmath>
#include "stack-mod.h"

typedef Stack<char> pilha;

template<class T> 
Stack<T>::Stack(Node<T> *n) {top = n;}

template<class T>
Stack<T>::Stack(){ 
  Node<T>* u = new Node<T>(); 
  //u->jump[0] = u;
  //std::cout << "Criando nó da raiz\n";

  //std::cout << "Valor da raiz: " << u->val << std::endl; //não se deve acessar o valor da raiz
  //std::cout << "Profundidade da raiz: ";
  //std::cout << u->depth << "\n";
  //std::cout << "\n";
                                                      
  top = u;
  
}

template<class T>
Stack<T> Stack<T>::Push(const T x){ 
  //std::cout << "Criando novo nó\n";
  //cria o nó "nu"
  unsigned int depth = top->depth + 1;
  Node<T>* v = new Node<T>(x,depth);
  //std::cout << "Valor do nó: " << v->val << std::endl; //não se deve acessar o valor da raiz
  //std::cout << "Profundidade do nó: " << v->depth << std::endl;
  unsigned int size =  (depth == 0 ? 0 : 1 + (int)(floor(log2(depth))));
  //std::cout << "Tamanho da lista a ser criada do nó: " << size << std::endl;
  //constrói jump list
  BuildJumpList(v,top,size);
  //std::cout << "\n";
  //PrintJumpList(v);
  return Stack(v);
}

template<class T>
void Stack<T> :: Show(){
  unsigned int size = Size();

  std::cout << "(" << size << ") ";
  Node<T> *x = top;
  for(int i = 0; i < size; i++){
    std::cout << x->val << " ";
    x = x->jump[0];
  }

  std::cout << "\n";
}

template<class T>
void Stack<T>::PrintJumpList(Node<T> *u) const{
  unsigned int size =  (u->depth == 0 ? 0 : 1 + (int)(floor(log2(u->depth))));

  for(int i = 0; i < size; i++){
    std::cout << "jump[" << i << "]: " << u->jump[i]->val << std::endl;
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
T Stack<T>::K(unsigned  int k) const {} 

template<class T>
void Stack<T>::BuildJumpList(Node<T>* u, Node<T>* parent, unsigned int size){
  //std::cout << "Construindo jump list do nó: " << std::endl;
  u->jump[0] = parent;
  //std::cout << "jump[" << 0 << "]: " << u->jump[0]->val << std::endl;
  //std::cout << "jump[" << 1 << "]: " << u->jump[1]->val << std::endl;

  for(int i = 1; i < size; i++){
    u->jump[i] = u->jump[i-1]->jump[i-1];
    //std::cout << "jump[" << i << "]: " << u->jump[i]->val << std::endl;
  }

}


int main(){
  pilha p0 = pilha();
  std::cout << "p0: "; //p0: 
  p0.Show();

  pilha p1 = p0.Push('a'); //p1: a
  std::cout << "p1: ";
  p1.Show();

  pilha p2 = p1.Push('b'); //p2: b a
  std::cout << "p2: ";
  p2.Show();

  pilha p3 = p2.Push('c'); //p3: c b a
  std::cout << "p3: ";
  p3.Show();

  pilha p4 = p3.Pop();
  std::cout << "p4: "; //p4: b a
  p4.Show();

  pilha p5 = p4.Pop(); //p5: a
  std::cout << "p5: ";
  p5.Show();

  pilha p6 = p5.Pop(); //p6: 
  std::cout << "p6: ";
  p6.Show();

  pilha p7 = p3.Push('d'); //p7: d c b a
  std::cout << "p7: ";
  p7.Show();

  //Looking sharp
  

  //Testar K-th() e Size(), implementar com Skew Binary

}
