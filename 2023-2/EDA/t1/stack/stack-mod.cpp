#include <iostream>
#include <cmath>
#include <string>
#include <chrono>
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
std::string Stack<T> :: Show(){
  //supõe char
  unsigned int size = Size();

  std::string output = std::string();

  output = output + "(" + std::to_string(size) + ") ";
  Node<T> *x = top;
  for(int i = 0; i < size; i++){
    output = output + x->val + " ";
    x = x->jump[0];
  }

  output = output + "\n";
  return output;
}

template<class T>
std::string Stack<T> :: Show_Int(){
  //supõe int
  unsigned int size = Size();

  std::string output = std::string();

  output = output + "(" + std::to_string(size) + ") ";
  Node<T> *x = top;
  for(int i = 0; i < size; i++){
    output = output + std::to_string(x->val) + " ";
    x = x->jump[0];
  }

  output = output + "\n";
  return output;
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
T Stack<T>::K(unsigned  int k) const {
  std::cout << "Query: K-th(" << k << ")\n";
  Node<T>* x = top;
  unsigned int leftmost_index = (int)(floor(log2(k)));
  unsigned int mask = 1 << (leftmost_index);
  for(int i = leftmost_index; i >= 0; i--, mask >>= 1){
    //std::cout << "leftmost_index " << i << std::endl;
    //std::cout << "mask " << mask << std::endl;
    if(k & mask){
      x = x->jump[i];
    }
  }

  return x->val;
} 

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

void Teste1(){
//Testes genéricos
  pilha p0 = pilha();
  std::cout << "p0: "; //p0: 
  std::cout << p0.Show();

  pilha p1 = p0.Push('a'); //p1: a
  std::cout << "p1: ";
  std::cout << p1.Show();

  pilha p2 = p1.Push('b'); //p2: b a
  std::cout << "p2: ";
  std::cout << p2.Show();

  pilha p3 = p2.Push('c'); //p3: c b a
  std::cout << "p3: ";
  std::cout << p3.Show();

  pilha p4 = p3.Pop();
  std::cout << "p4: "; //p4: b a
  std::cout << p4.Show();

  pilha p5 = p4.Pop(); //p5: a
  std::cout << "p5: ";
  std::cout << p5.Show();

  pilha p6 = p5.Pop(); //p6: 
  std::cout << "p6: ";
  std::cout << p6.Show();

  pilha p7 = p3.Push('d'); //p7: d c b a
  std::cout << "p7: ";
  std::cout << p7.Show();

  //Resultado: OK
}


void Teste2(){
  //Testa K-th()
  pilha p = pilha();
  p = p.Push('a');
  p = p.Push('b');
  p = p.Push('c');
  p = p.Push('d');
  p = p.Push('e');
  p = p.Push('f');
  p = p.Push('g');
  p = p.Push('h');
  p = p.Push('i');
  p = p.Push('j');
  p = p.Push('k');
  p = p.Push('l');
  p = p.Push('m');
  p = p.Push('n');
  p = p.Push('o');
  p = p.Push('p');
  p = p.Push('q');
  p = p.Push('r');
  p = p.Push('s');
  p = p.Push('t');
  p = p.Push('u');
  p = p.Push('v');
  p = p.Push('w');
  p = p.Push('x');
  p = p.Push('y');
  p = p.Push('z');
  std::cout << p.Show();

  //Algumas queries de LA
  std::cout << p.K(1) << std::endl;
  std::cout << p.K(2) << std::endl;
  std::cout << p.K(3) << std::endl;
  std::cout << p.K(4) << std::endl;
  std::cout << p.K(5) << std::endl;
  std::cout << p.K(6) << std::endl;
  std::cout << p.K(7) << std::endl;
  std::cout << p.K(8) << std::endl;
  std::cout << p.K(9) << std::endl;
  std::cout << p.K(10) << std::endl;
  std::cout << p.K(11) << std::endl;
  std::cout << p.K(12) << std::endl;
  std::cout << p.K(13) << std::endl;
  std::cout << p.K(14) << std::endl;
  std::cout << p.K(15) << std::endl;
  std::cout << p.K(16) << std::endl;
  std::cout << p.K(17) << std::endl;
  std::cout << p.K(18) << std::endl;
  std::cout << p.K(19) << std::endl;
  std::cout << p.K(20) << std::endl;
  std::cout << p.K(21) << std::endl;
  std::cout << p.K(22) << std::endl;
  std::cout << p.K(23) << std::endl;
  std::cout << p.K(24) << std::endl;
  std::cout << p.K(25) << std::endl;

  //Resultado: OK
}

void Teste3(int SIZE){
  //teste K-th() para pilha grande


  Stack<int> p = Stack<int>();
  for(int i = 0 ; i < SIZE ; i++){
    p = p.Push(i);
  }

  //std::cout << p.Show_Int();

  auto start = std::chrono::high_resolution_clock::now();

  std::cout << p.K(1) << std::endl;
  std::cout << p.K((int)SIZE/2) << std::endl;
  std::cout << p.K((int)(2*SIZE/3)) << std::endl;
  std::cout << p.K(SIZE-1) << std::endl;
      
  auto end = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

  std::cout << "tempo das queries: " << duration << " micro segundos" << std::endl;

  //Resultado: Rápido independente do tamanho da pilha. O que é de esperar pois K-th roda em tempo O(log(k))

}


int main(){
  //Teste1();
  //Teste2();
  //Teste3(10000000);

}
