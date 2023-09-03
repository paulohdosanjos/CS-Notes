// Implementação de uma ABB simples para eu começar a ter um feeling da implementação da treap. 

#include <iostream>
#include <string>
#include <cassert>

// Não será usado apontador para o pai. Mantenho a quantidade de nós da sub-árvore esquerda para implementar uma função de Print() da árvore.
class Node {
  public:
    const int key;
    int num_left; // Número de nós na sub-árvore esquerda
    Node* left;
    Node* right;
   
    Node(const int _key, int _num_left, Node* l, Node* r): key(_key), num_left(_num_left), left(l), right(r) {}
};

class Abb {
  public:
    Abb() {root = nullptr;}

    // Insere um nó com chave x na árvore. Caso já existe um nó com essa chave, nada acontece.
    void Insert(const int x) 
    {
      Node* u = new Node(x, 0, nullptr, nullptr);

      std::cout << "novo nó alocado com valor " << u->key << std::endl;

      if(root == nullptr) { // Árvore vazia
        std::cout << "árvore vazia, só atribui raiz para o novo nó criado" << std::endl;
        root = u;
        return;
      }

      std::cout << "procurando pai do novo nó" << std::endl;

      Node* parent = SearchParent(root, nullptr, x);
      std::cout << "pai encontrado" << std::endl;
      if(parent == nullptr) { // A raiz tem chave x
        std::cout << "chave já existe, não faço nada" << std::endl;
        return;
      }

      else {
        std::cout << "pai do novo nó : " << parent->key << std::endl;
      }

      if(x == parent->key) {}
      if(x > parent->key) parent->right = u;
      if(x < parent->key) parent->left = u;
      std::cout << "atribuido nó filho para o pai" << std::endl;
    }

    // Devolve string com representação visual da árvore.
    std::string Print()
    {
      std::string output = std::string();

    }

  private:
    Node* root;

    // Retorna o que seria o pai de um nó com chave x a ser inserido na árvore enraizada em r. O nó do pai de r é incluido na chamada para resolver o caso em que já existe um nó de chave x, nesse caso, retorna o pai desse nó.
    // Observe que se a árvore só tem um nó e a função é chamada com a x = chave da raiz, será retornado NULL. 
    // Não deve ser chamada para uma árvore vazia. Quem a chama é quem deve tratar esse caso
    Node* SearchParent(Node* const r, Node* const r_parent, const int x)
    {
      assert(r != nullptr);
      
      std::cout << "SearchParent(" << r->key << ", " << (r_parent == nullptr ? 0 : r_parent->key) << ", " << x << ")" << std::endl;

      if(x == r->key) { //Nó já existe em r
        std::cout << "Nó já existente, retornando pai" << std::endl;
        return r_parent; 
      }

      else if(x > r->key) {
        std::cout << "Caso 1" << std::endl;
        if(r->right != nullptr){
          std::cout << "chave x maior, indo pra direita" << std::endl;
          return SearchParent(r->right, r, x);
        }
        else {
          std::cout << "achei caso 1" << std::endl;
          return r;
        }
      }
      else {
        std::cout << "Caso 2" << std::endl;
        if(r->left != nullptr){
          std::cout << "chave x menor, indo pra esquerda" << std::endl;
          return SearchParent(r->left, r, x);
        }
        else{
          std::cout << "achei caso 2" << std::endl;
          return r;
        }
      }
    }

    // Verifica se chave x está presente na árvore
    bool SearchKey(const int x)
    {

    }

    // Verifica se a chave x está presente na árvore enraizada em r.
    bool _SearchKey(Node* r, const int x)
    {
      
    }

    std::string _Print(Node* u, int i) 
    {
       
    }
};


void Teste1() 
{
  Abb a = Abb();
  a.Insert(1);
  a.Insert(2);
  a.Insert(3);
  a.Insert(12);
  a.Insert(-2);
}


int main() {
  Teste1();
}
