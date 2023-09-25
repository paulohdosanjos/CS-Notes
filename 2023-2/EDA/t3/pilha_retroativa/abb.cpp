// Tarefa t3 - Abb com suporte a K-th() e Top()
// Paulo Henrique Albuquerque
// 12542251

#include <iostream>
#include <utility>


#define MAX_PRIORITY 1001
#define INFTY 12345
#define GARBAGE -42

#define _rand() (std::rand() % MAX_PRIORITY)

class Node {
  public:
    int key;
    int priority;
    int value;
    int operation; // push = +1 e pop = -1
    int max; // maior chave na subárvore
    int smax; // maior sufixo na subárvore
    int sum; // soma de créditos na subárvore


    Node* left;
    Node* right;
   
    Node(const int _key, int _priority, int _value, int _operation, int _max, int _smax, int _sum, Node* l, Node* r): key(_key), priority(_priority), value(_value), operation(_operation), max(_max), smax(_smax), sum(_sum), left(l), right(r) {}
};

class Abb {

  public:
    
    Abb() {std::srand(5); root = nullptr;}

    // Insere folha de operação op (de valor x caso seja push) na árvore no instante t. 
    void Insert(const int t, const int op, const int x) { root = _Insert(root, t, op, x); }

    // Remove folha com chave t da árvore 
    void Delete(const int t) { root = _Delete(root, t); }
    
    // Retorna a soma dos "créditos das folhas com chaves <= t"
    int Sum(const int t) { return _Sum(root, t); }

    // Retorna k-ésima elemento da pilha do instante t
    int K(const int t, const int k)
    {
      std::pair p = _FindSubtree(root, t, k);
      return _K(p.second, p.first);
    }

    // Retorna o elemento do topo da pilha do instante t
    int Top(const int t){ return K(t, 1); }

    // Imprime a árvore. Para depuração
    void Print() const { _Print(root, 0); }

  private:
    Node* root;

    // Acha t' na subárvore r.
    int _K(Node* r, int k)
    {
      if(r->right == nullptr && r->left == nullptr) return r->value;
      if(r->right->smax >= k){
        return _K(r->right, k);
      }
      else return _K(r->left, k - r->right->sum);
    }
    // Retorna a soma dos créditos das folhas com chave <= t na árvore enraizada em r
    int _Sum(Node* r, const int t)
    {
      if(r->left == nullptr && r->right == nullptr){
         return r->operation;
      } 

      if(t > r->left->max) return r->left->sum + _Sum(r->right, t);
      else return _Sum(r->left, t);
    }

    // Retorna subárvore que contém t'
    std::pair<int, Node*> _FindSubtree(Node* r, const int t, const int k)
    {
      if(r->left == nullptr && r->right == nullptr) // Folha
      {
        if(r->operation == 1) return std::make_pair(k-1, (k-1 == 0 ? r : nullptr));
        else return std::make_pair(2 + k-1, nullptr);

      }

      // Nó interno

      std::pair<int, Node*> p;
      if(t > r->left->max)
      { 
        p = _FindSubtree(r->right, t, k);
        if(p.second == nullptr) // Ainda não achou
        { 
          if(r->left->smax >= p.first){ // Achei
            return std::make_pair(p.first, r->left);
          }
          else return std::make_pair(p.first - r->left->sum, nullptr);
        }
        else // Achei a subárvore, retorna lá pra cima
         return p; 
      }
      else return _FindSubtree(r->left, t, k);
    }
    // Insere uma nova chave x na árvore enraizada em r
    Node* _Insert(Node* r, const int t, const int op, const int x)
    {
      if(r == nullptr) return new Node(t, -INFTY, x, op, t, (op == -1 ? 0 : 1), op, nullptr, nullptr); // Árvore vazia 
      
      if(r->left == nullptr && r->right == nullptr)
      { 
        // r é folha. Cria novo nó interno pai de r e da nova folha a ser adicionada
        Node* new_leaf = new Node(t, -INFTY, x, op, t, (op == -1 ? 0 : 1), op, nullptr, nullptr);
        Node* new_parent;

        if(t > r->key){
          int smax = std::max(std::max(0, op), op + r->operation); // Máximo entre os três sufixos possivéis
          new_parent = new Node(GARBAGE, _rand(), GARBAGE, GARBAGE, t, smax, op + r->sum, r, new_leaf); 
        }
        else {
          int smax = std::max(std::max(0, r->operation), op + r->operation); // Máximo entre os três sufixos possivéis
          new_parent = new Node(GARBAGE, _rand(), GARBAGE, GARBAGE, r->key, smax, op + r->sum, new_leaf, r);
        }
        
        return new_parent;
      }

      // r é nó interno
      r->max = std::max(r->max, t);
      if(t > r->left->max) 
      {
        r->right = _Insert(r->right, t, op, x);

        r->sum = r->left->sum + r->right->sum;
        r->smax = std::max(r->right->smax, r->right->sum + r->left->smax);

        if(r->right->priority > r->priority)
        {
          Node* child = r->right;
          RotateLeft(r, child);
          r = child;
        }
      }
     
      else 
      {
        r->left = _Insert(r->left, t, op, x);

        r->sum = r->left->sum + r->right->sum;
        r->smax = std::max(r->right->smax, r->right->sum + r->left->smax);

        if(r->left->priority > r->priority)
        {
          Node* child = r->left;
          RotateRight(r, child);
          r = child;
        }
    }

    return r;
    }
    
    // Deleta folha de chave t da árvore enraizada em u. Devolve a árvore resultante.
    Node* _Delete(Node* r, int const t)
    {
      if(r == nullptr) return nullptr; // Árvore vazia
      if(r->left == nullptr && r->right == nullptr) 
      {
        if(r->key == t) return nullptr; // Chave encontrada
        else return r; // Chave não existe na árvore. Não faz nada
      }

      if(t > r->left->max)
      {
        r->right = _Delete(r->right, t);
        if(r->right == nullptr) return r->left; // Shortcut
      }

      else
      {
        r->left =_Delete(r->left, t); 
        if(r->left == nullptr) return r->right; // Shortcut
      }

      r->max = std::max(r->left->max, r->right->max);
      r->sum = r->left->sum + r->right->sum;
      r->smax = std::max(r->right->smax, r->right->sum + r->left->smax);

      return r;
    }

    // v é o pai e u é o filho
    void RotateLeft(Node* v, Node* u)
    {
      // Como as rotações só acontecem nos nós internos, todos os campos estão devidamente inicializados nesse ponto
      int new_v_max = std::max(u->left->max, v->left->max);
      int new_v_sum = u->left->sum + v->left->sum;
      int new_v_smax = std::max(u->left->smax, u->left->sum + v->left->smax); 

      int new_u_max = v->max;
      int new_u_sum = v->sum;
      int new_u_smax = v->smax;

      // Atualiza campos
      v->max = new_v_max;
      v->sum = new_v_sum;
      v->smax = new_v_smax;
      u->max = new_u_max;
      u->sum = new_u_sum;
      u->smax = new_u_smax;

      // Atualiza ponteiros
      v->right = u->left;
      u->left = v;
    }

    // u é o pai e v é o filho
    void RotateRight(Node* u, Node* v)
    {
      // Como as rotações só acontecem entre nós internos, todos os campos estão devidamente inicializados nesse ponto
      int new_u_max = std::max(u->right->max, v->right->max);
      int new_u_sum = u->right->sum + v->right->sum;
      int new_u_smax = std::max(u->right->smax, u->right->sum + u->left->smax); 

      int new_v_max = u->max;
      int new_v_sum = u->sum;
      int new_v_smax = u->smax; 

      // Atualiza campos
      u->max = new_u_max;
      u->sum = new_u_sum;
      u->smax = new_u_smax;
      v->max = new_v_max;
      v->sum = new_v_sum;
      v->smax = new_v_smax;

      // Atualiza ponteiros
      u->left = v->right;
      v->right = u;
    }

    // Imprime nó u depois de i espaços na saída padrão
    void _Print(Node* u, int i) const
    {
      if(u == nullptr) return;
      _Print(u->left, i + 4); 
      std::string space (i, ' '); 
      std::cout << space;
      if(u->left == nullptr && u->right == nullptr) std::cout << (u->operation == 1 ? "push(" + std::to_string(u->value) + ")" : "pop()") << "\n";
      else std::cout << u->sum << " " << u->smax << " " << u->priority<< "\n";
      _Print(u->right, i + 4);
    }

};

// Teste inicial para insert. Usa o exemplo de árvore dos slides. A priori, parece OK
void Teste1()
{
  Abb a = Abb();
  a.Insert(1, 1, 1); // push(1)
  a.Insert(2, 1, 2); // push(2)
  a.Insert(3, -1, GARBAGE); // pop()
  a.Insert(4, 1, 3); // push(3)
  a.Insert(5, -1, GARBAGE); // pop()
  a.Insert(6, 1, 4); // push(4)
  a.Insert(7, 1, 5); // push(5)
  a.Insert(8, -1, GARBAGE); // pop()
  a.Print();
}

// Teste inicial para delete. Parece OK
void Teste2()
{
  Abb a = Abb();
  a.Insert(1, 1, 1); // push(1)
  a.Insert(2, 1, 2); // push(2)
  a.Insert(3, -1, GARBAGE); // pop()
  a.Insert(4, 1, 3); // push(3)
  a.Insert(5, -1, GARBAGE); // pop()
  a.Insert(6, 1, 4); // push(4)
  a.Insert(7, 1, 5); // push(5)
  a.Insert(8, -1, GARBAGE); // pop()
  std::cout << "Antes de deletar:\n";
  a.Print();
  std::cout << "******************\n";

  int delete_list[] = {2, 5, 7, 1, 3, 6, 8, 4};
  int size = (int) (sizeof(delete_list) / sizeof(delete_list[0]));
  for(int i = 0; i < size; i ++){
    std::cout << "Deleta " << delete_list[i] << "\n";
    a.Delete(delete_list[i]);
    a.Print();
    std::cout << "******************\n";
  }
}

// Teste inicial para Sum. Parece OK
//void Teste3()
//{
//  Abb a = Abb();
//  a.Insert(1, 1, 1); // push(1)
//  a.Insert(2, 1, 2); // push(2)
//  a.Insert(3, -1, GARBAGE); // pop()
//  a.Insert(4, 1, 3); // push(3)
//  a.Insert(5, -1, GARBAGE); // pop()
//  a.Insert(6, 1, 4); // push(4)
//  a.Insert(7, 1, 5); // push(5)
//  a.Insert(8, -1, GARBAGE); // pop()
//
//  std::cout << a.Sum(1) << "\n"; // 1
//  std::cout << a.Sum(2) << "\n"; // 2
//  std::cout << a.Sum(3) << "\n"; // 1
//  std::cout << a.Sum(4) << "\n"; // 2
//  std::cout << a.Sum(5) << "\n"; // 1
//  std::cout << a.Sum(6) << "\n"; // 2
//  std::cout << a.Sum(7) << "\n"; // 3
//  std::cout << a.Sum(8) << "\n"; // 2
//}

// Teste inicial para Top. Parece OK
void Teste4()
{
  Abb a = Abb();
  a.Insert(1, 1, 1); // push(1)
  a.Insert(2, 1, 5); // push(5)
  a.Insert(3, -1, GARBAGE); // pop()
  a.Insert(4, 1, 6); // push(6)
  a.Insert(5, -1, GARBAGE); // pop()
  a.Insert(6, 1, 0); // push(0)
  a.Insert(7, 1, 2); // push(2)
  a.Insert(8, -1, GARBAGE); // pop()
  a.Insert(9, -1, GARBAGE); // pop()
  a.Insert(10, 1, 3); // push(3)
  a.Insert(11, 1, 10); // push(10)
  a.Insert(12, 1, 7); // push(7)
  a.Insert(13, -1, GARBAGE); // pop()
  a.Print();

  std::cout << a.Top(1) << "\n"; // 1
  std::cout << a.Top(2) << "\n"; // 5
  std::cout << a.Top(3) << "\n"; // 1
  std::cout << a.Top(4) << "\n"; // 6
  std::cout << a.Top(5) << "\n"; // 1 
  std::cout << a.Top(6) << "\n"; // 0
  std::cout << a.Top(7) << "\n"; // 2
  std::cout << a.Top(8) << "\n"; // 0
  std::cout << a.Top(9) << "\n"; // 1
  std::cout << a.Top(10) << "\n"; // 3
  std::cout << a.Top(11) << "\n"; // 10
  std::cout << a.Top(12) << "\n"; // 7
  std::cout << a.Top(13) << "\n"; // 10
  std::cout << a.Top(14) << "\n"; // 10
  std::cout << a.Top(15) << "\n"; // 10
}

// Teste inicial para K-th(). Parece OK
void Teste5()
{
  Abb a = Abb();
  a.Insert(1, 1, 1); // push(1)
  a.Insert(2, 1, 5); // push(5)
  a.Insert(3, -1, GARBAGE); // pop()
  a.Insert(4, 1, 6); // push(6)
  a.Insert(5, -1, GARBAGE); // pop()
  a.Insert(6, 1, 0); // push(0)
  a.Insert(7, 1, 2); // push(2)
  a.Insert(8, -1, GARBAGE); // pop()
  a.Insert(9, -1, GARBAGE); // pop()
  a.Insert(10, 1, 3); // push(3)
  a.Insert(11, 1, 10); // push(10)
  a.Insert(12, 1, 7); // push(7)
  a.Insert(13, -1, GARBAGE); // pop()
  a.Print();

  std::cout << a.K(1, 1) << "\n"; // 1

  std::cout << a.K(2, 1) << "\n"; // 5
  std::cout << a.K(2, 2) << "\n"; // 1

  std::cout << a.K(3, 1) << "\n"; // 1

  std::cout << a.K(4, 1) << "\n"; // 6
  std::cout << a.K(4, 2) << "\n"; // 1

  std::cout << a.K(5, 1) << "\n"; // 1

  std::cout << a.K(6, 1) << "\n"; // 0
  std::cout << a.K(6, 2) << "\n"; // 1

  
  std::cout << a.K(7, 1) << "\n"; // 2
  std::cout << a.K(7, 2) << "\n"; // 0
  std::cout << a.K(7, 3) << "\n"; // 1

  
  std::cout << a.K(8, 1) << "\n"; // 0
  std::cout << a.K(8, 2) << "\n"; // 1

  
  std::cout << a.K(10, 1) << "\n"; // 3
  std::cout << a.K(10, 2) << "\n"; // 1


  std::cout << a.K(11, 1) << "\n"; // 10
  std::cout << a.K(11, 2) << "\n"; // 3
  std::cout << a.K(11, 3) << "\n"; // 1


  std::cout << a.K(12, 1) << "\n"; // 7
  std::cout << a.K(12, 2) << "\n"; // 10
  std::cout << a.K(12, 3) << "\n"; // 3
  std::cout << a.K(12, 4) << "\n"; // 1

  std::cout << a.K(13, 1) << "\n"; // 10
  std::cout << a.K(13, 2) << "\n"; // 3
  std::cout << a.K(13, 3) << "\n"; // 1

  std::cout << a.K(14, 1) << "\n"; // 10
  std::cout << a.K(14, 2) << "\n"; // 3
  std::cout << a.K(14, 3) << "\n"; // 1



}


//int main()
//{
//  //Teste1();
//  //Teste2();
//  //Teste3();
//  //Teste4();
//  Teste5();
//}
