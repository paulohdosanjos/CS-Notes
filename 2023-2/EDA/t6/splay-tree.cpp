// Tarefa t6 - Árvore splay 
// Paulo Henrique Albuquerque
// 12542251

#include <iostream>
#include <sstream>

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

    SplayTree (Node* r) { root = r;}

    // Constrói splay tree vazia
    SplayTree () { root = nullptr; }


    // Insere novo nó com chave x na splay tree
    void Insert (const int x) 
    { 
      auto p = _Insert(root, nullptr, x);
      root = p.first; 
      root = _Splay(p.second);
    }

    // Procura por chave x na árvore. Além de retornar o resultado da busca, aciona Splay() para o nó mais profundo atingido durante a busca
    bool Search (const int x) 
    {
      auto p = _Search(root, nullptr, x);
      root = _Splay(p.second);
      return p.first;
    }

    // Retorna menor chave presente na árvore e aciona Splay() para o nó correspondente. Não deve ser chamada para árvore vazia
    int Min () 
    { 
      Node* min_node = _Min(root); 
      root = _Splay(min_node);
      return min_node->key;
    }

    // Remove nó com chave x da árvore e aciona Splay() para o nó corresponde. Não deve ser chamada para x não presente na árvore
    void Delete(const int x) 
    { 
      auto p = _Delete(root, x);
      root = p.first;
      root = _Splay(p.second);
    }

    // Imprime a árvore deitada
    void Print() const { _Print(root, 0); }

    static std::pair<SplayTree*, SplayTree*> Split (SplayTree* st, const int x)
    {
      auto p = st->_Search(st->root, nullptr, x);
      Node* s1 = st->_Splay(p.second);  
      SplayTree* st2 = new SplayTree(s1->right);
      s1->right = nullptr;
      SplayTree* st1 = new SplayTree(s1);
      return std::make_pair(st1,st2);
    }

    static SplayTree* Join (SplayTree* S, SplayTree* T)
    {
      Node* min_t = T->_Min(T->root);
      Node* s1 = T->_Splay(min_t);
      s1->left = S->root;
      return new SplayTree(s1);
    }
    
  
  private:

    // Remove nó com chave x da árvore enraizada em r. Retorna a árvore resultante e o nó mais profundo atingindo durante a remoção 
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
        // Sem filho esquerdo. Faz shortcut. Nó mais profundo alcançado foi o pai de r (possivelmente nulo)
        if(r->left == nullptr) 
          return std::make_pair(r->right, r->parent);
        
        // Sem filho direito. Com filho esquerdo
        else if(r->right == nullptr)
          return std::make_pair(r->left, r->parent);
        
        // r tem os dois filhos
        else 
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

    // Retorna nó com menor chave na árvore enraizada em r. Não deve ser chamada para r == nullptr
    Node* _Min(Node* r)
    {
      if(r->left != nullptr) return _Min(r->left);
      else return r;
    }

    // Coloca nó u na raiz através de chamadas sucessivas de SplayOperation() nesse nó
    Node* _Splay(Node* u)
    {
      if(u == nullptr) return root; // Caso em que chamamos splay com nullptr, ou seja, o nó de interesse já está na raiz

      while(u->parent != nullptr) 
        _SplayOperation(u);

      return u;
    }

    void _SplayOperation(Node* u)
    {
      Node* p = u->parent;
      if(p == nullptr) return;
      Node* g = p->parent;

      if(g == nullptr) // Rotação simples 
      {
        if(p->left == u) // u é filho esquerdo. r splay step
          _RotateRight(u);

        else // u é filho direito. l splay step
          _RotateLeft(u);
      }

      else // u tem avô. Rotação dupla 
      {
        if(u == p->left && p == g->left) // rr splay step
        {
          _RotateRight(p);
          _RotateRight(u);
        }
        if(u == p->right && p == g->right) // ll splay step
        {
          _RotateLeft(p);
          _RotateLeft(u);
        }
        if(u == p->left && p == g->right) // rl splay step
        {
          _RotateRight(u);
          _RotateLeft(u);
        }
        if(u == p->right && p == g->left) // lr splay step
        {
          _RotateLeft(u);
          _RotateRight(u);
        }
      }
    }

    void _RotateLeft(Node* u) { return _Rotate(u, false); }

    void _RotateRight(Node* u) { return _Rotate(u, true); }

    // Sobe nó u rotacionando para a direita caso isRightRotation = true, e para esquerda caso contrário. Atualiza ponteiros afetados. Pressupõe que u é filho esquerdo de um pai não nulo se for uma rotação para a direita e pressupõe que u é filho direito caso contrário
    void _Rotate (Node* u, bool isRightRotation)
    {
      Node* p = u->parent;
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

      if(g == nullptr) {}

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
        // Atualiza ponteiros de g
        g->right = u;
      }
   }

    // Busca nó com chave x na árvore enraiza em r. Retorna true se x está presente em r e um ponteiro para nó mais profundo atingido 
    std::pair<bool, Node*> _Search (Node* r, Node* r_parent, const int x)
    {
      if(r == nullptr) return std::make_pair(false, r_parent);

      if(x > r->key) return _Search(r->right, r, x);
      else if(x < r->key) return _Search(r->left, r, x);
      else return std::make_pair(true, r);
    }


    // Imprime nó u depois de i espaços na saída padrão
    void _Print (Node* u, int i) const
    {
      if(u == nullptr) return;
      _Print(u->left, i + 3); 
      std::string space (i, ' '); 
      std::cout << space;
      std::cout << u->key << "\n";
      _Print(u->right, i + 3);
    }
    
    // Insere nó com chave x na árvore enraizada em r, onde r_parent é o pai de r. Devolve no segundo campo o nó mais profundo atingido
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

// Teste inicial para Split. Parece OK
void Teste1 ()
{
  std::cout << "*******************Teste1*******************\n";
  int insertion_list[] = {8, 3, 10, 1, 6, 14, 4, 7, 13};
  int n = sizeof(insertion_list) / sizeof(insertion_list[0]);
  SplayTree st = SplayTree();
  for(int i = 0; i < n ; i++) st.Insert(insertion_list[i]);
  std::cout << "Árvore original: \n";
  st.Print();

  int x = 7;
  auto p = SplayTree::Split(&st, x);

  std::cout << "Árvore com chaves menores ou igual a " << x << ":\n";
  p.first->Print();
  std::cout << "Árvore com chaves maiores que " << x << ":\n";
  p.second->Print();
}

// Teste inicial para Join. Parece OK
void Teste2 ()
{
  std::cout << "*******************Teste2*******************\n";
  int insertion_list1[] = {8, 3, 10, 1, 6, 14, 4, 7, 13};
  int n = sizeof(insertion_list1) / sizeof(insertion_list1[0]);
  SplayTree S = SplayTree();
  for(int i = 0; i < n ; i++) S.Insert(insertion_list1[i]);
  std::cout << "Árvore S: \n";
  S.Print();

  std::cout << "\n";

  int insertion_list2[] = {15, 14, 19, 22, 21};
  n = sizeof(insertion_list2) / sizeof(insertion_list2[0]);
  SplayTree T = SplayTree();
  for(int i = 0; i < n ; i++) T.Insert(insertion_list2[i]);
  std::cout << "Árvore T: \n";
  T.Print();

  SplayTree* R = SplayTree::Join(&S, &T);
  std::cout << "Árvore Join:\n";
  R->Print();
}

int main ()
{
  Teste2();
  return 0;
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
