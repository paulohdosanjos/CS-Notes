#include "deque.h"
#include <string>

template<class T>
Deque<T> :: Deque(){
  first = nullptr;
  last = nullptr;
}

template<class T>
Deque<T> :: Deque(Node<T>* new_first, Node<T>* new_last){
  first = new_first;
  last = new_last;
}

template<class T>
T Deque<T> :: Front() const {
  return first->val;
}

template<class T>
T Deque<T> :: Back() const {
  return last->val;
}

// Constrói campos parent e jump de um nó u com pai u_parent
template<class T>
void Deque<T> :: BuildJumpPointer(Node<T>* u, Node<T>* u_parent) {
  u->parent = u_parent;
  Node<T>& x = u_parent;
  if (x->jump != nullptr && x->jump->depth != 0 && x->depth - x->jump->depth == x->jump->depth - x->jump->jump->depth)
      u->jump = x->jump->jump;
  else
      u->jump = x;
}

template<class T>
Deque<T> Deque<T> :: Swap() {
  return Deque(last,first);
}

template<class T>
Deque<T> Deque<T> :: PushFront(T x) {
  unsigned new_depth = first->depth + 1;
  Node<T>* u = new Node(x,new_depth);
  BuildJumpPointer(u,first);
  return Deque(u, last);
}

template<class T>
Deque<T> Deque<T> :: PushBack(T x) {
  return Swap().PushFront(x).Swap();
}

// Level ancestor k do nó u
template<class T>
Node<T>* Deque<T> :: LA(Node<T>* u, unsigned int k) {
  Node<T>* x = u;
  unsigned int desired_depth = x->depth - k;

  while (x->depth != desired_depth) {
    if(x->jump->depth >= desired_depth) 
      x = x->jump;
    else 
      x = x->parent;
  }

  return x; 
}

// Lowest Commum Ancestor de u e v
template<class T>
Node<T>* Deque<T> :: LCA(Node<T>* u, Node<T>* v) {
  // Quero que u aponte para o nó mais fundo
  if(u->depth < v->depth){
    Node<T>* tmp = v;
    v = u;
    u = tmp;
  }

  // Subo u para a profundidade de v
  u = LA(u, u->depth - v->depth);

  // Subo os dois simultaneamente até encontrar o LCA. Ele será o primeiro nó tal que pai do apontador u coincide com o pai do apontador v
  
  while(u->parent != v->parent) {
    if(u->jump != v->jump) {
      u = u->jump;
      v = v->jump;
    }
    else {
      u = u->parent;
      v = v->parent;
    }
  }
  return u->parent;
}

template<class T>
Deque<T> Deque<T> :: PopFront() {
  Node<T>* new_first;
  Node<T>* new_last;

  if(LCA(first,last) == first) {
    unsigned int jumps_to_second = last->depth - first->depth - 1; // Jumps para ir de last até o antecessor de first
    new_first = LA(last, jumps_to_second);
    new_last = last;
  }

  else {
    new_first = first->parent;
    new_last = last;
  }

  return Deque(new_first,new_last);
}

template<class T>
Deque<T> Deque<T> :: PopBack() {
  return Swap().PopFront().Swap();
}

template<class T>
T Deque<T> :: K(unsigned int k) const {
  Node<T>* mid = LCA(first,last);
  unsigned int l1,l2;
  l1 = first->depth - mid->depth + 1; // Número de nós entre first e mid, incluindo first e mid
  l2 = last->depth - mid->depth - 1; // Númeor de nós entre last e mid, incluindo last e excluindo mid 

  if(k <= l1)
    return LA(first,k-1);
  else
    return LA(last, l1 + l2 - k);
}

template<class T>
std::string Deque<T> :: Print() const {
  Node<T>* mid = LCA(first, last);
  std::string output = std::string();
  Node<T>* x = first;

  // Imprimo de first à mid, incluindo mid
  unsigned int n1 = first->depth - mid->depth + 1; // Número de nós a serem impressos nesse primeiro segmento
  for(int i = 0; i < n1; i++) {
    output = output + x->val + " ";
    x = x->parent;
  } 
  // Imprimo de last a mid, excluindo mid, na ordem reversa
  unsigned int n2 = last->depth - mid->depth; // Número de nós a serem impressos nesse segundo segmento
  Node<T>** list = new Node<T>*[n2]; // Pilha para guardar nós na ordem reversa
  x = last;
  for(int i = n2 - 1; i >= 0; i--) {
    list[i] = x;
    x = x->parent;
  } 
  for(int i = 0; i < n2; i++)
    output = output + list[i]->val + " ";

  return output;
  
}
