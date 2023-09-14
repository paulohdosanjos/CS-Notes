// Tarefa t2 - Treap persistente
// Paulo Henrique Albuquerque
// 12542251

#include <iostream>
#include <sstream>
#include <vector>
#include <string>


#define MAX_PRIORITY 1001

#define _rand() (std::rand() % MAX_PRIORITY)

class Node {
  public:
    int priority;
    int key;
    Node* left;
    Node* right;
   
    Node(const int _key, int _priority, Node* l, Node* r): key(_key), priority(_priority),  left(l), right(r) {}

    Node(Node* u) : key(u->key), priority(u->priority), left(u->left), right(u->right) {}

};

class Treap {

  public:
    
    Treap() { std::srand(5); root = nullptr; }

    Treap(Node* u) { root = u; }

    // Insere um nó com chave x na treap.
    Treap Insert(const int x) { return Treap(_Insert(root, x)); } 
    
    // Remove nó com chave x da treap 
    Treap Delete(const int x) { return Treap(_Delete(root, x)); }

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
      if(r == nullptr) return new Node(x, _rand(), nullptr, nullptr); // Árvore vazia
      
      Node* new_r = new Node(r);
      if(x > r->key) 
      {
        new_r->right = _Insert(r->right, x);

        if(new_r->right->priority > new_r->priority)
        {
          Node* child = new_r->right;
          RotateLeft(new_r, child);
          new_r = child;
        }
      }
     
      else if(x < r->key)
      {
        new_r->left = _Insert(r->left, x);

        if(new_r->left->priority > new_r->priority)
        {
          Node* child = new_r->left;
          RotateRight(new_r, child);
          new_r = child;
        }
    }

    return new_r;
    }
    
    // Deleta nó de chave x da treap enraizada em u. Devolve a treap resultante. 
    Node* _Delete(Node* r, int const x)
    {
      if(r == nullptr) return nullptr; // Chave não encontrada
      
      Node* new_r = new Node(r);

      if(x > r->key)
      {
        new_r->right = _Delete(r->right, x);
        return new_r;
      }

      else if(x < r->key)
      {
        new_r->left =_Delete(r->left, x); 
        return new_r; 
      }

      else // x == r->key
      { 
        // Só faço o shortcut. Não é necessário cópia
        if(r->left == nullptr) return r->right; 
        if(r->right == nullptr) return r->left;

        else // Tem os dois filhos. Desço rotacionando para manter a propriedade de heap
        { 
          Node* child;

          // Determina qual filho toma o lugar de u
          if(r->left->priority > r->right->priority){
            child = new Node(r->left);
            RotateRight(new_r, child);
            child->right = _Delete(child->right, x);
          }
          else{
            child = new Node(r->right);
            RotateLeft(new_r, child);
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

int main()
{
  std::vector<Treap> v = std::vector<Treap>();

  v.push_back(Treap());

  for(std::string line; std::getline(std::cin, line) ; ) {
    int opt, t,  x;
    std::istringstream iss = std::istringstream(line);
    iss >> opt >> t >> x;

    switch (opt) {
      case 1:
        v.push_back(v[t].Insert(x));
        break;
      case 2:
        v.push_back(v[t].Delete(x));
        break;
      case 3:
        std::cout << (v[t].Search(x) ? "1" : "0") << "\n";
        break;
      case 4:
        std::cout << v[t].Min() << "\n";
        break;
      case 5:
        v[t].Print();
        break;
    }
  }
}

