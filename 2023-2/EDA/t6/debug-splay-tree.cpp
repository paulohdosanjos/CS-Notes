// Tarefa t6 - Árvore splay 
// Paulo Henrique Albuquerque
// 12542251

#include <iostream>
#include <sstream>
#include <assert.h>

#define DEBUG_MODE 0
#if DEBUG_MODE
#define DEBUGLN(x) std::cout << x << "\n"
#define DEBUG(x) std::cout << x
#else
#define DEBUGLN(x) 
#define DEBUG(x) 
#endif


class Node {
  public:
    int key;
    Node* left;
    Node* right;
    Node* parent;

    Node(int _key, Node* _left, Node* _right, Node* _parent)
      : key(_key), left(_left), right(_right), parent(_parent) {}
};

class SplayTree {

  public:

    SplayTree () { root = nullptr; }

    void Insert (const int x) 
    { 
      auto p = _Insert(root, nullptr, x);
      root = p.first; 
      root = _Splay(p.second);
    }

    bool Search (const int x) 
    {
      DEBUGLN("Search(" << std::to_string(x) << ")");
      std::pair<int, Node*> p = _Search(root, nullptr, x);
      assert(p.first == false || x == p.second->key);
      DEBUG("Nó mais profundo : "); 
      DEBUGLN(p.second->key); 
      DEBUGLN("************************");
      root = _Splay(p.second);
      return p.first;
    }

    // Retorna menor chave presente na árvore. Não deve ser chamada para árvore vazia
    int Min () 
    { 
      Node* min_node = _Min(root); 
      root = _Splay(min_node);
      return min_node->key;
    }

    // Remove nó com chave x da treap 
    void Delete(const int x) 
    { 
      DEBUGLN("Delete(" << std::to_string(x) << ")");
      auto p = _Delete(root, x);
      DEBUG("Nó mais profundo : "); 
      DEBUGLN( (p.second == nullptr ? "null" : std::to_string(p.second->key) ) ); 
      DEBUGLN("************************");
      root = p.first;
      root = _Splay(p.second);
    }

    // Remove nó com chave x na árvore enraizada em r. Retorna a árvore resultante e o nó mais profundo 
    std::pair<Node*, Node*> _Delete(Node* r, const int x)
    {
      std::pair<Node*, Node*> p;
      if(x > r->key){
        p = _Delete(r->right, x);
        r->right = p.first; 
        if(p.first != nullptr) p.first->parent = r;
        return std::make_pair(r, p.second); 
      }

      else if(x < r->key){
        p = _Delete(r->left, x);
        r->left = p.first; 
        if(p.first != nullptr) p.first->parent = r;
        return std::make_pair(r, p.second); 
      }

      else // x == r->key
      { 
        if(r->left == nullptr){ // Sem filho esquerdo. Faz shortcut. Nó mais profundo alcançado foi o pai de r (talvez seja nulo)
          return std::make_pair(r->right, r->parent);
        }
        else if(r->right == nullptr) // Sem filho direito. Com filho esquerdo
        {
          return std::make_pair(r->left, r->parent);
        }
        else // Tem os dois filhos
        { 
          Node* min_right = _Min(r->right); 
          r->key = min_right->key;
          auto p = _Delete(min_right, min_right->key);
          min_right->parent->left = p.first;
          if(p.first != nullptr) p.first->parent = min_right->parent;
          return std::make_pair(r, p.second);
        }
      }
    }

    // Retorna nó com menor chave na árvore enraizada em r
    Node* _Min(Node* r)
    {
      if(r->left != nullptr) return _Min(r->left);
      else return r;
    }

    Node* _Splay(Node* u)
    {
      if(u == nullptr) return root; // Caso em que chamamos splay com nullptr, ou seja, o nó de interesse já esta na raiz

      while(u->parent != nullptr) {
        _SplayOperation(u);
        //Print();
        DEBUGLN("************************");
      }
      return u;
    }

    // Faz splay operation no nó u
    void _SplayOperation(Node* u)
    {
      assert(u != nullptr);

      Node* p = u->parent;

      if(p == nullptr) return;

      Node* g = p->parent;

      if(g == nullptr) // Single rotations
      {
        if(p->left == u) // u é filho esquerdo. Faz r splay step
        {
          DEBUGLN("r splay step");
          _RotateRight(u);
        }
        else // u é filho direito. Faz l splay step
        {
          DEBUGLN("l splay step");
          _RotateLeft(u);
        }
      }
      else // Tem avô. Fazer double rotations
      {
        if(u == p->left && p == g->left) // rr splay step
        {
          DEBUGLN("rr splay step");
          _RotateRight(p);
          _RotateRight(u);
        }
        if(u == p->right && p == g->right) // ll splay step
        {
          DEBUGLN("ll splay step");
          _RotateLeft(p);
          _RotateLeft(u);
        }
        if(u == p->left && p == g->right) // rl splay step
        {
          DEBUGLN("rl splay step");
          _RotateRight(u);
          _RotateLeft(u);
        }
        if(u == p->right && p == g->left) // lr splay step
        {
          DEBUGLN("lr splay step");
          _RotateLeft(u);
          _RotateRight(u);
        }
      }
    }

    void _RotateLeft(Node* u) { return _Rotate(u, false); }

    void _RotateRight(Node* u) { return _Rotate(u, true); }

    // Sobe nó u rotacionando para a direita caso isRightRotation = true, e para esquerda caso contrário. Atualiza ponteiros afetados. Pressupõe que u é filho esquerdo de um pai não nulose for uma rotação para a direita e pressupõe que u é filho direito caso contrário
    void _Rotate (Node* u, bool isRightRotation)
    {
      DEBUG("Rotate " << (isRightRotation ? "Right " : "Left "));
      DEBUGLN(u->key);
      Node* p = u->parent;
      assert(p != nullptr);
      Node* g = p->parent;

      /* 
      Caso 1: u não tem avô
         p            u     |   p            u      
        / \          / \    |  / \          / \    
       u   C  -->   A   p   | C   u  -->   p   B    
      / \              / \  |    / \      / \  
     A   B            B   C |   A   B    C   A 
          RotateRight               RotateLeft
      */

      Node* A = u->left;
      Node* B = u->right;

      // Atualiza ponteiros de u
      if(isRightRotation) u->right = p;
      else u->left = p;
      u->parent = g; 

      // Atualiza ponteiros de p
      if(isRightRotation) p->left = B;
      else p->right = A;
      p->parent = u;

      // Atualiza ponteiros de B ou A
      if(isRightRotation && B != nullptr) B->parent = p;
      if(!isRightRotation && A != nullptr) A->parent = p;

      if(g == nullptr) {
        DEBUGLN("Caso 1");
      }

      /* 
      Caso 2: u tem avô e p é filho esquerdo
           g            g     |     g           g  
          / \          / \    |    / \         / \ 
         p   D        u   D   |   p   D       u   D
        / \          / \      |  / \         / \    
       u   C  -->   A   p     | C   u  -->  p   B    
      / \              / \    |    / \     / \  
     A   B            B   C   |   A   B   C   A 
          RotateRight               RotateLeft
      */

      else if(g->left == p)
      {
        DEBUGLN("Caso 2");
        // Atualiza ponteiros de g
        g->left = u;
      }

      /* 
      Caso 3: u tem avô e p é filho direito
        g             g       |  g            g  
       / \           / \      | / \          / \ 
      D   p         D   u     |D   p        D   u
         / \           / \    |   / \          / \    
        u   C  -->    A   p   |  C   u  -->   p   B    
       / \               / \  |     / \      / \  
      A   B             B   C |    A   B    C   A 
          RotateRight               RotateLeft
      */

      else
      {
        DEBUGLN("Caso 3");
        // Atualiza ponteiros de g
        g->right = u;
      }
   }

    // Procura nó com chave x na árvore enraiza em r. Retorna true se x está presente em r e um ponteiro para nó mais profundo alcançado
    std::pair<bool, Node*> _Search (Node* r, Node* r_parent, const int x)
    {
      if(r == nullptr) return std::make_pair(false, r_parent);

      if(x > r->key) return _Search(r->right, r, x);
      else if(x < r->key) return _Search(r->left, r, x);
      else return std::make_pair(true, r);
    }

    // Imprime a árvore 
    void Print() const { _Print(root, 0); }
    
  
  private:

    // Imprime nó u depois de i espaços na saída padrão
    void _Print (Node* u, int i) const
    {
      if(u == nullptr) return;
      _Print(u->left, i + 3); 
      std::string space (i, ' '); 
      std::cout << space;
      std::cout << u->key << "\n";
      //std::cout << u->key << "(" << (u->parent == nullptr ? "null" : std::to_string(u->parent->key)) << ")\n"; // depuração do nó parent
      _Print(u->right, i + 3);
    }
    
    // Insere nó com chave x na árvore enraizada em r, onde r_parent é o pai de r. Devolve no segundo campo o nó mais profundo visitado
    std::pair<Node*, Node*> _Insert (Node* r, Node* r_parent, const int x)
    {
      if(r == nullptr) // Árvore vazia
      {
        Node* new_leaf = new Node(x, nullptr, nullptr, r_parent);
        return std::make_pair(new_leaf, new_leaf); 
      }

    std::pair<Node*, Node*> p;
    if(x > r->key) {
      p = _Insert(r->right, r, x);
      r->right = p.first;
    }
    else if(x < r->key) {
      p = _Insert(r->left, r, x);
      r->left = p.first;
    }

    return std::make_pair(r, p.second);
    }

    Node* root;
};

// Teste inicial para insert. Parece OK
void Teste1 ()
{
  std::cout << "*******************Teste1*******************\n";
  int insertion_list[] = {8, 3, 10, 1, 6, 14, 4, 7, 13};
  int n = sizeof(insertion_list) / sizeof(insertion_list[0]);
  SplayTree st = SplayTree();
  for(int i = 0; i < n ; i++) st.Insert(insertion_list[i]);

  st.Print();
}

// Teste para primeiro retorno de Search(). Parece OK
void Teste2 ()
{
  std::cout << "*******************Teste2*******************\n";
  int insertion_list[] = {8, 3, 10, 1, 6, 14, 4, 7, 13};
  int n = sizeof(insertion_list) / sizeof(insertion_list[0]);
  SplayTree st = SplayTree();
  for(int i = 0; i < n ; i++) st.Insert(insertion_list[i]);

  st.Print();

  int query_list[] = {3, 10, 2, 50, 11, 1, 7, 13, -1, 5, 2, 13, 0};
  int m = sizeof(query_list) / sizeof(query_list[0]);
  for(int i = 0; i < m ; i++) std::cout << "Search(" << query_list[i] << ") = " << st.Search(query_list[i]) << "\n"; 
}

// Teste inicial para Splay(). Parece OK
void Teste3 ()
{
  std::cout << "*******************Teste3*******************\n";
  int insertion_list[] = {8, 3, 10, 1, 6, 14, 4, 7, 13};
  int n = sizeof(insertion_list) / sizeof(insertion_list[0]);
  SplayTree st = SplayTree();
  for(int i = 0; i < n ; i++) st.Insert(insertion_list[i]);

  st.Print();
  std::cout << "*****************************\n";

  int query_list[] = {7, 10, 1, 0, 15};
  int m = sizeof(query_list) / sizeof(query_list[0]);
  for(int i = 0; i < m ; i++) {
    std::cout << "Search(" << query_list[i] << ") = " << st.Search(query_list[i]) << "\n"; 
    st.Print();
    std::cout << "*****************************\n";
  }
}

// Teste inicial para Min(). Parece OK
void Teste4 ()
{
  int insertion_list[] = {8, 3, 10, 1, 6, 14, 4, 7, 13};
  int n = sizeof(insertion_list) / sizeof(insertion_list[0]);
  SplayTree st = SplayTree();
  for(int i = 0; i < n ; i++) st.Insert(insertion_list[i]);

  st.Print();
  std::cout << "*****************************\n";

  std::cout << "min = " << st.Min() << "\n";
  st.Print();
}

// Teste para Delete()
void Teste5 ()
{
  int insertion_list[] = {8, 3, 10, 1, 6, 14, 4, 7, 13};
  int n = sizeof(insertion_list) / sizeof(insertion_list[0]);
  SplayTree st = SplayTree();
  for(int i = 0; i < n ; i++) st.Insert(insertion_list[i]);

  st.Print();
  std::cout << "*****************************\n";

  int query_list[] = {7, 8, 1, 6, 14, 4, 13, 3, 10};
  int m = sizeof(query_list) / sizeof(query_list[0]);
  for(int i = 0; i < m ; i++) {
    st.Delete(query_list[i]);
    st.Print();
    std::cout << "*****************************\n";
  }
}

void Teste ()
{
  SplayTree st = SplayTree();

  for(std::string line; std::getline(std::cin, line) ; ) {
    int opt; int x; 
    std::istringstream iss = std::istringstream(line);
    iss >> opt >> x;
    switch (opt) {
      case 1:
        st.Insert(x);
        break;
      case 2:
        st.Delete(x);
        break;
      case 3:
        std::cout << st.Search(x) << "\n";
        break;
      case 4:
        std::cout << st.Min() << "\n";
        break;
      case 5:
        st.Print();
        break;
    }
  }
}
int main()
{
  //Teste1();
  //Teste2();
  //Teste3();
  //Teste4();
  //Teste5();
  Teste();
}



