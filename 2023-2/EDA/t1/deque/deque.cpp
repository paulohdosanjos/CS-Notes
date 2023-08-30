#include "deque.h"
#include <string>

typedef Deque<char> fila;

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
  //if(first == nullptr) std::cout << "NULLPTR!\n";
  return first->val;
}

template<class T>
T Deque<T> :: Back() const {
  return last->val;
}

// Constrói campos parent e jump de um nó u com pai u_parent
template<class T>
void Deque<T> :: BuildJumpPointer(Node<T>* u, Node<T>* const u_parent) {
  u->parent = u_parent;
  Node<T>* x = u_parent;
  if(x == nullptr){ // Primeiro nó
    //u->jump = nullptr; // Quero isso?
    u->jump = u;
    return;
  }
  if (x->jump != nullptr && x->depth - x->jump->depth == x->jump->depth - x->jump->jump->depth)
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
  if(first == nullptr) { // Fila vazia
    //std::cout << "fila vazia, criando primeiro nó\n";
    Node<T>* u = new Node<T>(x,1); 
    //std::cout << "nó criado, construindo ponteiros do primeiro nó\n";
    BuildJumpPointer(u,first);
    //std::cout << "apontadores criados!\n";
    //std::cout << u->val << std::endl;
    return Deque<T>(u,u);
  } 

  unsigned new_depth = first->depth + 1;
  Node<T>* u = new Node<T>(x,new_depth);
  //std::cout << "criando nó " << u->val << "\n";
  BuildJumpPointer(u,first);
  //std::cout << "apontadores criados!\n";
  return Deque(u, last);
}

// Constrói apontadores parent e jump para o primeiro nó da estrutura
// template<class T>
// void Deque<T> :: BuildFirstPointer(Node<T>* new_first) {
//   new_first->parent = first;
//   new_first->jump = nullptr;
// }

template<class T>
Deque<T> Deque<T> :: PushBack(T x) {
  return Swap().PushFront(x).Swap();
}

// Level ancestor k do nó u
template<class T>
Node<T>* Deque<T> :: LA(Node<T>* const u, unsigned int const k) const {
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
Node<T>* Deque<T> :: LCA(Node<T>* const  _u, Node<T>* const _v) const {
  //std::cout << "LCA(" << _u->val << "," << _v->val << "):" << std::endl;
  Node<T>* u = _u;
  Node<T>* v = _v;
  // Quero que u aponte para o nó mais fundo entre u e v
  if(u->depth < v->depth){
    Node<T>* tmp = v;
    v = u;
    u = tmp;
  }

  //std::cout << "ponto 1\n";

  // Subo u para a profundidade de v
  u = LA(u, u->depth - v->depth);
  //std::cout << "novo u : " << u->val << "\n";

  //std::cout << "ponto 2\n";
  // Subo os dois simultaneamente até encontrar o LCA. Ele será o primeiro nó tal que o pai do apontador u coincide com o pai do apontador v

  //std::cout << "ponto 3\n";
  //std::cout << u->val << " " << v->val << "\n";
  //std::cout << (u == v ? "ok" : "No") << std::endl;
  if(u == v) {
    //std::cout << "retornando u: OK\n";
    return u; // Já achei, engloba o caso em que v é o LCA 
  }
  
  //std::cout << "ponto 4\n";
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
  //std::cout << "Dentro de LCA(): " << "\n";

  return u->parent;
}

template<class T>
Deque<T> Deque<T> :: PopFront() {
  Node<T>* new_first;
  Node<T>* new_last;

  //std::cout << "antes de LCA()\n";
  //std::cout << LCA(first,last)->val << " " << first->val << " " << last->val << "\n";
  //std::cout << (LCA(first,last) == first ? "OK1" : "OKNOTOK") << std::endl;
  //std::cout << (LCA(first,last) == nullptr ? "NULLPTR" : "OK2") << std::endl;
  if(LCA(first,last) == first) {
    //std::cout << "entrei no if\n";
    //std::cout << LCA(first,last)->val << "\n"; 
    //std::cout << (LCA(first,last) == nullptr ? "NULLPTR" : "OK3") << std::endl;
    unsigned int jumps_to_second;
    if(last->depth == first->depth) {// Só há um elemento na fila
      //std::cout << "só um elemento" << std::endl;
      jumps_to_second = 0;
      new_first = nullptr;
      new_last = nullptr;
    }
    else {
      jumps_to_second = last->depth - first->depth - 1;
      //unsigned int jumps_to_second = last->depth - first->depth - 1; // Jumps para ir de last até o antecessor de first, no caso em que first e last são distintos
      //std::cout << "OK4" << std::endl;
      new_first = LA(last, jumps_to_second);
      //std::cout << "OK5" << std::endl;
      new_last = last;
    }
  }


  else {
    //std::cout << "entrei no else\n";
    new_first = first->parent;
    new_last = last;
  }

  //std::cout << "depois de LCA(1)\n";
  //std::cout << "Tudo certo na PopFront()\n";
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
  l2 = last->depth - mid->depth; // Número de nós entre last e mid, incluindo last e excluindo mid 
  //std::cout << "Dentro de K() : \n";
  //std::cout << first->depth << " " << mid->depth << " " <<  last->depth << "\n";
  //std::cout << "l1 " << l1 << " l2 " << l2 << std::endl;
  if(k <= l1)
    return LA(first,k-1)->val;
  else
    return LA(last, l1 + l2 - k)->val;
}

template<class T>
std::string Deque<T> :: Print() const {
  if(Size() == 0) return std::string();
  Node<T>* mid = LCA(first, last);
  //std::cout << "LCA calculado!" << std::endl;
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
  //std::cout << n1 << " " << n2 << std::endl;
  Node<T>** list = new Node<T>*[n2]; // Pilha para guardar nós na ordem reversa
  x = last;
  for(int i = n2 - 1; i >= 0; i--) {
    list[i] = x;
    x = x->parent;
  } 
  for(int i = 0; i < n2; i++)
    output = output + list[i]->val + " ";

  //std::cout << "Tudo certo na função Print()\n";

  return output;
  
}

template<class T>
void Deque<T> :: Print_cout() const {
  Node<T>* mid = LCA(first, last);
  //std::cout << "LCA calculado\n";
  //std::cout << (mid == nullptr ? "mid nullptr!!" : "OK");
  //std::cout << "LCA calculado : " << mid->val << std::endl;
  Node<T>* x = first;

  // Imprimo de first à mid, incluindo mid
  unsigned int n1 = first->depth - mid->depth + 1; // Número de nós a serem impressos nesse primeiro segmento
  for(int i = 0; i < n1; i++) {
    std::cout << x->val << " ";
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
    std::cout << list[i]->val << " ";

  std::cout << std::endl;

}
// Somente para fins de depuração, é chamada apenas pela função Teste0() para um fila "reta"
template<class T>
std::string Deque<T> :: Debug() const {
  std::string output = std::string();
   
  Node<T>* x = first;

  output = output + "Nó  Prof  Jump-depth     LA(Jump)\n";

  while(x != last) {
    output = output + x->val + "    " + std::to_string(x->depth) + "    " + std::to_string(x->jump->depth) + "          " +  LA(x, x->depth - x->jump->depth)->val + "\n";
    x = x->parent;
  }

  output = output + x->val + "    " + std::to_string(x->depth) + "    " + std::to_string(x->jump->depth) + "          " +  LA(x, x->depth - x->jump->depth)->val + "\n";

  return output;

}

template<class T>
unsigned int Deque<T> :: Size() const {
  if(first == nullptr) return 0;
  Node<T>* mid = LCA(first,last);
  unsigned int l1,l2;
  l1 = first->depth - mid->depth + 1; // Número de nós entre first e mid, incluindo first e mid
  l2 = last->depth - mid->depth; // Número de nós entre last e mid, incluindo last e excluindo mid 
                                     
  //std::cout << "Dentro de Size() : \n";
  //std::cout << "l1 " << l1 << " l2 " << l2 << std::endl;
  return l1 + l2;
}

// Usa K() para imprimir a fila
template<class T>
void Deque<T> :: Print_K() const {
  std::cout << Size() << ", ";
  for(int i = 1 ; i <= Size() ; i++){
    std::cout << K(i) << " ";
  }
  std::cout << "\n";
}


// Testes para LA(). OK
void Teste0() {
  fila f1 = fila();
  //std::cout << (int) 'a' << std::endl;
  for(int i = (int) 'a'; i < (int) 'a' + 26; i++) f1 = f1.PushFront(i);
  //std::cout << f1.Front() << std::endl; // z
  //std::cout << f1.Back() << std::endl; // a

  //std::cout << f1.Debug() << std::endl;
  f1.Print_cout();
}

// Testes das funcionalidades básicas. O "grosso" está aqui. OK
void Teste1() {
  fila f1 = fila();
  fila f2 = f1.PushFront('a');
  std::cout << f2.Print() << std::endl; // a
  fila f3 = f2.PushFront('b');
  std::cout << f3.Print() << std::endl; // b a
  fila f4 = f3.PushFront('c');
  std::cout << f4.Print() << std::endl; // c b a
  fila f5 = f4.PushFront('d'); 
  std::cout << f5.Print() << std::endl; // d c b a

  fila f6 = f3.PushBack('e'); 
  std::cout << f6.Print() << std::endl; // b a e
  fila f7 = f6.PushBack('s');
  std::cout << f7.Print() << std::endl; // b a e s
  fila f8 = f4.PushBack('x');
  std::cout << f8.Print() << std::endl; // c b a x
  
  fila f9 = f8.PopBack();
  std::cout << f9.Print() << std::endl; // c b a 
  fila f10 = f8.PopFront();
  std::cout << f10.Print() << std::endl; //b a x 
                                         
  // Testa K-th()
  
  std::cout << "************** Testando K-th() *************\n";

  f8.Print_K(); // 4, c b a x
  f9.Print_K(); // 3, c b a
  f5.Print_K(); // 4, d c b a

}

// Teste dos slides. OK
void Teste2() {
  fila f0 = fila();
  fila f1 = f0.PushBack('c');
  std::cout << f1.Print() << std::endl; // c 
  fila f2 = f1.PushBack('d');
  std::cout << f2.Print() << std::endl; // c d 
  fila f3 = f2.PushFront('b');
  std::cout << f3.Print() << std::endl; // b c d 
  fila f4 = f3.PushFront('a');
  std::cout << f4.Print() << std::endl; // a b c d
  fila f5 = f3.PopBack();
  std::cout << f5.Print() << std::endl; // b c 
  fila f6 = f5.PopBack();
  std::cout << f6.Print() << std::endl; // b 
  fila f7 = f6.PushFront('i');
  std::cout << f7.Print() << std::endl; // i b 
  fila f8 = f6.PopFront();
  std::cout << f8.Print() << std::endl; //  
  fila f9 = f8.PushFront('f');
  std::cout << f9.Print() << std::endl; // f 

}


// Depurar função PopFront(), que não deleta quando só tem um elemento. OK
void Teste3() {
  fila f0 = fila();
  fila f1 = f0.PushFront('a');
  std::cout << f1.Print() << std::endl;

  fila f2 = f1.PopFront();
  std::cout << f2.Print() << std::endl;
}

int main (int argc, char *argv[]) {
  //Teste0();
  Teste1();
  //Teste2();
  //Teste3();
  return 0;
}
