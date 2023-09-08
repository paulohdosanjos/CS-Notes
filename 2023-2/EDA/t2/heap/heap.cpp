// Implementação simples de um heap para preparar para a implementação da treap.
// Usamos uma árvore quase completa na implementação 


#include <iostream>
#include <string>
#include <cmath>

#define EMPTY_CHAR -12345
#define INFITY 1000000

class Node {
  public:
    int key;
    int size;
    Node* left;
    Node* right;

    Node(int c, int s): key(c), size(s), left(nullptr), right(nullptr) {}

};

class Heap{
  public:

    // Inicializa o heap
    Heap(){ root = nullptr; }

    void Insert(int x)
    {
      if(root == nullptr){ // Heap vazio
        root = new Node(x, 0);
        return;
      }
      _Insert(x, root);
    }

    // Imprime a árvore em pré-ordem. Para depuração
    void PrintPre()
    {
      std::cout << "Size : " << root->size + 1 << std::endl;
      _PrintPre(root);
      std::cout << "\n";
    }

    // Remove o nó de menor chave do heap
    void DeleteMin()
    {
      root = Sink(root);
      //std::cout << "Delete completado\n";
    }

     // Imprime uma representação gráfica da árvore. Consome espaço O(n²) e tempo O(n)
    void Print()
    {
      int size = root->size + 1;
      if(size == 0)
      {
        std::cout << "\n";
        return;
      }
      int num_rows = Height() * 3 - 2; 
      int num_cols = size;
      int output [num_rows*num_cols];
      for(int i = 0; i < num_rows*num_cols; i++) output[i] = EMPTY_CHAR;
      int num_left = (root->left == nullptr ? 0 : root->left->size + 1);
      int i_root = 0, j_root = num_left;
      int index = i_root * num_cols + j_root;

      output[index] = root->key;

      _Print(num_rows, num_cols, output, root->left, true, i_root, j_root, num_left);
      _Print(num_rows, num_cols, output, root->right, false, i_root, j_root, num_left);

      Print_matrix(num_rows, num_cols, output);
    }
 
  private:
    Node* root;

    Node* _Insert(int x, Node* r)
    {
      if(x > r->key){ // Troca o valor da raiz pelo x
        int tmp = r->key;
        r->key = x;
        x = tmp;
      }

      if(r->left == nullptr){
        r->left = new Node(x,0);
        r->size++;
        return r;
      }
      if(r->left != nullptr && r->right == nullptr){
        r->right = new Node(x,0);
        r->size++;
        return r;
      }

      if(!IsComplete(r->left) || IsComplete(r->right)){
        r->left = _Insert(x, r->left);
      }
      else{
        r->right = _Insert(x, r->right);
      }

      r->size++;
      return r;
    }

    // Deleta o nó da raiz enraizada em u e devolve a árvore resultante 
    Node* Sink(Node* u)
    {
      //std::cout << "Sinking " << (u == nullptr ? "NULL" : std::to_string(u->key)) << "\n";
      if(u->left == nullptr && u->right == nullptr) {
        return nullptr;
      }

      Node* max_child;
      int left_key = (u->left == nullptr ? -INFITY : u->left->key);
      int right_key = (u->right == nullptr ? -INFITY : u->right->key);

      if(left_key > right_key){
        max_child = u->left;
        u->key = max_child->key;
        u->left = Sink(u->left);
        if(u->left == nullptr) { u->left = u->right; u->right = nullptr;}
      }
      else{
        max_child = u->right;
        u->key = max_child->key;
        u->right = Sink(u->right);
      }

      u->size--;
      //PrintPre();
      return u;
    }

    // Verifica se árvore enraizada em r é completa
    // Equivalente a testar se log2(size(r) + 1) é inteiro
    bool IsComplete(Node* r)
    {
      int n = r->size + 1; // Tamanho da árvore enraizada em r
      if(n == 1) return false;
      return std::log2(n+1) == (int) std::floor(std::log2(n+1));
    }

    void _PrintPre(const Node* const u)
    {
      if(u == nullptr) return;
      std::cout << u->key << "(" << u->size << ") ";
      _PrintPre(u->left);
      _PrintPre(u->right);
    }
    void _Print(int num_rows, int num_cols, int output[], Node* u, bool is_left_child, int i_parent, int j_parent, int num_left_parent)
    {
      if(u == nullptr) return;

      int i = i_parent + 3, j;
      int num_left = (u->left == nullptr ? 0 : u->left->size + 1);
      if(is_left_child) {
        j = j_parent - num_left_parent + num_left; 
      }
      else {
        j = j_parent + num_left + 1;
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

      _Print(num_rows, num_cols, output, u->left, true, i, j, num_left);
      _Print(num_rows, num_cols, output, u->right, false, i, j, num_left);
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

   // Retorna o número de digitos de n. Converter para uma macro
    int number_of_digits(int n)
    {
      return (int) (std::log10(n) + 1);
    }

    // Retorna a maior chave da árvore
    int Max() 
    {
      return root->key;
    }

    // Retorna a menor chave do Heap
    int Min() 
    {
      return _Min(root)->key;
    }

    // Essa é a versão da abb. Adaptar para o heap
    Node* _Min(Node* u){
      if(u == nullptr) return nullptr; // Entra somente quando a árvore está vazia
      if(u->left != nullptr) return _Min(u->left);
      else return u;
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
      return 1 + height_left_tree;
    }

};

// Cópia de IsComplete para teste básico. "Recebe o size da sub-ávore enraizada em baixo"
bool IsComplete(int n)
{
  return std::log2(n+2) == (int) std::floor(std::log2(n+2));
}

// Testa função IsComplete. OK
void Teste0()
{
  for(int i = 0; i < 1000000; i++){
    if(IsComplete(i)) std::cout << i << "\n";
  }
}

// Primeiro teste para depurar Insert. Parece OK
void Teste1()
{
  Heap h = Heap();
  h.Insert(30);
  h.PrintPre();
  h.Print();
  h.Insert(29);
  h.PrintPre();
  h.Print();
  h.Insert(27);
  h.PrintPre();
  h.Print();
  h.Insert(25);
  h.PrintPre();
  h.Print();
  h.Insert(22);
  h.PrintPre();
  h.Print();
  h.Insert(18);
  h.PrintPre();
  h.Print();
  h.Insert(20);
  h.Insert(15);
  h.Insert(7);
  h.Insert(25);
  h.Insert(31);
  h.PrintPre();
  h.Print();
}

// Teste inicial para DeleteMin().
void Teste2()
{
  Heap h = Heap();
  h.Insert(30);
  h.Insert(24);
  h.Insert(25);
  h.Insert(19);
  h.Insert(18);
  h.Insert(7);
  h.PrintPre();
  h.Print();

  h.DeleteMin();
  h.PrintPre();
  h.Print();

  h.DeleteMin();
  h.PrintPre();
  h.Print();


  h.DeleteMin();
  h.PrintPre();
  h.Print();


  h.DeleteMin();
  h.PrintPre();
  h.Print();


  h.DeleteMin();
  h.PrintPre();
  h.Print();


  h.DeleteMin();
  h.PrintPre();
  h.Print();


}

int main()
{
  //Teste0();
  //Teste1();
  Teste2();
}
