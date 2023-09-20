// Tarefa t3 - Abb com suporte a count e k-th
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
    int max; // maior chave na subárvore
    int num; // número de elementos na subárvore

    Node* left;
    Node* right;
   
    Node(const int _key, int _priority, int _max, int _num, Node* l, Node* r): key(_key), priority(_priority), max(_max), num(_num), left(l), right(r) {}
};

class Abb {

  public:
    
    Abb() {std::srand(5); root = nullptr;}

    // Insere folha x na árvore. Caso já exista um nó com essa chave, nada acontece.
    void Insert(const int x) { root = _Insert(root, x); }

    //int K(int k){ return _K(root, k); }

    int Count(int c) { return _Count(root, c); }

    int K(int k) { return _K(root, k); }
    
    // Remove folha com chave x da treap 
    //void Delete(const int x) { root = _Delete(root, x); }

    // Verifica se folha com chave x está na árvore
    //bool Search(int x) const { return _Search(root, x); }
    
    // Retorna a menor folha da árvore
    //int Min() const { return _Min(root)->key; }

    // Imprime a árvore 
    void Print() const { _Print(root, 0); }
    
  private:
    Node* root;
 
    // Insere uma nova chave x na árvore enraizada em r
    Node* _Insert(Node* r, const int x)
    {
      if(r == nullptr) return new Node(x, -INFTY, x, 1, nullptr, nullptr); // Árvore vazia 
      
      if(r->left == nullptr && r->right == nullptr)
      { 
        // r é folha. Cria novo nó interno pai de r e da nova folha a ser adicionada
        Node* new_leaf = new Node(x, -INFTY, x, 1, nullptr, nullptr);
        Node* new_parent;
        assert(x != r->key);

        if(x > r->key)
          new_parent = new Node(GARBAGE, _rand(), x, 2, r, new_leaf); 

        else
          new_parent = new Node(GARBAGE, _rand(), r->key, 2, new_leaf, r);
        
        return new_parent;
      }

      // r é nó interno
      assert(x != r->left->max);
      r->num++;
      r->max = std::max(r->max, x);
      if(x > r->left->max) 
      {
        r->right = _Insert(r->right, x);

        if(r->right->priority > r->priority)
        {
          Node* child = r->right;
          RotateLeft(r, child);
          r = child;
        }
      }
     
      else 
      {
        r->left = _Insert(r->left, x);

        if(r->left->priority > r->priority)
        {
          Node* child = r->left;
          RotateRight(r, child);
          r = child;
        }
    }

    return r;
    }
    
    // Deleta nó de chave x da treap enraizada em u. Devolve a treap resultante. Se a chave não existir na treap, nada acontece 
//    Node* _Delete(Node* u, int const x)
//    {
//      if(u == nullptr) return nullptr; // Chave não encontrada
//
//      if(x > u->key)
//      {
//        u->right = _Delete(u->right, x);
//        return u;
//      }
//
//      else if(x < u->key)
//      {
//        u->left =_Delete(u->left, x); 
//        return u; 
//      }
//
//      else // x == u->key
//      { 
//        // Só faço o shortcut
//        if(u->left == nullptr) return u->right; 
//        if(u->right == nullptr) return u->left;
//
//        else //Tem os dois filhos. Desço rotacionando para manter a propriedade de heap
//        { 
//          // Determina qual filho toma o lugar de u
//          Node* child;
//          if(u->left->priority > u->right->priority){
//            child = u->left;
//            RotateRight(u, child);
//            child->right = _Delete(child->right, x);
//          }
//          else{
//            child = u->right;
//            RotateLeft(u, child);
//            child->left = _Delete(child->left, x);
//          }
//          return child;
//        }
//      }
//    }
//
    // v é o pai e u é o filho
    void RotateLeft(Node* v, Node* u)
    {
      // Como as rotações só acontecem entre nós internos, todos os campos estão devidamente inicializados nesse ponto
      int new_v_max = std::max(u->left->max, v->left->max);
      int new_v_num = u->left->num + v->left->num;
      int new_u_max = v->max;
      int new_u_num = v->num;

      // Atualiza campos
      v->max = new_v_max;
      v->num = new_v_num;
      u->max = new_u_max;
      u->num = new_u_num;

      // Atualiza ponteiros
      v->right = u->left;
      u->left = v;
    }

    // u é o pai e v é o filho
    void RotateRight(Node* u, Node* v)
    {
      // Como as rotações só acontecem entre nós internos, todos os campos estão devidamente inicializados nesse ponto
      int new_u_max = std::max(u->right->max, v->right->max);
      int new_u_num = u->right->num + v->right->num;
      int new_v_max = u->max;
      int new_v_num = u->num;

      // Atualiza campos
      u->max = new_u_max;
      u->num = new_u_num;
      v->max = new_v_max;
      v->num = new_v_num;

      // Atualiza ponteiros
      u->left = v->right;
      v->right = u;
    }

    // Retorna o número de chaves menores que c na árvore enraizada em r
    int _Count(Node* r, int k)
    {
      if(r->left == nullptr && r->right == nullptr) return r->key <= k;
      if(r->left->max <= k) return r->left->num + _Count(r->right, k);
      else return _Count(r->left, k);
    }
    
    // Retorna a k-ésima maior chave da árvore enraizada em r
    int _K(Node* r, int k)
    {
      if(r->left == nullptr && r->right == nullptr) {
        assert(k == 1);
        return r->key;
      }

      if(r->left->num >= k) return _K(r->left, k);
      else return _K(r->right, k - r->left->num);
    }
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
      std::cout << u->key << " " << u->priority << " " << u->max << " " << u->num << "\n";
      _Print(u->right, i + 3);
    }

};

// Teste inicial para insert. Parece OK
void Teste1()
{
  Abb a = Abb();
  int list[] = {5, 9, 14, 16, 20, 40};
  int size = (int) (sizeof(list) / sizeof(list[0]));
  for(int i = 0; i < size; i++){
    a.Insert(list[i]);
    a.Print();
    std::cout << "*****************\n";
  }
}

// Teste incial para count. Parece OK
void Teste2()
{
  Abb a = Abb();
  int list[] = {5, 9, 16, 20, 40, 33, 3, 11, 23, 14, 41, 19};
  int size = (int) (sizeof(list) / sizeof(list[0]));
  for(int i = 0; i < size; i++){
    a.Insert(list[i]);
  }
  a.Print();
  std::cout << a.Count(-10) << "\n"; // 0
  std::cout << a.Count(0) << "\n"; // 0
  std::cout << a.Count(2) << "\n"; // 0
  std::cout << a.Count(5) << "\n"; // 2
  std::cout << a.Count(10) << "\n"; // 3
  std::cout << a.Count(22) << "\n"; // 8
  std::cout << a.Count(25) << "\n"; // 9
  std::cout << a.Count(40) << "\n"; // 11
  std::cout << a.Count(50) << "\n"; // 12
}

// Teste inicial para K-th(). Parece OK
void Teste3()
{
  Abb a = Abb();
  int list[] = {5, 9, 16, 20, 40, 33, 3, 11, 23, 14, 41, 19};
  int size = (int) (sizeof(list) / sizeof(list[0]));
  for(int i = 0; i < size; i++){
    a.Insert(list[i]);
  }
  a.Print();

  for(int i = 1; i <= size; i++){
    std::cout << a.K(i) << "\n";
  }
}

int main()
{
  //Teste1();
  //Teste2();
  Teste3();
}
