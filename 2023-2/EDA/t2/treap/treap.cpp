// Tarefa t2 - Treap normal
// Paulo Henrique Albuquerque
// 12542251

#include <iostream>
#include <string>
#include <sstream>
#include <vector>


#define MAX_PRIORITY 1001

#define _rand() (std::rand() % MAX_PRIORITY)

class Node {
  public:
    int priority;
    int key;
    Node* left;
    Node* right;
   
    Node(const int _key, int _priority, int _num_left, Node* l, Node* r): key(_key), priority(_priority), left(l), right(r) {}
};

class Treap {

  public:
    
    Treap() {std::srand(5); root = nullptr;}

    // Insere um nó com chave x na treap. Caso já exista um nó com essa chave, nada acontece.
    void Insert(const int x) { root = _Insert(root, x); }
    
    // Remove nó com chave x da treap 
    void Delete(const int x) { root = _Delete(root, x); }

    // Verifica se chave x está na árvore
    bool Search(int x) const { return _Search(root, x); }
    
    // Retorna a menor chave da árvore
    int Min() const { return _Min(root)->key; }

    // Imprime a árvore 
    void Print() const { _Print(root, 0); }
    
  private:
    Node* root;
 
    // Insere um novo nó com chave x na árvore enraizada em r
    Node* _Insert(Node* r, const int x)
    {
      if(r == nullptr) return new Node(x, _rand(), 0, nullptr, nullptr); // Árvore vazia
      
      if(x > r->key) 
      {
        r->right = _Insert(r->right, x);

        if(r->right->priority > r->priority)
        {
          Node* child = r->right;
          RotateLeft(r, child);
          r = child;
        }
      }
     
      else if(x < r->key)
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
    Node* _Delete(Node* u, int const x)
    {
      if(u == nullptr) return nullptr; // Chave não encontrada

      if(x > u->key)
      {
        u->right = _Delete(u->right, x);
        return u;
      }

      else if(x < u->key)
      {
        u->left =_Delete(u->left, x); 
        return u; 
      }

      else // x == u->key
      { 
        // Só faço o shortcut
        if(u->left == nullptr) return u->right; 
        if(u->right == nullptr) return u->left;

        else //Tem os dois filhos. Desço rotacionando para manter a propriedade de heap
        { 
          // Determina qual filho toma o lugar de u
          Node* child;
          if(u->left->priority > u->right->priority){
            child = u->left;
            RotateRight(u, child);
            child->right = _Delete(child->right, x);
          }
          else{
            child = u->right;
            RotateLeft(u, child);
            child->left = _Delete(child->left, x);
          }
          return child;
        }
      }
    }

    void RotateLeft(Node* u, Node* child)
    {
      u->right = child->left;
      child->left = u;
    }

    void RotateRight(Node* u, Node* child)
    {
      u->left = child->right;
      child->right = u;
    }

    // Retonar a menor chave da árvore enraizada em u
    Node* _Min(Node* u) const
    {
      if(u == nullptr) return nullptr; // Entra somente quando a árvore está vazia
      if(u->left != nullptr) return _Min(u->left);
      else return u;
    }


    // Verifica se a chave x está presente na árvore enraizada em r.
    bool _Search(const Node* const r, const int x) const
    {
      if(r == nullptr) return false;

      if(x == r->key) return true;
      else if(x < r->key) return _Search(r->left, x);
      else return _Search(r->right, x);
    }

    // Imprime nó u depois de i espaços na saída padrão
    void _Print(Node* u, int i) const
    {
      if(u == nullptr) return;
      _Print(u->left, i + 3); 
      std::string space (i, ' '); 
      std::cout << space;
      std::cout << u->key << " " << u->priority << "\n";
      _Print(u->right, i + 3);
    }

};

// Cliente teste da tarefa
int main()
{
  Treap t = Treap();
  for(std::string line; std::getline(std::cin, line) ; ) {
    int opt, x;
    std::istringstream iss = std::istringstream(line);
    iss >> opt >> x;
    //std::cout << opt << " " << x << "\n";

    switch (opt) {
      case 1:
        //std::cout << "oi\n";
        t.Insert(x);
        break;
      case 2:
        t.Delete(x);
        break;
      case 3:
        std::cout << (t.Search(x) ? "1" : "0") << "\n";
        break;
      case 4:
        std::cout << t.Min() << "\n";
        break;
      case 5:
        t.Print();
        break;
    }
  }
}
