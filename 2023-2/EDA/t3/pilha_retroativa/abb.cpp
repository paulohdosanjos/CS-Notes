// Tarefa t3 - Abb com suporte a Sum e Top
// Paulo Henrique Albuquerque
// 12542251

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <assert.h>


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
    int smax; // maior sufixo na árvore
    int sum; // soma de créditos na subárvore


    Node* left;
    Node* right;
   
    Node(const int _key, int _priority, int _value, int _operation, int _max, int _smax, int _sum, Node* l, Node* r): key(_key), priority(_priority), value(_value), operation(_operation), max(_max), smax(_smax), sum(_sum), left(l), right(r) {}
};

class Abb {

  public:
    
    Abb() {std::srand(5); root = nullptr;}

    // Insere folha de operação op (valor x caso seja push) na árvore, no instante t. Caso já exista um nó com essa chave, nada acontece.
    void Insert(const int t, const int op, const int x) { root = _Insert(root, t, op, x); }

    //int K(int k){ return _K(root, k); }

    //int Count(int c) { return _Count(root, c); }

    //int K(int k) { return _K(root, k); }
    
    // Remove folha com chave x da árvore 
    void Delete(const int t) { root = _Delete(root, t); }

    // Verifica se folha com chave x está na árvore
    //bool Search(int x) const { return _Search(root, x); }
    
    // Retorna a menor folha da árvore
    //int Min() const { return _Min(root)->key; }

    // Imprime a árvore 
    void Print() const { _Print(root, 0); }

    //int Size() const { return root->num; }
    
  private:
    Node* root;
 
    // Insere uma nova chave x na árvore enraizada em r
    Node* _Insert(Node* r, const int t, const int op, const int x)
    {
      if(r == nullptr) return new Node(t, -INFTY, x, op, t, (op == -1 ? 0 : 1), op, nullptr, nullptr); // Árvore vazia 
      
      if(r->left == nullptr && r->right == nullptr)
      { 
        // r é folha. Cria novo nó interno pai de r e da nova folha a ser adicionada
        Node* new_leaf = new Node(t, -INFTY, x, op, t, (op == -1 ? 0 : 1), op, nullptr, nullptr);
        Node* new_parent;
        assert(t != r->key);

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
      assert(t != r->left->max);
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
    
    // Deleta folha de chave t da árvore enraizada em u. Devolve a treap resultante. Se a chave não existir, nada acontece 
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
      // Como as rotações só acontecem entre nós internos, todos os campos estão devidamente inicializados nesse ponto
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

    // Retorna o número de chaves menores que c na árvore enraizada em r
//    int _Count(Node* r, int k)
//    {
//      if(r == nullptr) return 0;
//      if(r->left == nullptr && r->right == nullptr) return r->key <= k;
//      if(r->left->max <= k) return r->left->num + _Count(r->right, k);
//      else return _Count(r->left, k);
//    }
//    
//    // Retorna a k-ésima maior chave da árvore enraizada em r
//    int _K(Node* r, int k)
//    {
//      if(r->left == nullptr && r->right == nullptr) {
//        assert(k == 1);
//        return r->value;
//      }
//
//      if(r->left->num >= k) return _K(r->left, k);
//      else return _K(r->right, k - r->left->num);
//    }
    // Retonar a menor chave da árvore enraizada em u
    //Node* _Min(Node* u) const
    //{
    //  if(u == nullptr) return nullptr; // Entra somente quando a árvore está vazia
    //  if(u->left != nullptr) return _Min(u->left);
    //  else return u;
    //}


    //// Verifica se a chave x está presente na árvore enraizada em r.
    //bool _Search(const Node* const r, const int x) const
    //{
    //  if(r == nullptr) return false;

    //  if(x == r->key) return true;
    //  else if(x < r->key) return _Search(r->left, x);
    //  else return _Search(r->right, x);
    //}

    // Imprime nó u depois de i espaços na saída padrão
    void _Print(Node* u, int i) const
    {
      if(u == nullptr) return;
      _Print(u->left, i + 4); 
      std::string space (i, ' '); 
      std::cout << space;
      //std::cout << u->key << " " << u->priority << " " << u->max << " " << u->sum << " " << u->smax << "\n";
      if(u->left == nullptr && u->right == nullptr) std::cout << (u->operation == 1 ? "push(" + std::to_string(u->value) + ")" : "pop()") << "\n";
      else std::cout << u->sum << " " << u->smax << "\n";
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

int main()
{
  //Teste1();
  Teste2();
  //Teste3();
  //Teste4();
  //Teste5();
}
