// Tarefa t5 - Heap de certificados
// Paulo Henrique Albuquerque
// 12542251

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <gtest/gtest.h>

#define GARBAGE std::make_pair(-1,-1)
#define INFTY 123456

typedef std::pair<int, double> cert;

class Heap {
  
  public:
    
    // Cria heap vazio
    Heap()
    {
      h = std::vector<cert>(); h.push_back(GARBAGE); // Não usamos a primeira posição
      m = std::map<int, int>(); // Usar um map em vez de um array não altera a complexidade das funções do heap cinético
    }

    // Constrói heap a partir do array de certificados v
    Heap(cert v[], int n)
    {
      h = std::vector<cert>(); h.push_back(GARBAGE);
      m = std::map<int, int>(); 

      for(int i = 0; i < n; i++) {
        h.push_back(v[i]);

        // Atualiza mapeamento
        int id = v[i].first; 
        m[id] = h.size() - 1;
      }

      for(int i = h.size() / 2; i > 0; i--) Sink(i); // Restaura heap
    }

    // Insere cerficado c = (id, t) no heap
    void Insert(int id, double t)
    {
      h.push_back(std::make_pair(id, t));

      m[id] = h.size() - 1;

      Swim(h.size() - 1);
    }

    // Remove certificado com menor prioridade do heap
    void DeleteMin()
    {
      int cert_id = h[1].first;
      Swap(1, h.size() - 1);  // Substitui raiz pelo último elemento
      h.pop_back();
      Sink(1);
      m.erase(m.find(cert_id)); // Remove entrada do mapeamento
      //m[cert_id] = -1; // Verificar isso
    }

    // Retorna certificado (id, t) com menor t 
    cert Min() { return h[1]; }

    // Atualiza prioridade do certificado id para new_t
    void Update(const int id, const double new_t)
    {
      h[m[id]].second = new_t;
      Sink(m[id]);
      Swim(m[id]);
    }

    // Imprime representação de árvore do heap, junto com a representação de vetor. Para depuração
    void Print()
    {
      //_PrintTree(0, 1); 

      for(int i = 1; i <= h.size() - 1; i++) std::cout << "(" << h[i].first << "," << (h[i].second == INFTY ? "inf" : std::to_string(h[i].second)) << "), ";
      std::cout << "\n";
    }


  private:

    void _PrintTree(int s, int i)
    {
      if(i > h.size() - 1) return;
      _PrintTree(s + 3, 2*i);
      std::string space (s, ' ');
      std::cout << space;
      std::cout << h[i].second << "\n";
      _PrintTree(s + 3, 2*i + 1);
    }

    // Sobe elemento na posição i do vetor h para a sua posição correta no heap
    void Swim(int i)
    {
      if(i / 2 == 0) return;

      cert u = h[i];
      cert u_parent = h[i/2];
      
      if(u_parent.second > u.second) 
      { 
        Swap(i, i/2); 
        Swim(i/2);
      }
    }

    // Troca a posição dos certificados na posição i e j
    void Swap(const int i, const int j)
    {
      // Atualiza mapeamentos
      m[h[i].first] = j;
      m[h[j].first] = i;

      cert tmp = h[i];
      h[i] = h[j];
      h[j] = tmp;
    }

    // Desce o elemento na posição i do vetor h para a sua posição correta no heap
    void Sink(int i)
    {
      if(2*i > h.size() - 1) { // Não tem filho
        return; 
      }
      else if (2*i + 1 > h.size() - 1) // Só tem filho esquerdo
      {
        if(h[2*i].second < h[i].second)
        {
          Swap(i, 2*i);
          return;
        }
      }
      else // Tem os dois filhos
      {
        cert u = h[i];
        cert left_child = h[2*i];
        cert right_child = h[2*i + 1];

        if(left_child.second < u.second && 
           left_child.second < right_child.second)
        {
          Swap(i, 2*i);
          Sink(2*i);
        }

        if(right_child.second < u.second && 
           right_child.second < left_child.second)
        {
          Swap(i, 2*i + 1);
          Sink(2*i + 1);
        }
      }
    }
    
    std::vector<cert> h; // Vetor que representa árvore do heap
    std::map<int, int> m; // Mapeamento id -> índice em h

};

//TEST(Heap, Teste1)
//{
//  Heap h = Heap();
//  double insertion_list[] = {27, 17, 18, 19, 11, 5, 9, 33, 14, 21};
//  int size = (int) (sizeof(insertion_list) / sizeof(insertion_list[0]));
//
//  for(int i = 0; i < size; i++){
//    h.Insert(i + 2, insertion_list[i]);
//  }
//
//  h.Print();
//  cert min = h.Min();
//  //std::cout << "(" <<  min.first << "," << min.second << ")\n";
//
//  std::cout << "Deletando mínimo\n";
//  h.DeleteMin();
//  h.Print();
//  min = h.Min();
//  std::cout << "(" <<  min.first << "," << min.second << ")\n";
//
//  std::cout << "Deletando mínimo\n";
//  h.DeleteMin();
//  h.Print();
//  min = h.Min();
//  std::cout << "(" <<  min.first << "," << min.second << ")\n";
//
//  std::cout << "Deletando mínimo\n";
//  h.DeleteMin();
//  h.Print();
//  min = h.Min();
//  std::cout << "(" <<  min.first << "," << min.second << ")\n";
//}
////
////TEST (Heap, Teste2)
////{
////  Heap h = Heap();
////  int insertion_list[] = {27, 17, 18, 19, 11, 5, 9, 33, 14, 21};
////  int size = (int) (sizeof(insertion_list) / sizeof(insertion_list[0]));
////
////  for(int i = 0; i < size; i++){
////    h.Insert(i + 2, insertion_list[i]);
////  }
////
////  h.Print();
////}
////
//TEST(Heap, Teste3)
//{
//  Heap h = Heap();
//  int insertion_list[] = {27, 17, 18, 19, 11, 5, 9, 33, 14, 21};
//  int size = (int) (sizeof(insertion_list) / sizeof(insertion_list[0]));
//
//  for(int i = 0; i < size; i++){
//    h.Insert(i + 2, insertion_list[i]);
//  }
//
//  h.Print();
//
//  h.Update(2, 3);
//  h.Print();
//
//  h.Update(9, 12);
//  h.Print();
//}
//
//TEST (Heap, Teste4)
//{
//  Heap h = Heap();
//  h.Insert(2,3);
//  h.Insert(3,11);
//  h.Insert(4,11);
//
//  h.Print();
//  cert min = h.Min();
//  std::cout << "(" <<  min.first << "," << min.second << ")\n";
//
//  h.DeleteMin();
//  h.Print();
//  min = h.Min();
//  std::cout << "(" <<  min.first << "," << min.second << ")\n";
//
//  h.DeleteMin();
//  h.Print();
//  min = h.Min();
//  std::cout << "(" <<  min.first << "," << min.second << ")\n";
//
//}
////
//// Delete + Insert pode dar ruim
//TEST (Heap, Teste5)
//{
//  Heap h = Heap();
//  h.Insert(2,3);
//  h.Insert(3,10);
//  h.Insert(4,11);
//  h.Print();
//
//  cert min = h.Min();
//  h.DeleteMin();
//  h.Print();
//
//  h.Insert(min.first, 4);
//  h.Print();
//
//}
////
////// Criado heap a partir de vetor
//TEST(Heap, Teste6)
//{
//  cert v[10];
//  v[0] = std::make_pair(2,27);
//  v[1] = std::make_pair(3,17);
//  v[2] = std::make_pair(4,18);
//  v[3] = std::make_pair(5,19);
//  v[4] = std::make_pair(6,11);
//  v[5] = std::make_pair(7,5);
//  v[6] = std::make_pair(8,9);
//  v[7] = std::make_pair(9,33);
//  v[8] = std::make_pair(10,14);
//  v[9] = std::make_pair(11,21);
//
//  Heap h = Heap(v, 10);
//  h.Print();
//}
//
//int main(int argc, char** argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//  }
