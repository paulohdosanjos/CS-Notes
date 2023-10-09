// Tarefa t5 - Heap de certificados
// Paulo Henrique Albuquerque
// 12542251

#include <iostream>
#include <vector>
#include <string>
#include <gtest/gtest.h>

#define GARBAGE std::make_pair(-1,-1)
#define INFTY 123456

#define DEBUG_MODE 0
#if DEBUG_MODE
#define DEBUG(x) std::cout << x
#else
#define DEBUG(x) 
#endif

typedef std::pair<int, double> cert;

class Heap {
  
  public:
    
    // Cria heap vazio
    Heap()
    {
      h = std::vector<cert>(); h.push_back(GARBAGE); // Não usamos a primeira posição
      map = std::vector<int>(); map.push_back(-1); map.push_back(-1); // Não usamos a duas primeiras posições, pois os ids dos certificados começam de 2
    }

    // Constrói heap a partir do vetor de certificados v
    Heap(std::vector<cert> v)
    {
      h = std::vector<cert>(); h.push_back(GARBAGE);
      map = std::vector<int>(); map.push_back(-1); map.push_back(-1);

      for(int i = 0; i < v.size(); i++) {
        h.push_back(v[i]);
        int current_heap_size = h.size() - 1;
        int id = v[i].first;
        if(id > current_heap_size) map.push_back(1); // Só adiciona mais uma posição à map[] se id não foi adicionado anteriormente
        map[id] = current_heap_size;
      }

      for(int i = h.size()/2; i > 0; i--) Sink(i);
    }

    // Insere cerficado (id, t) no heap. Pressupõe que os ids dos certificados são 2, 3, ..., 2 + num_certificados - 1
    void Insert(const int id, const double t)
    {
      h.push_back(std::make_pair(id, t));
      int current_heap_size = h.size() - 1;
      if(id > current_heap_size) map.push_back(1); // Só adiciona mais uma posição à map[] se id não foi adicionado anteriormente
      map[id] = current_heap_size;
      Swim(current_heap_size);
    }

    // Remove certificado com menor prioridade do heap
    void DeleteMin()
    {
      int cert_id = h[1].first;
      Swap(1, h.size() - 1);  // Substitui raiz pelo último elemento
      h.pop_back();
      Sink(1);
      map[cert_id] = -1;
    }

    // Retorna certificado (id, t) com menor t 
    cert Min() { return h[1]; }

    // Atualiza prioridade do certificado id para new_t
    void Update(const int id, const double new_t)
    {
      h[map[id]].second = new_t;
      Sink(map[id]);
      Swim(map[id]);
    }

    // Imprime representação de árvore do heap, junto com a representação de vetor
    void Print()
    {
      //std::cout << "Representação de árvore: \n";
      //_PrintTree(0, 1); 
      //std::cout << "Representação de vetor: \n";
      for(int i = 1; i <= h.size() - 1; i++) std::cout << "(" << h[i].first << "," << (h[i].second == INFTY ? "inf" : std::to_string(h[i].second)) << "), ";
      std::cout << "\n";

      //std::cout << "Mapeamento: \n";
      //for(int i = 2; i <= map.size() - 1; i++) std::cout << "map[" << i << "] = " << map[i] << "\n";
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
      if(i/2 < 1) return;

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
      map[h[i].first] = j;
      map[h[j].first] = i;

      cert tmp = h[i];
      h[i] = h[j];
      h[j] = tmp;
    }

    // Desce o elemento na posição i do vetor h para a sua posição correta no heap
    void Sink(int i)
    {
      DEBUG("Sink(");
      DEBUG(i);
      DEBUG(")\n");
      int current_heap_size = h.size() - 1;
      DEBUG(h[i].second);
      DEBUG("\n");
      if(2*i > current_heap_size) {
        DEBUG("folha ou NULL: acabou\n");
        return; // Folha ou NULL
      }
      else if (2*i + 1 > current_heap_size) // Só tem filho esquerdo
      {
        if(h[2*i].second < h[i].second)
        {
          DEBUG("Tem filho esquerdo menor\n");
          Swap(i, 2*i);
          return;
        }
      }
      else // Tem os dois filhos
      {
        cert u = h[i];
        cert left_child = h[2*i];
        cert right_child = h[2*i + 1];

        if(left_child.second < u.second && left_child.second < right_child.second)
        {
          DEBUG("Tem filho esquerdo mínimo\n");
          Swap(i, 2*i);
          Sink(2*i);
        }

        if(right_child.second < u.second && right_child.second < left_child.second)
        {
          DEBUG("Tem filho direito mínimo\n");
          Swap(i, 2*i + 1);
          Sink(2*i + 1);
        }
      }
    }
    
    std::vector<cert> h; // vetor que representa árvore do heap
    std::vector<int> map; // mapeamento id -> índice em h

};

//TEST(Heap, Teste1)
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
//
//TEST (Heap, Teste2)
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
//}
//
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
//
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
//
//// Criado heap a partir de vetor
//TEST(Heap, Teste6)
//{
//  std::vector<cert> v;
//  v.push_back(std::make_pair(2,27));
//  v.push_back(std::make_pair(3,17));
//  v.push_back(std::make_pair(4,18));
//  v.push_back(std::make_pair(5,19));
//  v.push_back(std::make_pair(6,11));
//  v.push_back(std::make_pair(7,5));
//  v.push_back(std::make_pair(8,9));
//  v.push_back(std::make_pair(9,33));
//  v.push_back(std::make_pair(10,14));
//  v.push_back(std::make_pair(11,21));
//  Heap h = Heap(v);
//  h.Print();
//}

//int main(int argc, char** argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//  }
