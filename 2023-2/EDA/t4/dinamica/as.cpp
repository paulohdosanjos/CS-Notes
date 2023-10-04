// Tarefa t4 - Árvore de segmentos estática para a implementação da árvore dinâmica
// Paulo Henrique Albuquerque
// 12542251

#include <iostream>
#include <algorithm>
#include <sstream> 
#include <cmath>
#include <assert.h>
#include <vector>

#define INFTY 123456

typedef std::tuple<int,int,int> seg;

class Node {
  public:
    // Intervalo = [? l, r ]?
    int l;         
    int r;
    bool left_open; // (l, r 
    bool right_open; // l, r) 
    Node* left;
    Node* right;
    std::vector<int> seg_list; 

};

class As {

  public:

    // Constrói árvore de segmentos estática a partir dos segmentos em S
    As(std::vector<seg> _S)
    { 
      S = _S;
      int num_segs = S.size();

      // Cónstroi array de intervalos elementares
      std::vector<int> e; e.push_back(-INFTY);
      for(int i = 0; i < num_segs; i++){
        e.push_back(std::get<1>(S[i]));
        e.push_back(std::get<2>(S[i]));
      }
      e.push_back(+INFTY);

      std::sort(e.begin(), e.end());
      e.erase( unique( e.begin(), e.end() ), e.end() ); // Remove duplicatas

      int k = e.size() - 2; // Número de elementos distintos no array, tirando os extremos
      int num_leafs = 2*k + 1; // Número de folhas na árvore. k folhas do tipo [ei,ei] e k+1 do tipo (ei,ei+1)
                               //
      int height = std::ceil(std::log2(num_leafs)); // Altura da árvore

      root = _Build(root, height, 0, e).first; 

      // Insere intervalos um a um
      for(int i = 0; i < S.size(); i++) _Insert(root, S[i]);
    }

        // Imprime a árvore deitada.
    void Print() const { _Print(root, 0); }

    // Retorna lista com os índices dos segmentos que contém x
    std::vector<int> Segments(int x)
    {
      std::vector<int> ans = std::vector<int>();
      _Segments(root, x, ans);
      return ans;
    }

  private:
    
    // Insere segmento s nas listas de segmentos dos nós da árvore enraizada em r
    void _Insert(Node* r, seg s)
    {
      // O intervalo está contido em Si ? 
      if(std::get<1>(s) <= r->l && std::get<2>(s) >= r->r)
      {
        // Adiciona na lista e para
        r->seg_list.push_back(std::get<0>(s));
        return;
      }

      // Nunca entro nesses ifs no caso r = folha. Então, sem segfault 
      // Caso o filho esquerdo tenha interseção com Si, insiro nele
      if(!(Disjoint(std::get<1>(s), std::get<2>(s), r->left->l, r->left->left_open, r->left->r, r->left->right_open))) _Insert(r->left, s);
      // Mesma coisa para o filho direito
      if(!(Disjoint(std::get<1>(s), std::get<2>(s), r->right->l, r->right->left_open, r->right->r, r->right->right_open))) _Insert(r->right, s);
    }

    // Verifica se os intervalos [ul,ur], [? vl, vr ]? são disjuntos
    bool Disjoint(int ul, int ur, int vl, bool l_left_open, int vr, bool r_right_open)
    {
     if(l_left_open && r_right_open) return vl >= ur || vr <= ul;
     if(l_left_open && !r_right_open) return vl >= ur || vr < ul;
     if(!l_left_open && r_right_open) return vl > ur || vr <= ul;
     else return vl > ur || vr < ul;
    }

    // Função recursiva para construção da árvore de segmentos. height é a altura da árvore alvo (constante). i é a quantidade de folhas já adicionadas e e[] é o vetor de intervalos elementares. Retorna a árvore resultante e a quantidade de folhas adicionadas no processo.
    std::pair<Node*, int> _BuildTree(Node* r, int height, int i, std::vector<int> e)
    {
      if(height != 0) // Nó interno ou folha de nível "superior"
      { 
        r = new Node();

        std::pair<Node*, int> pl = _BuildTree(r->left, height - 1, i, e);
        r->left = pl.first;

        std::pair<Node*, int> pr = _BuildTree(r->right, height - 1, i + pl.second, e);
        r->right = pr.first;

        if(pl.second + pr.second == 0) // Folha de nível "superior"
        {
          AddLeaf(r, i, e); // Adiciona o próximo intervalo elementar em r
          return std::make_pair(r, 1);
        }

        Union(r); // Calcula intervalo de r a partir dos filhos
        return std::make_pair(r, pl.second + pr.second);
      }

      else // Folha do último nível
      {
        int num_leafs = 2*(e.size() - 2) + 1;
        int h = std::ceil(std::log2(num_leafs)); 
        int x = 2*num_leafs - (1 << h); // Quantidade máxima de folhas no último nível. Impomos esse limite por causa das folhas do nível acima

        // Limite já atingido. Não adiciona mais nada nesse nível
        if(i >= x) return std::make_pair(nullptr, 0);

        r = new Node();
        AddLeaf(r, i, e);
        return std::make_pair(r, 1);
      }
    }

    // Insere i-ésimo intervalo elementar em r
    void AddLeaf(Node* r, int i, std::vector<int> e)
    {
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
    }

    // Função recursiva para Segments(). Escreve no vetor ans os segmentos no caminho da raiz até a folha que contém x
    void _Segments(Node* r, int x, std::vector<int>& ans)
    {
      // Adiciona segmentos da lista de segmentos do nó atual no caminho
      for(int i = 0; i < r->seg_list.size(); i++) 
        ans.push_back(r->seg_list[i]);

      // Caso seja folha, nada a fazer mais
      if(r->left == nullptr) return;

      // Desço no caminho a procura de x
      if(!Disjoint(x, x, r->left->l, r->left->left_open, r->left->r, r->left->right_open)) {
        _Segments(r->left, x, ans);
      }
      else {
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

      for(int i = 0; i < u->seg_list.size(); i++) std::cout << " " << u->seg_list[i];
      std::cout << "\n";

      _Print(u->right, i + 4);
    }

    // Insere em u a união dos intervalos dos filhos de u
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
    std::vector<seg> S;
};


//int main()
//{
//  int n;
//  std::cin >> n;
//  std::vector<seg> v;
//
//  for(int i = 0; i < n; i++)
//  {
//    int l,r;
//    std::cin >> l >> r;
//    v.push_back(std::make_tuple(i+1,l,r));
//  }
//
//  As a = As(v);
//
//  for(std::string line; std::getline(std::cin, line) ; ) {
//    int opt, x;
//    std::istringstream iss = std::istringstream(line);
//    iss >> opt >> x;
//
//    switch (opt) {
//      case 1:
//        a.Segments(x);
//        break;
//      case 2:
//        a.Print();
//        break;
//    }
//  }
//}
