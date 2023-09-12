// Implementação de uma treap com uma função Print() 
#include <iostream>
#include <string>
#include <cassert>
#include <cmath>
#include <cstdlib>


#define INFTY 1000000
#define EMPTY_CHAR -12345

#define _rand() (std::rand() % 1001)

// Nó não terá campo apontador para o pai. Mantenho a quantidade de nós da sub-árvore esquerda para implementar uma função de Print() bonitinha da árvore.
class Node {
  public:
    int priority;
    int key;
    int num_left; // Número de nós na sub-árvore esquerda
    Node* left;
    Node* right;
   
    Node(const int _key, int _priority, int _num_left, Node* l, Node* r): key(_key), priority(_priority), num_left(_num_left), left(l), right(r) {}
};

class Treap {
  public:
    Treap(int seed) {std::srand(seed); root = nullptr;}

    // Insere um nó com chave x na treap. Caso já exista um nó com essa chave, nada acontece.
    void Insert(const int x) 
    {
      if(root == nullptr) { // Árvore vazia
        Node* u = new Node(x, _rand(), 0, nullptr, nullptr);
        root = u;
        return;
      }
      if(SearchKey(x)) return;
      _Insert(root, x);
      root = Swin(root, x);
    }
    
    // Remove nó com chave x da treap 
    void Delete(const int x)
    {
      root = _Delete(root, x);
      assert(verify_heap()); // Verifica propriedade do heap
    }

    // Imprime a árvore em pré-ordem. Para depuração
    void PrintPre()
    {
      std::cout << "Size : " << Size() << std::endl;
      _PrintPre(root);
      std::cout << "\n";
    }

    // Para testar altura da árvore
    void PrintInfo()
    {
      int size = Size();
      std::cout << "Size = " << size << ", Height = " << Height() << ", Log(Size) = " << std::log2(size) << "\n";
    }

    // Imprime uma representação gráfica da árvore. Consome espaço O(nh) e tempo O(n)?
    void Print()
    {
      if(Size() == 0)
      {
        std::cout << "\n";
        return;
      }
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
    
    // Retorna a menor chave da árvore
    int Min() 
    {
      return _Min(root)->key;
    }

    // Verifica se chave x está na árvore
    bool Search(const int x)
    {
      return SearchKey(x);
    }
    
    int Size()
    {
      return _Size(root);
    }

  private:
    Node* root;
 
    // Sobe um nó adicionado numa folha até sua altura correta
    Node* Swin(Node* u, int x)
    {
      if(u == nullptr) return nullptr;

      if(x > u->key)
      {
        u->right = Swin(u->right, x);
        if(u->right != nullptr && u->right->priority > u->priority) {
          Node* child = u->right;
          RotateLeft(u, u->right); 
          return child;
        }
      }

      if(x < u->key)
      {
        u->left = Swin(u->left, x);
        if(u->left != nullptr && u->left->priority > u->priority) {
          Node* child = u->left;
          RotateRight(u, child);
          return child;
        }
      }
        return u; 
    }

    void RotateLeft(Node* u, Node* child)
    {
      assert(child == u->right);

      u->right = child->left;
      child->left = u;
      int beta = child->num_left;
      int alpha = u->num_left;
      child->num_left = alpha + beta + 1;
    }

    void RotateRight(Node* u, Node* child)
    {
      assert(child == u->left);

      u->left = child->right;
      child->right = u;
      int beta = child->num_left;
      int alpha = u->num_left -1 - beta;
      u->num_left = alpha;
 
    }

    // Verifica se a propriedade do heap é válida na treap
    bool verify_heap()
    {
      return _verify_heap(root);
    }

    // Verifica se a propriedade do heap é válida na treap enraizada em u
    bool _verify_heap(Node* u)
    {
      if(u == nullptr) return true;
      int left_priority = (u->left == nullptr ? -INFTY : u->left->priority);
      int right_priority = (u->right == nullptr ? -INFTY : u->right->priority);
      if(u->priority < left_priority || u->priority < right_priority) return false;
      return _verify_heap(u->left) && _verify_heap(u->right);
    }

    void _PrintPre(const Node* const u)
    {
      if(u == nullptr) return;
      std::cout << u->key << "(" << u->num_left << ", p = "<< u->priority << ") ";
      _PrintPre(u->left);
      _PrintPre(u->right);

    }

   // Retorna o número de digitos de n. Usada na função Print()
    int number_of_digits(int n)
    {
      return (int) (std::log10(n) + 1);
    }

    // Função auxiliar para a função Print()
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

    // Retorna a maior chave da árvore. Usada em Print()
    int Max() 
    {
      return _Max(root);
    }

    // Retonar a maior chave da árvore enraizada em u
    int _Max(Node* u){
      if(u == nullptr) return -INFTY;
      if(u->right != nullptr) return _Max(u->right);
      else return u->key;
    }

    // Retonar a menor chave da árvore enraizada em u
    Node* _Min(Node* u){
      if(u == nullptr) return nullptr; // Entra somente quando a árvore está vazia
      if(u->left != nullptr) return _Min(u->left);
      else return u;
    }

    // Função auxiliar usada em Print()
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
      output[index] = u->key;

      _Print(num_rows, num_cols, output, u->left, true, i, j, u->num_left);
      _Print(num_rows, num_cols, output, u->right, false, i, j, u->num_left);
    }
    
    // Retorna altura da árvore. Usada em Print()
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

    // Insere um novo nó como folha com chave r na árvore enraizada em r e retorna a árvore resultante.
    // Não deve ser chamada para uma árvore vazia. Quem a chama é quem deve tratar esse caso.
    // Não trata a propriedade de heap, isso é feito pela função Insert()
    Node* _Insert(Node* const r, const int x)
    {
      assert(r != nullptr);
      assert(x != r->key);
      
      if(x > r->key) {
        if(r->right == nullptr){
          Node* u = new Node(x, _rand(), 0, nullptr, nullptr);
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
          int random_priority;
          Node* u = new Node(x, _rand(), 0, nullptr, nullptr);
          r->left = u;
          return u;
        }
        else{
          return _Insert(r->left, x);
        }
      }
    }
    
    // Deleta nó de chave x da treap enraizada em u. Devolve a treap resultante. Se a chave não existir na treap, nada acontece 
    Node* _Delete(Node* u, int const x)
    {
      if(u == nullptr) return nullptr; // Chave não encontrada

      if(x > u->key){
        u->right = _Delete(u->right, x);
        return u;
      }
      else if(x < u->key){
        u->left =_Delete(u->left, x); 
        u->num_left--;
        return u; 
      }
      else
      { // x == u->key
        if(u->left == nullptr && u->right == nullptr){ // É folha 
          return nullptr; 
        }
        else // Não é folha. Rotaciona para baixo para manter a proprieda de heap
        { 
          int left_priority = (u->left == nullptr ? -INFTY : u->left->priority);
          int right_priority = (u->right == nullptr ? -INFTY : u->right->priority);

          // Determina qual filho toma o lugar de u
          if(left_priority > right_priority){
            Node* child = u->left;
            RotateRight(u, child);
            child->right = _Delete(child->right, x);
            return child;
          }
          else{
            Node* child = u->right;
            RotateLeft(u, child);
            child->left = _Delete(child->left, x);
            child->num_left--;
            return child;
          }
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

    Node* SearchNode(const int x) const
    {
      return _SearchNode(root, x);
    }

    Node* _SearchNode(Node* const n, const int x) const
    {
      if(n == nullptr) return nullptr;
      if(x == n->key) return n;
      else if(x < n->key) return _SearchNode(n->left, x);
      else return _SearchNode(n->right, x);
    }

    int _Size(Node* u)
    {
      if(u == nullptr) return 0;
      return 1 + _Size(u->left) + _Size(u->right);
    }
};

// Testes básicos para Insert. Parece OK
void Teste1(int seed)
{
  Treap t = Treap(seed);
  t.Insert(30);
  t.Insert(24);
  t.Insert(23);
  t.Insert(12);
  t.Insert(32);
  t.Insert(20);
  t.Insert(4);
  t.Insert(45);
  t.Insert(40);
  t.Insert(7);
  t.Insert(33);
  t.Insert(18);
  t.Insert(19);
  t.Insert(9);
  std::cout << t.Size() << "\n";
  t.PrintPre();
  t.Print();
}

// Testes básicos para a altura da árvore gerada. Parece OK
void Teste2(int seed)
{
  Treap t = Treap(seed);
  for(int i = 1; i <= 2000000; i++){
    t.Insert(i);
  }
  //t.PrintPre();
  t.PrintInfo();
  //t.Print();
}

// Teste inicial para delete
void Teste4(int seed)
{
  Treap t = Treap(seed);
  t.Insert(30);
  t.Insert(12);
  t.Insert(33);
  t.Insert(40);
  t.Insert(22);
  std::cout << "Antes das deleções:\n";
  t.Print();

  t.Delete(33);
  std::cout << "Deleta " << 33 << "\n";
  t.Print();
  //t.PrintPre();
  t.Delete(40);
  std::cout << "Deleta " << 40 << "\n";
  t.Print();
  t.Delete(22);
  std::cout << "Deleta " << 22 << "\n";
  t.Print();
  t.Delete(30);
  std::cout << "Deleta " << 30 << "\n";
  t.Print();
  t.Delete(12);
  std::cout << "Deleta " << 12 << "\n";
  t.Print();
}

// Teste final para delete. Parece OK
void Teste3(int seed)
{
  //Inserindo elementos

  int insert_list[] = {10, 5, 15, 3, 8, 12, 18, 2, 4, 7, 9, 11, 14, 16, 20};
  int n = sizeof(insert_list) / sizeof(insert_list[0]);
  Treap t = Treap(seed);
  for(int i = 0; i < n ; i++) t.Insert(insert_list[i]);
  std::cout << "Antes de deletar" << std::endl;
  std::cout << std::endl;
  t.Print();
  std::cout << std::endl;

  // Deletando elementos

  int delete_list[] = {2, 4, 7, 9, 11, 14, 16,20,18, 12, 15, 10, 5, 3, 8};
  n = sizeof(delete_list) / sizeof(delete_list[0]);
  for(int i = 0; i < n; i++){
    std::cout << "Deleta " << delete_list[i] << std::endl;
    t.Delete(delete_list[i]);
    t.PrintPre();
    t.Print();
  }
}

int main (int argc, char *argv[]) {

  int seed = std::atoi(argv[1]);
  //Teste1(seed);
  //Teste2(seed);
  Teste3(seed);
  //Teste4(seed);
  
  return 0;
}
