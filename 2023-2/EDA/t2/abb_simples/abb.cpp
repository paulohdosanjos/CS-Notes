// Implementação de uma ABB simples para eu começar a ter um feeling da implementação da treap. 

#include <iostream>
#include <string>
#include <cassert>

#define INFTY 1000000

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

      if(root == nullptr) { // Árvore vazia
        //std::cout << "árvore vazia, só atribui raiz para o novo nó criado" << std::endl;
        Node* u = new Node(x, 0, nullptr, nullptr);
        //std::cout << "novo nó alocado com valor " << u->key << std::endl;
        root = u;
        return;
      }

      //std::cout << "verificando se já existe chave " << x << " na árvore" << std::endl;

      if(SearchKey(x)) {
        //std::cout << "chave já existe, não faço nada" << std::endl;
        return;
      }

      //std::cout << "chave não existe, inserindo novo nó" << std::endl;

      _Insert(root, x);

    }

    // Imprime a árvore em pós-ordem. Para depuração
    void PrintPre()
    {
      std::cout << "Size : " << Size() << std::endl;
      _PrintPre(root);
      std::cout << "\n";
    }

    void _PrintPre(const Node* const u)
    {
      if(u == nullptr) return;
      std::cout << u->key << "(" << u->num_left << ") ";
      _PrintPre(u->left);
      _PrintPre(u->right);

    }

    // Devolve string com representação visual da árvore.
    //std::string Print()
    //{
    //  std::string output = std::string();

    //}
    
    void Print_matrix(int num_rows, int num_cols, int matrix[]){
      for(int i = 0; i < num_rows; i++){
        for(int j = 0; j < num_cols; j++){
          int index = i * num_cols + j;
          std::cout << (matrix[index] == 0 ? " " : std::to_string(matrix[index])) << "   ";
        }
        std::cout << std::endl << std::endl;
      } 
    }

    // Retorna altura da árvore
    int Height()
    {
      return _Height(root, 0);
    }

    // Retorna a altura da árvore enraizada em u, onde a profundidade do pai de u é parent_depth
    int _Height(Node* u, int parent_depth)
    {
      if(u == nullptr) return 0;

      int depth = parent_depth + 1;
      int height_left_tree = _Height(u->left, depth);
      int height_right_tree = _Height(u->right, depth);
      return 1 + std::max(height_left_tree, height_right_tree);
    }

    // Imprime uma representação gráfica da árvore. Consome espaço O(n²) e tempo O(n)
    void Print()
    {
      int num_rows = Height(); 
      int num_cols = Size();
      int output [num_rows*num_cols];
      for(int i = 0; i < num_rows*num_cols; i++) output[i] = 0;
      int i_root = 0, j_root = root->num_left;
      int index = i_root * num_cols + j_root;

      //std::cout << "i_root = " << i_root << " j_root = " << j_root << std::endl;
      output[index] = root->key;
      //Print_matrix(num_rows, num_cols, output);

      _Print(num_rows, num_cols, output, root->left, true, i_root, j_root, root->num_left);
      _Print(num_rows, num_cols, output, root->right, false, i_root, j_root, root->num_left);

      Print_matrix(num_rows, num_cols, output);
     
    }

    void _Print(int num_rows, int num_cols, int output[], Node* u, bool is_left_child, int i_parent, int j_parent, int num_left_parent)
    {
      if(u == nullptr) return;

      int i = i_parent + 1, j;
      if(is_left_child) {
        j = j_parent - num_left_parent + u->num_left; 
      }
      else {
        j = j_parent + u->num_left + 1;
      }

      int index = i * num_cols + j;
      //std::cout << "i = " << i << " j = " << j << std::endl;
      output[index] = u->key;
      //Print_matrix(num_rows, num_cols, output);

      _Print(num_rows, num_cols, output, u->left, true, i, j, u->num_left);
      _Print(num_rows, num_cols, output, u->right, false, i, j, u->num_left);
    }
    

  private:
    Node* root;

    // Retorna o pai de um nó com chave x a ser inserido na árvore enraizada em r e edita o campo num_left do nós no caminho da raiz até o novo nó. Pressupõe que a chave não existe na árvore 
    // Não deve ser chamada para uma árvore vazia. Quem a chama é quem deve tratar esse caso
    Node* _Insert(Node* const r, const int x)
    {
      assert(r != nullptr);
      assert(x != r->key);
      
      //std::cout << "SearchParent(" << r->key << ", " << (r_parent == nullptr ? 0 : r_parent->key) << ", " << x << ")" << std::endl;

      if(x > r->key) {
        //std::cout << "Caso 1" << std::endl;
        if(r->right == nullptr){
          Node* u = new Node(x, 0, nullptr, nullptr);
          r->right = u;
          return u;
        }
        else {
          return _Insert(r->right, x);
          //std::cout << "achei caso 1" << std::endl;
        }
      }
      else {
        r->num_left++;
        //std::cout << "Caso 2" << std::endl;
        if(r->left == nullptr){
          //std::cout << "chave x menor, indo pra esquerda" << std::endl;
          Node* u = new Node(x, 0, nullptr, nullptr);
          r->left = u;
          return u;
        }
        else{
          return _Insert(r->left, x);
          //std::cout << "achei caso 2" << std::endl;
        }
      }
    }

    // Verifica se chave x está presente na árvore
    bool SearchKey(const int x) const
    {
      return _SearchKey(root,x);
    }

    // Verifica se a chave x está presente na árvore enraizada em r.
    bool _SearchKey(const Node* const r, const int x) const
    {
      if(r == nullptr) return false;

      if(x == r->key) return true;
      else if(x < r->key) return _SearchKey(r->left, x);
      else return _SearchKey(r->right, x);
    }

    int Size()
    {
      return _Size(root);
    }

    int _Size(Node* u)
    {
      if(u == nullptr) return 0;
      return 1 + _Size(u->left) + _Size(u->right);
    }
};


// Teste inicial para insert. Parece OK.
void Teste1() 
{
  Abb a = Abb();
  a.Insert(10);
  a.Insert(3);
  a.Insert(23);
  a.Insert(5);
  a.Insert(11);
  a.Insert(2);
  a.Insert(14);
  a.Insert(4);
  a.Insert(6);
  a.Insert(21);
  a.Insert(12);
  a.Insert(15);
  a.Insert(1);
  std::cout << "Height = " << a.Height() << std::endl;
  a.PrintPre();
  a.Print();
}


int main() {
  Teste1();
}
