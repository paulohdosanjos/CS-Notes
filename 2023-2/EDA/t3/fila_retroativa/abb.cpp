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
    int value;
    int max; // maior chave na subárvore
    int num; // número de elementos na subárvore

    Node* left;
    Node* right;
   
    Node(const int _key, int _priority, int _value, int _max, int _num, Node* l, Node* r): key(_key), priority(_priority), value(_value), max(_max), num(_num), left(l), right(r) {}
};

class Abb {

  public:
    
    Abb() {std::srand(5); root = nullptr;}

    // Insere folha x na árvore. Caso já exista um nó com essa chave, nada acontece.
    void Insert(const int t, const int x) { root = _Insert(root, t, x); }

    //int K(int k){ return _K(root, k); }

    int Count(int c) { return _Count(root, c); }

    int K(int k) { return _K(root, k); }
    
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
    Node* _Insert(Node* r, const int t, const int x)
    {
      if(r == nullptr) return new Node(t, -INFTY, x, t, 1, nullptr, nullptr); // Árvore vazia 
      
      if(r->left == nullptr && r->right == nullptr)
      { 
        // r é folha. Cria novo nó interno pai de r e da nova folha a ser adicionada
        Node* new_leaf = new Node(t, -INFTY, x, t, 1, nullptr, nullptr);
        Node* new_parent;
        assert(t != r->key);

        if(t > r->key)
          new_parent = new Node(GARBAGE, _rand(), GARBAGE, t, 2, r, new_leaf); 

        else
          new_parent = new Node(GARBAGE, _rand(), GARBAGE, r->key, 2, new_leaf, r);
        
        return new_parent;
      }

      // r é nó interno
      assert(t != r->left->max);
      r->num++;
      r->max = std::max(r->max, t);
      if(t > r->left->max) 
      {
        r->right = _Insert(r->right, t, x);

        if(r->right->priority > r->priority)
        {
          Node* child = r->right;
          RotateLeft(r, child);
          r = child;
        }
      }
     
      else 
      {
        r->left = _Insert(r->left, t, x);

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

      r->num--;
      r->max = std::max(r->left->max, r->right->max);
      return r;
    }

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
      if(r == nullptr) return 0;
      if(r->left == nullptr && r->right == nullptr) return r->key <= k;
      if(r->left->max <= k) return r->left->num + _Count(r->right, k);
      else return _Count(r->left, k);
    }
    
    // Retorna a k-ésima maior chave da árvore enraizada em r
    int _K(Node* r, int k)
    {
      if(r->left == nullptr && r->right == nullptr) {
        assert(k == 1);
        return r->value;
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
//void Teste1()
//{
//  Abb a = Abb();
//  int list[] = {5, 9, 14, 16, 20, 40};
//  int size = (int) (sizeof(list) / sizeof(list[0]));
//  for(int i = 0; i < size; i++){
//    a.Insert(list[i], GARBAGE);
//    a.Print();
//    std::cout << "*****************\n";
//  }
//}
//
//// Teste incial para count. Parece OK
void Teste2()
{
  Abb a = Abb();
  int list[] = {5, 9, 16, 20, 40, 33, 3, 11, 23, 14, 41, 19};
  int size = (int) (sizeof(list) / sizeof(list[0]));
  for(int i = 0; i < size; i++){
    a.Insert(list[i], GARBAGE);
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
  std::cout << a.Count(INFTY) << "\n"; // 12
}
//
//// Teste inicial para K-th(). Parece OK
//void Teste3()
//{
//  Abb a = Abb();
//  int list[] = {5, 9, 16, 20, 40, 33, 3, 11, 23, 14, 41, 19};
//  int size = (int) (sizeof(list) / sizeof(list[0]));
//  for(int i = 0; i < size; i++){
//    a.Insert(list[i], GARBAGE);
//  }
//  a.Print();
//
//  for(int i = 1; i <= size; i++){
//    std::cout << a.K(i) << "\n";
//  }
//}
//
// Boss Fight: Teste para Delete(). Parece OK
void Teste4()
{
  Abb a = Abb();
  int insert_list[] = {5, 9, 16, 20, 40, 33, 3, 11, 23, 14, 41, 19};
  int size = (int) (sizeof(insert_list) / sizeof(insert_list[0]));
  for(int i = 0; i < size; i++){
    a.Insert(insert_list[i], GARBAGE);
  }

  std::cout << "Antes de deletar:\n";
  a.Print();
  std::cout << "*********************\n";

  std::cout << "Deleta chave inexistente\n";
  a.Delete(202); // Deleta chave que não existe na árvore
  a.Print();
  std::cout << "*********************\n";


  int delete_list[] = {9, 5, 23, 41, 40, 19, 3, 11, 16, 14, 20, 33};
  for(int i = 0; i < size; i++){
    std::cout << "Deleta " << delete_list[i] << "\n";
    a.Delete(delete_list[i]);
    a.Print();
    std::cout << "*********************\n";
  }
}
//


// Degubar Count: Count(INFTY) para uma árvore com um elemento tá dando pau no teste de fila_retroativa
void Teste5()
{
  Abb a = Abb();
  a.Insert(10, GARBAGE);
  std::cout << a.Count(INFTY) << "\n"; // 1
}

int main()
{
  //Teste1();
  //Teste2();
  //Teste3();
  Teste4();
  //Teste5();
}
