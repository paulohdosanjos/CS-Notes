// Tarefa t4 - Árvore de segmentos dinâmica 
// Paulo Henrique Albuquerque
// 12542251

#include <deque>

#include "as.cpp" // Implementação da AS estática

class Asd {
  public: 

    // Cria Asd vazia
    Asd() { n = 0; SegTreesList = std::deque<As*>(); }

    // Imprime todos os segmentos que contém x
    void Segments(int x)
    {
      std::vector<int> ans = std::vector<int>();
      for(auto a : SegTreesList)
        ans = Concatenate(ans, a->Segments(x));

      if(ans.size() == 0) std::cout << "nenhum elemento\n";

      else
      {
        for(int i = 0; i < ans.size(); i++) 
          std::cout << ans[i] << " ";
        std::cout << "\n";
      }
    }

    // Insere intervalo [l, r] na árvore
    void Insert(int l, int r)
    {
      n++; 
      std::vector<seg> h; 
      h.push_back(std::make_tuple(n,l,r));
      As* a = new As(h);
      SegTreesList.push_back(a);

      // Enquanto há pelo menos duas árvores
      while(SegTreesList.size() >= 2)
      {
        // Extrai as duas primeiras árvores. 
        As* a = SegTreesList.back();
        SegTreesList.pop_back();
        As* b = SegTreesList.back();
        SegTreesList.pop_back();
        
        auto S1 = a->GetS();
        auto S2 = b->GetS();

        if(S1.size() == S2.size()) // Joga fora as duas e cria uma nova com os segmentos delas
        {
          auto S = Concatenate(S1, S2);
          As* c = new As(S);
          SegTreesList.push_back(c);

          // Aqui deveríamos chamar o destrutor das árvores a e b
        }

        else // Coloca de volta na pilha as árvores que removi e sai do laço
        {
          SegTreesList.push_back(b);
          SegTreesList.push_back(a);
          break;
        }
      }
    }

    // Concatena dois vetores e retorna o vetor resultante
    template <class T>
    std::vector<T> Concatenate(std::vector<T> S1, std::vector<T> S2)
    {
      std::vector<T> S;
      for(int i = 0; i < S1.size(); i++) S.push_back(S1[i]);
      for(int i = 0; i < S2.size(); i++) S.push_back(S2[i]);
      return S;
    }

    // Chama Print() para cada árvore estática que compõe a árvore dinâmica
    void PrintAds()
    { 
      for(auto a : SegTreesList) {
        a->Print(); 
        //std::cout << "****************\n";
      }
    }

  private:
    int n; // Número de inserções
    std::deque<As*> SegTreesList; // Pilha de árvores estáticas

};

int main()
{
  Asd ad = Asd();

  for(std::string line; std::getline(std::cin, line) ; ) {
    int opt, s, f;
    std::istringstream iss = std::istringstream(line);
    iss >> opt >> s >> f;
    switch (opt) {
      case 1:
        ad.Insert(s, f);
        break;
      case 2:
        ad.Segments(s);
        break;
      case 3:
        ad.PrintAds();
        break;
    }
  }
}
