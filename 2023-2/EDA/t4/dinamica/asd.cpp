// Tarefa t4 - Árvore de segmentos dinâmica 
// Paulo Henrique Albuquerque
// 12542251

#include <deque>

#include "as.cpp" // Implementação das AS estáticas

class Asd {
  public: 
    Asd() { n = 0; SegTreesList = std::deque<As*>(); }

    void Segments(int x)
    {
      std::vector<int> ans = std::vector<int>();
      for(auto a : SegTreesList)
      {
        ans = Concatenate(ans, a->Segments(x));
      }

      if(ans.size() == 0) std::cout << "nenhum elemento\n";
      else
      {
        for(int i = 0; i < ans.size(); i++) std::cout << ans[i] << " ";
        std::cout << "\n";
      }
    }

    void Insert(int l, int r)
    {
      n++;
      std::vector<seg> h; 
      h.push_back(std::make_tuple(n,l,r));
      As* a = new As(h);
      SegTreesList.push_back(a);

      // Enquanto há pelo menos dois elementos
      while(SegTreesList.size() >= 2)
      {
        As* a = SegTreesList.back();
        SegTreesList.pop_back();
        As* b = SegTreesList.back();
        SegTreesList.pop_back();
        
        auto S1 = a->S;
        auto S2 = b->S;

        if(S1.size() == S2.size()) // Joga fora as duas e cria uma nova com os segmentos delas
        {
          auto S = Concatenate(S1, S2);
          As* c = new As(S);
          SegTreesList.push_back(c);
          //delete[] a;
          //delete[] b;
        }

        else // Coloca de volta na pilha as árvores que removi e sai do laço
        {
          SegTreesList.push_back(b);
          SegTreesList.push_back(a);
          break;
        }
      }
    }

    template <class T>
    std::vector<T> Concatenate(std::vector<T> S1, std::vector<T> S2)
    {
      std::vector<T> S;
      for(int i = 0; i < S1.size(); i++) S.push_back(S1[i]);
      for(int i = 0; i < S2.size(); i++) S.push_back(S2[i]);
      return S;
    }

    // Chama Print() para cada árvore estática
    void PrintAds()
    { 
      for(auto a : SegTreesList) {
        a->Print(); 
        std::cout << "****************\n";
      }
    }

  //private:
    int n; // Número de inserções
    std::deque<As*> SegTreesList;

};

// Teste inicial para insert e Segments. Parece OK
void Teste1()
{
  Asd ad = Asd();
  ad.Insert(1, 8);
  ad.Insert(2, 4);
  ad.Insert(3, 9);

  std::cout << "n = " << ad.n << "\n";
  std::cout << "Segtree size = " << ad.SegTreesList.size() << "\n";

  ad.PrintAds();
  ad.Segments(1);
  ad.Segments(3);

  ad.Insert(1,5);
  std::cout << "n = " << ad.n << "\n";
  std::cout << "Segtree size = " << ad.SegTreesList.size() << "\n";
  ad.PrintAds();
  ad.Segments(1);
  ad.Segments(3);
}

void Teste2()
{
  Asd ad = Asd();
  ad.Insert(1, 8); // S1
  ad.Insert(2, 4); // S2
  ad.Insert(3, 9); // S3
  ad.Insert(3, 11); // S4
  ad.Insert(9, 12); // S5
  ad.Insert(4, 10); // S6

  std::cout << "n = " << ad.n << "\n";
  std::cout << "Segtree size = " << ad.SegTreesList.size() << "\n";
  ad.PrintAds();
  ad.Segments(7);
  ad.Segments(11);
}

int main()
{
  //Teste2();
  //return 0;
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
