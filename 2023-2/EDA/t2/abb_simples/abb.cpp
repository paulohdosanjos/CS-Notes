// Implementação de uma ABB simples para eu começar a ter um feeling da implementação da treap. 

#include <iostream>
#include <string>
#include <cassert>
#include <cmath>

#define INFTY 1000000
#define EMPTY_CHAR -12345

// Não será usado apontador para o pai. Mantenho a quantidade de nós da sub-árvore esquerda para implementar uma função de Print() bonitinha da árvore.
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
        Node* u = new Node(x, 0, nullptr, nullptr);
        root = u;
        return;
      }

      if(SearchKey(x)) {
        return;
      }

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

   // Retorna o número de digitos de n. Converter para uma macro
    int number_of_digits(int n)
    {
      return (int) (std::log10(n) + 1);
    }

    
    void Print_matrix(int num_rows, int num_cols, int matrix[]){
      // Original arrow mapping
      // arrow[0] = '/'
      // arrow[1] = '-'
      // arrow[2] = '|'
      // arrow[3] = '\\'
      // arrow[4] = ' '
      char arrows[] = {'/', '-', '*', '\\', ' '};

      int n1 = number_of_digits(Max());
      int n2;
      n2 = (Min() < 0 ? 1 + number_of_digits(std::abs(Min())) : 0);
      int cell_width = std::max(n1,n2);
      std::string empty_cell (cell_width,' '); 

      for(int i = 0; i < num_rows; i++){
        for(int j = 0; j < num_cols; j++){
          int index = i * num_cols + j;
          if(i % 3 == 1 || i % 3 == 2){ // Linha de setas
            if(matrix[index] == EMPTY_CHAR){
              std::cout << empty_cell;// << "|";
              continue;
            }
            int num_digits = 1; 
            int delta = (cell_width - num_digits) / 2;
            std::cout << std::string(delta, ' ');
            std::cout << arrows[matrix[index]];
            std::cout << std::string(cell_width - num_digits - delta, ' ');// << "|";
          }
          else { // Linha de chaves
            if(matrix[index] == EMPTY_CHAR){
              std::cout << empty_cell; // << "|";
              continue;
            }
            int num_digits = (matrix[index] < 0 ? 1 + number_of_digits(std::abs(matrix[index])) : number_of_digits(matrix[index]));
            int delta = (cell_width - num_digits) / 2;
            std::cout << std::string(delta, ' ');
            std::cout << matrix[index];
            std::cout << std::string(cell_width - num_digits - delta, ' ');// << "|";
          }
        }
        std::cout << std::endl;
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
      int num_rows = Height() * 3 - 2; 
      int num_cols = Size();
      int output [num_rows*num_cols];
      for(int i = 0; i < num_rows*num_cols; i++) output[i] = EMPTY_CHAR;
      int i_root = 0, j_root = root->num_left;
      int index = i_root * num_cols + j_root;

      output[index] = root->key;

      _Print(num_rows, num_cols, output, root->left, true, i_root, j_root, root->num_left);
      _Print(num_rows, num_cols, output, root->right, false, i_root, j_root, root->num_left);

      Print_matrix(num_rows, num_cols, output);
    }

    // Retorna a maior chave da árvore
    int Max() 
    {
      return _Max(root);
    }

    int _Max(Node* u){
      if(u == nullptr) return -INFTY;
      if(u->right != nullptr) return _Max(u->right);
      else return u->key;
    }
    
    // Retorna a menor chave da árvore
    int Min() 
    {
      return _Min(root);
    }

    int _Min(Node* u){
      if(u == nullptr) return -INFTY; // Entra somente quando a árvore está vazia
      if(u->left != nullptr) return _Min(u->left);
      else return u->key;
    }
    void _Print(int num_rows, int num_cols, int output[], Node* u, bool is_left_child, int i_parent, int j_parent, int num_left_parent)
    {
      if(u == nullptr) return;

      int i = i_parent + 3, j;
      if(is_left_child) {
        j = j_parent - num_left_parent + u->num_left; 
      }
      else {
        j = j_parent + u->num_left + 1;
      }

      // Imprimindo setas
      // '/' = 0 , '-' = 1 , '|' = 3 , '\' = 4

      int i_line = i_parent + 1;
      int index_vertical_bar_below_parent = i_line * num_cols + j_parent;
      if(j < j_parent - 1 || j > j_parent + 1){
        output[index_vertical_bar_below_parent] = 2; // '|'
      }

      if(is_left_child){ // Filho esquerdo
        int index;
        for(int x = j_parent - 1; x >= j + 1; x--){
          index = i_line * num_cols + x;
          output[index] = 1; // '-'
        }
        i_line++;
        index = i_line * num_cols + j;
        output[index] = 0; // '/'
      }

      else{ // Filho direito
        int index;
        for(int x = j_parent + 1; x <= j - 1; x++){
          index = i_line * num_cols + x;
          output[index] = 1; // '-'
        }
        i_line++;
        index = i_line * num_cols + j;
        output[index] = 3; // '\'
      }

      // Imprimindo actual key
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
      
      if(x > r->key) {
        //std::cout << "Caso 1" << std::endl;
        if(r->right == nullptr){
          Node* u = new Node(x, 0, nullptr, nullptr);
          r->right = u;
          return u;
        }
        else {
          return _Insert(r->right, x);
        }
      }
      else {
        r->num_left++;
        if(r->left == nullptr){
          Node* u = new Node(x, 0, nullptr, nullptr);
          r->left = u;
          return u;
        }
        else{
          return _Insert(r->left, x);
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
  //int list[] = {10, 3, 23, 5, 11, 2, 14, 4, 6, 20};
  int list[] = {-10111, 30, 2, 33, 7, 10, 11, 103, 102, 1109};
  int n = sizeof(list) / sizeof(list[0]);
  Abb a = Abb();
  for(int i = 0; i < n ; i++) a.Insert(list[i]);
  //std::cout << a.Min() << std::endl;
  a.Print();
}


int main() {
  Teste1();
}
