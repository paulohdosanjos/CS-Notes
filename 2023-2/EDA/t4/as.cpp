// Tarefa t4 - Árvore de segmentos estática
// Paulo Henrique Albuquerque
// 12542251

#include <iostream>
#include <algorithm>
#include <cmath>
#include <assert.h>
#include <vector>

#define INFTY 12345

bool verbose = true;
#define DEBUG_MODE 1

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
      int n = S.size();
      DEBUG("n = ");
      DEBUG(n);
      DEBUG("\n");
      int e[2*n+2];
      e[0] = -INFTY;
      e[2*n+1] = +INFTY;

      int k = 1;
      for(int i = 0; i < n; i++){
        e[k] = S[i].first;
        e[k+1] = S[i].second;
        k += 2;
      } 

      // Antes de ordenar
      DEBUG("e: ");
      for(int i = 0; i < 2*n+2; i++) {
        DEBUG(e[i]);
        DEBUG(" ");
      }
      DEBUG("\n");

      std::sort(e, e + 2*n + 2);
      // Depois de ordenar
      DEBUG("e: ");
      for(int i = 0; i < 2*n+2; i++) {
        DEBUG(e[i]);
        DEBUG(" ");
      }
      DEBUG("\n");

      int height = std::floor(std::log2(4*n+1)) + 1;

      DEBUG("height = ");
      DEBUG(height);
      DEBUG("\n");

      std::pair<Node*, int> p = Build(root, height, 0, e, n, height); 
      DEBUG("deu bom");
      DEBUG("\n");
      root = p.first; 
      assert(p.second == 4*n + 1);

      // Insere intervalos
      for(int i = 0; i < n; i++) Insert(root, S, i);
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
     if(!l_left_open && !r_right_open) return vl > ur || vr < ul;
    }

    std::pair<Node*, int> Build(Node* r, int height, int i, int e[], int n, int h)
    {
      if(height != 0) // Nó interno ou folha "superior"
      { 
        DEBUG("Nó interno");
        DEBUG("\n");
        r = new Node();

        std::pair<Node*, int> pl = Build(r->left, height - 1, i, e, n, h);
        r->left = pl.first;

        std::pair<Node*, int> pr = Build(r->right, height - 1, i + pl.second, e, n, h);
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
        int x = 8*n + 2 - (1 << h);  // Quantidade máxima de folhas no último nível
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

  private:

    // Imprime árvore enraizada em u deitada, após i espaços em branco 
    void _Print(Node* u, int i) const
    {
      if(u == nullptr) return;
      _Print(u->left, i + 4); 
      std::string space (i, ' '); 
      std::cout << space;
      std::cout << (u->left_open ? "(" : "[") << std::to_string(u->l) << "," << std::to_string(u->r) << (u->right_open ? ")" : "]") << "\n";
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
}

int main()
{
  Teste1();
}
