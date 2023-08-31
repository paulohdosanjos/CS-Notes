#include <iostream>
#include <string>
#include <vector>
#include <sstream>

class Node 
{
  public:
    int val;
    const unsigned int depth;
    Node* parent;
    Node* jump;
    
    // Os ponteiros parent e jump são inicializados pela classe deque
    Node(const int val, unsigned int depth_new_node) : val(val), depth(depth_new_node) {}
    
    // Construtor para a raiz
    Node() : depth(0) {
      parent = nullptr;
      jump = nullptr;
    }

};


class Deque 
{
  private:

    Node* first;
    Node* last;

    Node* LA(Node* const u, unsigned int const k) const;

    Node* LCA(Node* const u, Node* const v) const;

    Deque Swap();

    Deque(Node* new_first, Node* new_last);

    void BuildJumpPointer(Node* u, Node* const u_parent);
    
    unsigned int Size() const;

  public:

    Deque();

    Deque PushFront(int x);

    Deque PushBack(int x);

    Deque PopFront();

    Deque PopBack();

    int Front() const;

    int Back() const;

    int K() const;

    int K(unsigned int k) const; 

    std::string Print() const;
    
    void Print_K() const; // Para fins de depuração. Chamar Print() em vez dela.

};

// Constrói fila vazia
Deque :: Deque()
{
  first = nullptr;
  last = nullptr;
}

// Contrói fila com apontadores new_first e new_last
Deque :: Deque(Node* new_first, Node* new_last)
{
  first = new_first;
  last = new_last;
}

// Retorna valor do primeiro da fila
int Deque :: Front() const 
{
  return first->val;
}

// Retorna valor do último da fila
int Deque :: Back() const 
{
  return last->val;
}

// Constrói campos parent e jump de um nó u com pai u_parent
void Deque :: BuildJumpPointer(Node* u, Node* const u_parent) 
{
  u->parent = u_parent;
  Node* x = u_parent;

  if(x == nullptr){ // Primeiro nó da fila. O jump pointer aponta para ele mesmo
    u->jump = u;
    return;
  }

  // Se a profundidade do pai tem 2 na sua representação skew binary, o jump de u é o jump do jump do seu pai. Caso contrário, o jump de u é seu pai. 
  if (x->jump != nullptr && x->depth - x->jump->depth == x->jump->depth - x->jump->jump->depth)
      u->jump = x->jump->jump;
  else
      u->jump = x;
}

// Retonar fila invertida
Deque Deque :: Swap() {
  return Deque(last,first);
}

// Insere elemento na frente da fila
Deque Deque :: PushFront(int x) 
{
  if(first == nullptr) { // Fila vazia. Criar primeiro nó com first e last apontando para ele
    Node* u = new Node(x,1); 
    BuildJumpPointer(u,first);
    return Deque(u,u);
  } 

  unsigned new_depth = first->depth + 1;
  Node* u = new Node(x,new_depth);
  BuildJumpPointer(u,first);
  return Deque(u, last);
}


// Insere elemento atrás na fila
Deque Deque :: PushBack(int x) 
{
  return Swap().PushFront(x).Swap();
}

// Ancestral de nível k do nó u
Node* Deque :: LA(Node* const u, unsigned int const k) const 
{
  Node* x = u;
  unsigned int desired_depth = x->depth - k;

  // Pulo com jump sempre que der, senão, vou para o pai.
  while (x->depth != desired_depth) {
    if(x->jump->depth >= desired_depth) 
      x = x->jump;
    else 
      x = x->parent;
  }


  return x; 
}

// Ancestral comum mais baixo de u e v
Node* Deque :: LCA(Node* const  _u, Node* const _v) const 
{

  Node* u = _u;
  Node* v = _v;

  // Quero que u aponte para o nó mais fundo entre u e v
  if(u->depth < v->depth){
    Node* tmp = v;
    v = u;
    u = tmp;
  }

  // Subo u até a profundidade de v
  u = LA(u, u->depth - v->depth);

  // Subo os dois simultaneamente até encontrar o LCA. Ele será determinado na primeira vez que o pai do apontador u coincidir com o pai do apontador v. Ele será esse pai em comum.
 if(u == v) {
    return u; // Já achei o LCA(u,v). Engloba o caso em que v é o LCA 
  }
  
 // Subo u e v simultaneamente pelos jumps sempre que der, senão, subo pelos pais.
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

// Deleta elemento da frente da fila
Deque Deque :: PopFront() 
{
  Node* new_first;
  Node* new_last;

  if(LCA(first,last) == first) { // Fila só tem um "ramo"

    unsigned int jumps_to_second; // Jumps para ir de last até o sucessor de first na fila

    if(first == last) { // Só há um elemento na fila. First == last
      jumps_to_second = 0;
      new_first = nullptr;
      new_last = nullptr;
    }
    else {
      jumps_to_second = last->depth - first->depth - 1; // First e last são distintos
      new_first = LA(last, jumps_to_second);
      new_last = last;
    }
  }

  else { // Fila tem dois "ramos"
    new_first = first->parent;
    new_last = last;
  }

  return Deque(new_first,new_last);
}

// Deleta elemento de trás da fila
Deque Deque :: PopBack() 
{
  return Swap().PopFront().Swap();
}

// Retorna valor do k-ésimo elemento da fila
int Deque :: K(unsigned int k) const 
{
  Node* mid = LCA(first,last);
  unsigned int l1,l2;
  l1 = first->depth - mid->depth + 1; // Número de nós entre first e mid, incluindo first e mid
  l2 = last->depth - mid->depth; // Número de nós entre last e mid, incluindo last e excluindo mid 

  if(k <= l1)
    return LA(first,k-1)->val;
  else
    return LA(last, l1 + l2 - k)->val;
}

// Imprime os elementos da fila em ordem numa mesma linha
std::string Deque :: Print() const 
{
  if(Size() == 0) return std::string(); // Fila vazia
  
  std::string output = std::string();

  Node* mid = LCA(first, last);

  // Imprimo de first à mid, incluindo mid
 
  unsigned int n1 = first->depth - mid->depth + 1; // Número de nós a serem impressos nesse primeiro segmento
  
  Node* x = first;
  for(int i = 0; i < n1; i++) {
    output = output + std::to_string(x->val) + " ";
    x = x->parent;
  } 

  // Imprimo de last a mid, excluindo mid, na ordem reversa
  
  unsigned int n2 = last->depth - mid->depth; // Número de nós a serem impressos nesse segundo segmento

  Node** list = new Node*[n2]; // Pilha para guardar nós na ordem reversa
  x = last;
  for(int i = n2 - 1; i >= 0; i--) {
    list[i] = x;
    x = x->parent;
  } 

  for(int i = 0; i < n2; i++)
    output = output + std::to_string(list[i]->val) + " ";

  return output;
}

// Retorna a quantidade de elementos na fila
unsigned int Deque :: Size() const 
{
  if(first == nullptr) return 0; // Fila vazia

  Node* mid = LCA(first,last);
  unsigned int l1,l2;

  l1 = first->depth - mid->depth + 1; // Número de nós entre first e mid, incluindo first e mid

  l2 = last->depth - mid->depth; // Número de nós entre last e mid, incluindo last e excluindo mid 
                                     
  return l1 + l2;
}

// Usa K() para imprimir a fila. Somente para fins de depuração.
void Deque :: Print_K() const 
{
  std::cout << Size() << ", ";
  for(int i = 1 ; i <= Size() ; i++){
    std::cout << K(i) << " ";
  }
  std::cout << "\n";
}

// Testes das funcionalidades básicas. O "grosso" está aqui. OK
void Teste1() 
{
  typedef Deque fila;
  fila f1 = fila();
  fila f2 = f1.PushFront(1);
  std::cout << f2.Print() << std::endl; // 1
  fila f3 = f2.PushFront(2);
  std::cout << f3.Print() << std::endl; // 2 1 
  fila f4 = f3.PushFront(3);
  std::cout << f4.Print() << std::endl; // 3 2 1
  fila f5 = f4.PushFront(4); 
  std::cout << f5.Print() << std::endl; // 4 3 2 1

  fila f6 = f3.PushBack(5); 
  std::cout << f6.Print() << std::endl; // 2 1 5 
  fila f7 = f6.PushBack(10);
  std::cout << f7.Print() << std::endl; // 2 1 5 10 
  fila f8 = f4.PushBack(13);
  std::cout << f8.Print() << std::endl; // 3 2 1 13
  
  fila f9 = f8.PopBack();
  std::cout << f9.Print() << std::endl; // 3 2 1 
  fila f10 = f8.PopFront();
  std::cout << f10.Print() << std::endl; // 2 1 13 
                                         
  // Testa K-th()
  
  std::cout << "************** Testando K-th() *************\n";

  f8.Print_K(); // 4, 3 2 1 13
  f9.Print_K(); // 3, 3 2 1
  f5.Print_K(); // 4, 4 3 2 1

}

// Teste dos slides. OK
void Teste2() 
{
  typedef Deque fila;
  fila f0 = fila();
  fila f1 = f0.PushBack(3);
  std::cout << f1.Print() << std::endl; // 3 
  fila f2 = f1.PushBack(4);
  std::cout << f2.Print() << std::endl; // 3 4 
  fila f3 = f2.PushFront(2);
  std::cout << f3.Print() << std::endl; // 2 3 4 
  fila f4 = f3.PushFront(1);
  std::cout << f4.Print() << std::endl; // 1 2 3 4 
  fila f5 = f3.PopBack();
  std::cout << f5.Print() << std::endl; // 2 3 
  fila f6 = f5.PopBack();
  std::cout << f6.Print() << std::endl; // 2 
  fila f7 = f6.PushFront(8);
  std::cout << f7.Print() << std::endl; // 8 2 
  fila f8 = f6.PopFront();
  std::cout << f8.Print() << std::endl; //  
  fila f9 = f8.PushFront(6);
  std::cout << f9.Print() << std::endl; // 6 
}


// Para depurar função PopFront(), que não deleta quando só tem um elemento. OK
void Teste3() {
  typedef Deque fila;
  fila f0 = fila();
  fila f1 = f0.PushFront(1);
  std::cout << f1.Print() << std::endl;

  fila f2 = f1.PopFront();
  std::cout << f2.Print() << std::endl;
}

// Cliente para teste da tarefa
void Teste4() {
  typedef Deque fila;
  std::vector<fila> v = std::vector<fila>(); 

  v.push_back(fila()); // Cria primeira fila vazia
  
  for(std::string line; std::getline(std::cin, line) ; ) {
    // Extrai linha da entrada padrão
    int opt, t, x;
    std::istringstream iss = std::istringstream(line);
    iss >> opt >> t >> x;

    switch (opt) {
      case 1:
        v.push_back(v[t].PushFront(x));
        break;
      case 2:
        v.push_back(v[t].PushBack(x));
        break;
      case 3:
        v.push_back(v[t].PopFront());
        break;
      case 4:
        v.push_back(v[t].PopBack());
        break;
      case 5:
        std::cout << v[t].Front() << std::endl;
        break;
      case 6:
        std::cout << v[t].Back() << std::endl;
        break;
      case 7:
        std::cout << v[t].K(x) << std::endl;
        break;
      case 8:
        std::cout << v[t].Print() << std::endl;
        break;
    }

  }

}

int main (int argc, char *argv[]) {
  //Teste1();
  //Teste2();
  //Teste3();
  Teste4();
  return 0;
}
