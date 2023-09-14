// Tarefa t2
// Paulo Henrique Albuquerque
// 12542251

#include <iostream>
#include <string>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <vector>


#define INFTY 1000000
#define EMPTY_CHAR -12345
#define MAX_PRIORITY 1001

#define _rand() (std::rand() % MAX_PRIORITY)

class Node {
  public:
    int priority;
    int key;
    int num_left; // Número de nós na sub-árvore esquerda
    Node* left;
    Node* right;
   
    Node(const int _key, int _priority, int _num_left, Node* l, Node* r): key(_key), priority(_priority), num_left(_num_left), left(l), right(r) {}

    Node(Node* u) : key(u->key), priority(u->priority), num_left(u->num_left), left(u->left), right(u->right) {}

};

class Treap {

  public:
    
    Treap() { std::srand(5); root = nullptr; }

    Treap(Node* u) { root = u; }

    // Insere um nó com chave x na treap. Caso já exista um nó com essa chave, nada acontece.
    Treap Insert(const int x) 
    {
      if(SearchKey(x)) return nullptr; // Essa primeira passada evita que o campo num_left seja alterado nos nós do caminho se a chave já estiver presente na treap
 

      Treap t = Treap(_Insert(root, x));
      assert(t.verify_heap()); // Verifica propriedade do heap
      return t; 
    }
    
    // Remove nó com chave x da treap 
    Treap Delete(const int x)
    {
      Treap t = Treap(_Delete(root, x));
      assert(t.verify_heap()); // Verifica propriedade do heap
      return t;
      //assert(verify_heap()); // Verifica propriedade do heap
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

    void PrintPre()
    {
      _PrintPre(root);
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
        new_r->num_left++;
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
    
    // Deleta nó de chave x da treap enraizada em u. Devolve a treap resultante. Se a chave não existir na treap, nada acontece 
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
        new_r->num_left--;
        return new_r; 
      }

      else // x == r->key
      { 
        // Só faço o shortcut
        if(r->left == nullptr) return r->right; 
        if(r->right == nullptr) return r->left;

        else //Tem os dois filhos. Desço rotacionando para manter a propriedade de heap
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

// Testes básicos para Insert persistente. Parece OK

void Teste1()
{
  Treap t1 = Treap();
  Treap t2 = t1.Insert(10);
  t2.Print();
  Treap t3 = t2.Insert(7);
  t3.Print();
  Treap t4 = t3.Insert(12);
  t4.Print();
  t3.Print();
}

// Testes básicos para Delete persistente. Parece OK
void Teste2()
{
  Treap t1 = Treap();
  t1 = t1.Insert(10);
  t1 = t1.Insert(7);
  t1 = t1.Insert(12);
  t1 = t1.Insert(15);
  t1 = t1.Insert(20);
  t1 = t1.Insert(5);
  t1.Print();
  std::cout << "***************\n";
  Treap t2 = t1.Delete(5);
  t2.Print();
  std::cout << "***************\n";
  Treap t3 = t2.Delete(10);
  t3.Print();
  std::cout << "***************\n";
  Treap t4 = t3.Delete(15);
  t4.Print();
  std::cout << "***************\n";
  Treap t5 = t4.Delete(12);
  t5.Print();
  std::cout << "***************\n";
  Treap t6 = t5.Delete(7);
  t6.Print();
  std::cout << "***************\n";
  Treap t7 = t6.Delete(20);
  t7.Print();
  std::cout << "***************\n";
}

// Testes básicos para a altura da árvore gerada. BUG: Gerando árvore linear. BUG corrigido
void Teste3()
{
  Treap t = Treap();
  for(int i = 1; i <= 200; i++){
    t = t.Insert(i);
  }
  std::cout << t.Height() << "\n";
  //t.Print();
  //t.PrintPre();
  //t.PrintInfo();
  //t.Print();
}

// Depuração da altura linear. Parece OK
void Teste4()
{
  Treap t = Treap();
  t = t.Insert(10);
  t.Print();
  std::cout << "*****************\n";
  t = t.Insert(7);
  t.Print();
  std::cout << "*****************\n";
  t = t.Insert(12);
  t.Print();
  std::cout << "*****************\n";
  t = t.Insert(15);
  t.Print();
  std::cout << "*****************\n";
  t = t.Insert(20);
  t.Print();
  std::cout << "*****************\n";
  t = t.Insert(5);
  t.Print();
  t.PrintPre();
  
}

// Teste da tarefa
void Teste()
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

int main (int argc, char *argv[]) {
  //Teste1();
  //Teste2();
  //Teste3();
  //Teste4();
  Teste();
  
  return 0;
}
