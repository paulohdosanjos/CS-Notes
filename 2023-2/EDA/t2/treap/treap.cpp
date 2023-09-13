// Implementação de uma treap com uma função Print() mais bonitinha.
// Não imprimo a prioridade, mas após cada delete e insert tem um assert que verifica se a propriedade de heap é válida na treap.
#include <iostream>
#include <string>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <sstream>


#define INFTY 1000000
#define EMPTY_CHAR -12345
#define MAX_PRIORITY 1001

#define _rand() (std::rand() % MAX_PRIORITY)

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
    Treap() {std::srand(121); root = nullptr;}

    // Insere um nó com chave x na treap. Caso já exista um nó com essa chave, nada acontece.
    void Insert(const int x) 
    {
      if(SearchKey(x)) return; // Essa primeira passada evita que o campo num_left seja alterado nos nós do caminho se a chave já estiver presente na treap
 
      root = _Insert(root, x);
      assert(verify_heap()); // Verifica propriedade do heap
    }
    
    // Remove nó com chave x da treap 
    void Delete(const int x)
    {
      root = _Delete(root, x);
      assert(verify_heap()); // Verifica propriedade do heap
    }

    // Verifica se chave x está na árvore
    bool Search(int x) const { return SearchKey(x); }
    
    // Retorna a menor chave da árvore
    int Min() const { return _Min(root)->key; }

    // Retorna altura da árvore. Usada em Print()
    int Height() const
    {
      return _Height(root, 0);
    } 

    // Imprime uma representação gráfica da árvore. Consome espaço O(nh) e tempo O(n)?. Tem bug para árvores com chave nula
    void Print() const
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
    
    

    int Size() const
    {
      return _Size(root);
    }

  private:
    Node* root;
 
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
    int number_of_digits(int n) const
    {
      if(n == 0) return 1;
      return (int) (std::log10(n) + 1);
    }

    // Função auxiliar para a função Print()
    void Print_matrix(int num_rows, int num_cols, int matrix[]) const{
      // Original arrow mapping
      // arrow[0] = '/'
      // arrow[1] = '-'
      // arrow[2] = '|'
      // arrow[3] = '\\'
      // arrow[4] = ' '
      char arrows[] = {'/', '-', '*', '\\', ' '};

      int min = Min();
      int n1 = number_of_digits(Max());
      int n2 = (min < 0 ? 1 + number_of_digits(std::abs(min)) : 0);
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
    int Max() const
    {
      return _Max(root);
    }

    // Retonar a maior chave da árvore enraizada em u
    int _Max(Node* u) const 
    {
      if(u == nullptr) return -INFTY;
      if(u->right != nullptr) return _Max(u->right);
      else return u->key;
    }

    // Retonar a menor chave da árvore enraizada em u
    Node* _Min(Node* u) const
    {
      if(u == nullptr) return nullptr; // Entra somente quando a árvore está vazia
      if(u->left != nullptr) return _Min(u->left);
      else return u;
    }

    // Função auxiliar usada em Print()
    void _Print(int num_rows, int num_cols, int output[], Node* u, bool is_left_child, int i_parent, int j_parent, int num_left_parent) const
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
    
   

    // Retorna a altura da árvore enraizada em u, onde a profundidade do pai de u é parent_depth
    int _Height(Node* u, int parent_depth) const
    {
      if(u == nullptr) return 0;

      int depth = parent_depth + 1;
      int height_left_tree = _Height(u->left, depth);
      int height_right_tree = _Height(u->right, depth);
      return 1 + std::max(height_left_tree, height_right_tree);
    }

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
        r->num_left++;
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
        u->num_left--;
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
            child->num_left--;
          }
          return child;
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

    int _Size(Node* u) const
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
  //t.PrintPre();
  t.Print();
}

// Testes básicos para a altura da árvore gerada. Parece OK
void Teste2(int seed)
{
  Treap t = Treap(seed);
  for(int i = 1; i <= 20000; i++){
    t.Insert(i);
  }
  std::cout << t.Height() << "\n";
  //t.PrintPre();
  //t.PrintInfo();
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
    //t.PrintPre();
    t.Print();
  }
}

// Cliente teste da tarefa
void Teste()
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

void Teste5()
{
  Treap t = Treap();
  t.Insert(1);
  t.Print();
}

int main (int argc, char *argv[]) {

  //int seed = std::atoi(argv[1]);
  //Teste1(seed);
  //Teste2(seed);
  //Teste3(seed);
  //Teste4(seed);
  //Teste5();
  Teste();
  
  return 0;
}
