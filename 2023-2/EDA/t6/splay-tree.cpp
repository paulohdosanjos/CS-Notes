// Tarefa t6 - Árvore splay 
// Paulo Henrique Albuquerque
// 12542251

#include <iostream>
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

    void Insert (const int x) { root = _Insert(root, nullptr, x); }

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

    Node* _Splay(Node* u)
    {
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
    
    // Insere nó com chave x na árvore enraizada em r, onde r_parent é o pai de r
    Node* _Insert (Node* r, Node* r_parent, const int x)
    {
      if(r == nullptr) return new Node(x, nullptr, nullptr, r_parent); // Árvore vazia

    if(x > r->key) r->right = _Insert(r->right, r, x);
    else if(x < r->key) r->left = _Insert(r->left, r, x);

    return r;
    }

    Node* root;
};

// Teste inicial para insert. Parece OK
void Teste1 ()
{
  int insertion_list[] = {8, 3, 10, 1, 6, 14, 4, 7, 13};
  int n = sizeof(insertion_list) / sizeof(insertion_list[0]);
  SplayTree st = SplayTree();
  for(int i = 0; i < n ; i++) st.Insert(insertion_list[i]);

  st.Print();
}

// Teste para primeiro retorno de Search(). Parece OK
void Teste2 ()
{
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

int main()
{
  //Teste1();
  //Teste2();
  Teste3();
}



