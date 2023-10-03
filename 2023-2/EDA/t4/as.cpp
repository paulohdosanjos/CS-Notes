// Tarefa t4 - Árvore de segmentos estática
// Paulo Henrique Albuquerque
// 12542251

#include <iostream>
#include <algorithm>
#include <sstream> 
#include <cmath>
#include <assert.h>
#include <vector>

#define INFTY 12345

bool verbose = true;
#define DEBUG_MODE 0

#if DEBUG_MODE
#define DEBUG(x) if(verbose) std::cout << x 
#else
#define DEBUG(x)
#endif

class Node {
  public:
    int l;
    int r;
    bool left_open;
    bool right_open;
    Node* left;
    Node* right;
    std::vector<int> seg_list;

    Node(){};
};

class As {

  public:

    As(std::vector<std::pair<int, int>> S)
    { 
      int num_segs = S.size();
      DEBUG("num_segs = ");
      DEBUG(num_segs);
      DEBUG("\n");

      std::vector<int> e; e.push_back(-INFTY);
      for(int i = 0; i < num_segs; i++){
        e.push_back(S[i].first);
        e.push_back(S[i].second);
      }
      e.push_back(+INFTY);

      // Antes de ordenar
      DEBUG("e: ");
      for(int i = 0; i < e.size(); i++) {
        DEBUG(e[i]);
        DEBUG(" ");
      }
      DEBUG("\n");

      std::sort(e.begin(), e.end());

      // Depois de ordenar
      DEBUG("e: ");
      for(int i = 0; i < e.size(); i++) {
        DEBUG(e[i]);
        DEBUG(" ");
      }
      DEBUG("\n");

      e.erase( unique( e.begin(), e.end() ), e.end() );
      // Depois de remover duplicatas
      DEBUG("e: ");
      for(int i = 0; i < e.size(); i++) {
        DEBUG(e[i]);
        DEBUG(" ");
      }
      DEBUG("\n");


      int k = e.size() - 2; // Número de elementos distintos no conjunto e, tirando os extremos
      int num_leafs = 2*k + 1;
      int height = std::ceil(std::log2(num_leafs));

      DEBUG("height = ");
      DEBUG(height);
      DEBUG("\n");

      std::pair<Node*, int> p = Build(root, height, 0, e); 
      DEBUG("deu bom");
      DEBUG("\n");
      root = p.first; 
      //assert(p.second == 4*num_segs + 1);

      // Insere intervalos
      for(int i = 0; i < num_segs; i++) Insert(root, S, i);
    }

    void Insert(Node* r, std::vector<std::pair<int,int>> S, int i)
    {
      // O intervalo está contido em Si ?
      if(S[i].first <= r->l && S[i].second >= r->r)
      {
        r->seg_list.push_back(i);
        return;
      }

      // Nunca entro nesses if no caso de folhas
      if(!(Disjoint(S[i].first, S[i].second, r->left->l, r->left->left_open, r->left->r, r->left->right_open))) Insert(r->left, S, i);
      if(!(Disjoint(S[i].first, S[i].second, r->right->l, r->right->left_open, r->right->r, r->right->right_open))) Insert(r->right, S, i);
    }

    // Verifica se os intervalos [ul,ur], [vl, vr] são disjuntos
    bool Disjoint(int ul, int ur, int vl, bool l_left_open, int vr, bool r_right_open)
    {
     if(l_left_open && r_right_open) return vl >= ur || vr <= ul;
     if(l_left_open && !r_right_open) return vl >= ur || vr < ul;
     if(!l_left_open && r_right_open) return vl > ur || vr <= ul;
     else return vl > ur || vr < ul;
    }

    std::pair<Node*, int> Build(Node* r, int height, int i, std::vector<int> e)
    {
      if(height != 0) // Nó interno ou folha "superior"
      { 
        DEBUG("Nó interno");
        DEBUG("\n");
        r = new Node();

        std::pair<Node*, int> pl = Build(r->left, height - 1, i, e);
        r->left = pl.first;

        std::pair<Node*, int> pr = Build(r->right, height - 1, i + pl.second, e);
        r->right = pr.first;

        if(pl.second + pr.second == 0) // Folha "superior"
        {
          DEBUG("Folha superior\n");
          if(i % 2 == 0)
          {
            r->l = e[i/2];
            r->r = e[i/2 + 1];
            r->left_open = true;
            r->right_open = true;
          }
          else
          {
            r->l = e[i/2 + 1];
            r->r = e[i/2 + 1];
            r->left_open = false;
            r->right_open = false;
          }
            
          return std::make_pair(r, 1);
        }
        Union(r);
        return std::make_pair(r, pl.second + pr.second);
      }

      else // Folha
      {
        DEBUG("Folha");
        DEBUG("\n");
        int num_leafs = 2*(e.size() - 2) + 1;
        int h = std::ceil(std::log2(num_leafs)); 
        int x = 2*num_leafs - (1 << h); // Quantidade máxima de folhas no último nível

        //int x = 8*n + 2 - (1 << h);    
        if(i >= x) return std::make_pair(nullptr, 0);
        r = new Node();
        if(i % 2 == 0)
        {
          r->l = e[i/2];
          r->r = e[i/2 + 1];
          r->left_open = true;
          r->right_open = true;
        }
        else
        {
          r->l = e[i/2 + 1];
          r->r = e[i/2 + 1];
          r->left_open = false;
          r->right_open = false;
        }
        DEBUG((r->left_open ? "(" : "["));
        DEBUG(std::to_string(r->l) + "," + std::to_string(r->r));
        DEBUG((r->right_open ? ")" : "]"));
        DEBUG("\n");
        return std::make_pair(r, 1);
      }
    }

    // Imprime a árvore deitada. Para depuração
    void Print() const { _Print(root, 0); }

    // Imprime lista com os índices dos segmentos que contém x
    void Segments(int x)
    {
      std::vector<int> ans = std::vector<int>();
      _Segments(root, x, ans);
      DEBUG("deu certo\n");
      DEBUG(ans.size());
      DEBUG("\n");
      if(ans.size() == 0) std::cout << "nenhum segmento\n";
      else{
        for(int i = 0; i < ans.size(); i++) std::cout << ans[i] + 1 << " ";
        std::cout << "\n";
      }
    }

  private:

    void _Segments(Node* r, int x, std::vector<int>& ans)
    {
      DEBUG("adiciona ");
      for(int i = 0; i < r->seg_list.size(); i++) {
        DEBUG(r->seg_list[i]);
        DEBUG(" ");
        ans.push_back(r->seg_list[i]);
      }
      DEBUG("\n");

      if(r->left == nullptr) return;

      if(!Disjoint(x, x, r->left->l, r->left->left_open, r->left->r, r->left->right_open)) {
        DEBUG("indo para a esquerda\n");
        _Segments(r->left, x, ans);
      }
      else {
        DEBUG("indo para a direita\n");
        _Segments(r->right, x, ans);
      }
    }

    // Imprime árvore enraizada em u deitada, após i espaços em branco 
    void _Print(Node* u, int i) const
    {
      if(u == nullptr) return;
      _Print(u->left, i + 4); 
      std::string space (i, ' '); 
      std::cout << space;
      std::cout << (u->left_open ? "(" : "[") << (u->l == -INFTY ? "-inf" : std::to_string(u->l)) << "," << (u->r == INFTY ? "inf": std::to_string(u->r)) << (u->right_open ? ")" : "]");
      for(int i = 0; i < u->seg_list.size(); i++) std::cout << " " << u->seg_list[i] + 1;
      std::cout << "\n";
      _Print(u->right, i + 4);
    }

    // Guarda união dos intervalos dos filhos do nó interno u
    void Union(Node* u)
    {
      u->l = u->left->l;
      u->r = u->left->r;
      u->left_open = u->left->left_open;
      u->right_open = u->left->right_open;

      if(u->right != nullptr)
      {
        u->r = u->right->r;
        u->right_open = u->right->right_open;
      }
    }

    Node* root;
};

// Teste inicial
void Teste1()
{
  int n;
  std::cin >> n;
  std::vector<std::pair<int,int>> v;

  for(int i = 0; i < n; i++)
  {
    int l,r;
    std::cin >> l >> r;
    v.push_back(std::make_pair(l,r));
  }

  As a = As(v);
  a.Print();
  verbose = false;
  a.Segments(1);
  a.Segments(2);
  a.Segments(3);
  a.Segments(4);
  a.Segments(5);
  a.Segments(6);
  a.Segments(7);
  a.Segments(8);
  
}

void Teste2()
{
  int n;
  std::cin >> n;
  std::vector<std::pair<int,int>> v;

  for(int i = 0; i < n; i++)
  {
    int l,r;
    std::cin >> l >> r;
    v.push_back(std::make_pair(l,r));
  }

  As a = As(v);

  for(std::string line; std::getline(std::cin, line) ; ) {
    int opt, x;
    std::istringstream iss = std::istringstream(line);
    iss >> opt >> x;

    switch (opt) {
      case 1:
        a.Segments(x);
        break;
      case 2:
        a.Print();
        break;
    }
  }
}
int main()
{
  //Teste1();
  Teste2();
}
