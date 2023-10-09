// Tarefa t5 - Heap cinético 
// Paulo Henrique Albuquerque
// 12542251

#include "heap.cpp"
#include <map>


#define DEBUG_MODE_2 0
#if DEBUG_MODE_2
#define DEBUG2(x) std::cout << x
#else
#define DEBUG2(x) 
#endif

class KinHeap {
  public:

    // Constrói KinHeap vazio
    KinHeap()
    {
      now = 0;
      m = std::map<int, std::tuple<double, double, int>>();
      H = std::vector<int>(); H.push_back(-1); // Não utilizo primeira posição
      Q = new Heap();
    }

    // Constrói KinHeap a partir de valores de entrada
    KinHeap(int id[], double x0[], double v[], int n)
    {
      now = 0;
      m = std::map<int, std::tuple<double, double, int>>();
      BuildHeap(id, x0, v, n);
      BuildQ();
    }

    // Insere o elemento id, cujo valor atual é xnow com velocidade v
    void Insert(int id, double xnow, double v)
    {
      // Adiciona elemento id no MaxHeap H
      H.push_back(id);
      int i = H.size() - 1;
      double new_x0 = xnow - v * now; 
      m[id] = std::make_tuple(new_x0, v, i);

      if(i == 1) return; // Raiz

      int parent_id = H[i/2];
      if(Key(id, now) > Key(parent_id, now)) Q->Insert(i, now);
      else Q->Insert(i, Validade(i));

      while(Q->Min().second == now)
      {
        Event(Q->Min().first, Q->Min().second);
        
        int i = std::get<2>(m[id]);

        if(i / 2 == 0) break;

        int parent_id = H[i/2];
        if(Key(id, now) > Key(parent_id, now)) Q->Update(i, now);
        else Q->Update(i, Validade(i));
      }

    }

    void Advance(const double t)
    {
      int num_it = 0;
      while(Q->Min().second <= t)
      {
        now = Q->Min().second;
        DEBUG2("now = "); DEBUG2(now); DEBUG2("\n");
        Event(Q->Min().first, Q->Min().second);
        DEBUG2("Depois do evento:\n");

        // ****** PARA DEPURAÇÃO
        //Print();
        //Q->Print();

        num_it++;
        //if(num_it == 1) break;
      }
      now = t;
    }

    void Change(const int id, const double v)
    {
      double x0 = std::get<0>(m[id]);
      double v0 = std::get<1>(m[id]);
      int i = std::get<2>(m[id]);
      double new_x0 = x0 + now * (v0 - v); 

      m[id] = std::make_tuple(new_x0, v, i);

      if(i > 1) UpdateCert(i);
      if(2*i < H.size()) UpdateCert(2*i);
      if(2*i + 1 < H.size()) UpdateCert(2*i + 1);
    }

    // Retorna id do elemento com maior valor
    int Max() { return H[1]; }

     
    // Imprime representação de árvore do heap, junto com a representação de vetor
    void Print()
    {
      //std::cout << "Representação de árvore: \n";
      _PrintTree(0, 1); 

      // ****** PARA DEPURAÇÃO ******
      //std::cout << "H: \n";
      //for(int i = 1; i <= H.size() - 1; i++) std::cout << H[i] << " ";
      //std::cout << "\n";

      //std::cout << "Mapeamento: \n";
      //for(int i = 2; i <= map.size() - 1; i++) std::cout << "map[" << i << "] = " << map[i] << "\n";
    }

    // Remove elemento id do heap
    void Delete(int id)
    {
      int i = std::get<2>(m[id]);
      Swap(i, H.size() - 1);
      H.pop_back();

      id = H[i];
      f(i);
      
      while(Q->Min().second == now)
      {
        Event(Q->Min().first, Q->Min().second);
        
        int i = std::get<2>(m[id]);

        f(i);

      }
    }

    // Remove o elemento de maior valor do heap
    void DeleteMax() 
    { 
      Swap(1, H.size() - 1);
      H.pop_back();
      
      int id = H[1];
      f(1);

      while(Q->Min().second == now)
      {
        Event(Q->Min().first, Q->Min().second);
        
        int i = std::get<2>(m[id]);

        f(i);

      }
    }

    // Verifica se algum filho tem valor maior que o elemento em i. Caso tenha, produz um certificado para now
    void f(int i)
    {
      if(2*i > H.size() - 1) // Não tem filho
      {
        return;
      }
      else if(2*i + 1 > H.size() - 1) // Só tem filho esquerdo
      {
        int id = H[i];
        int child_id = H[2*i];
        if(Key(id, now) < Key(child_id, now)) Q->Update(2*i, now);
        else Q->Update(2*i, Validade(2*i));
      }
      else // Tem os dois filhos 
      {
        int id = H[i];
        int left_child_id = H[2*i];
        int right_child_id = H[2*i + 1];

        if(Key(left_child_id, now) > Key(id, now) && Key(left_child_id, now) > Key(right_child_id, now))
        {
          Q->Update(2*i, now);
        }
        else Q->Update(2*i, Validade(2*i));
        if(Key(right_child_id, now) > Key(id, now) && Key(right_child_id, now) > Key(left_child_id, now))
        {
          Q->Update(2*i + 1, now);
        }
        else Q->Update(2*i + 1, Validade(2*i + 1));
      }
    }

    //// Remove o elemento id
    //void Delete(int id)
    //{
    //    
    //}

     //private:

    void _PrintTree(int s, int i)
    {
      if(i > H.size() - 1) return;

      _PrintTree(s + 3, 2*i);

      std::string space (s, ' ');
      std::cout << space;

      int id = H[i];
      double xnow = Key(id, now);
      double x0 = std::get<0>(m[id]);
      double v = std::get<1>(m[id]);
      std::cout << xnow << " (" << x0 << " " << v << ")\n";
      _PrintTree(s + 3, 2*i + 1);
    }


    // Retorna prazo de validade para certificado i
    double Validade(int i)
    {
      int u = H[i];
      int u_parent = H[i/2];

      double x_u = std::get<0>(m[u]);
      double v_u = std::get<1>(m[u]);
      double x_u_parent = std::get<0>(m[u_parent]);
      double v_u_parent = std::get<1>(m[u_parent]);

      if(v_u == v_u_parent) return INFTY;
      //DEBUG2(v_u_parent); DEBUG2("\n");
      //DEBUG2(v_u); DEBUG2("\n");
      //DEBUG2(x_u-x_u_parent); DEBUG2("\n");
      //DEBUG2(v_u_parent - v_u); DEBUG2("\n");
      double t = (x_u - x_u_parent) / (v_u_parent - v_u);
      //DEBUG2("t = "); DEBUG2(t); DEBUG2("\n");
      if(t < 0) return INFTY;
      else return t;
    }
    
    // Constrói heap cinético a partir da entrada 
    void BuildHeap(int _id[], double _x0[], double _v[], int n)
    {
      H.push_back(-1); // Não utilizo primeira posição
    
      // Copia para H
      for(int i = 0; i < n; i++)
      {
        int id = _id[i];
        double x0 = _x0[i];
        double v = _v[i];

        H.push_back(id);
        std::get<0>(m[id]) = x0;
        std::get<1>(m[id]) = v;
        std::get<2>(m[id]) = H.size() - 1;
      }
      
      // Restaura propriedade de heap
      for(int i = H.size()/2; i > 0; i--) Sink(i);
    }

    // Constrói MinHeap Q de certificados
    void BuildQ()
    {
      Q = new Heap();
      int num_cert = H.size() - 2;
      for(int cert_id = 2; cert_id <= 2 + num_cert - 1; cert_id++) // num_cert = num_elementos - 1 = H.size() - 1 - 1 = H.size() - 2
      {
        Q->Insert(cert_id, Validade(cert_id));
      }
    }

    // Atualiza o certificado i
    void UpdateCert(int i)
    {
      DEBUG2("Update("); DEBUG2(i); DEBUG2(")\n");
      if(i == 1) return;
      int u = H[i];
      int u_parent = H[i/2];

      double x_u = std::get<0>(m[u]);
      double v_u = std::get<1>(m[u]);
      double x_u_parent = std::get<0>(m[u_parent]);
      double v_u_parent = std::get<1>(m[u_parent]);
      
      double t;
      if(v_u == v_u_parent) t = INFTY;
      t = (x_u - x_u_parent) / (v_u_parent - v_u);
      if(t <= now) t = INFTY;
      DEBUG2("t = "); DEBUG2(t); DEBUG2("\n");
      Q->Update(i, t);
    }

    // Certificado (i, t) expirou
    void Event(int i, double t)
    {
      DEBUG2("Event("); DEBUG2(i); DEBUG2(", "); DEBUG2(t); DEBUG2(")\n");
      int id = H[i];
      int parent_id = H[i/2];
      DEBUG2("id = "); DEBUG2(id); DEBUG2(", parent_id = "); DEBUG2(parent_id); DEBUG2("\n");

      Swap(i, i/2);
      
      if(i > 1) UpdateCert(i/2);
      UpdateCert(i);
      if(2*i < H.size()) UpdateCert(2*i);
      if(2*i + 1 < H.size()) UpdateCert(2*i + 1);
      int s = Sibling(i);
      DEBUG2("sibling = "); DEBUG2(s); DEBUG2("\n");
      if(s > 0) UpdateCert(s);
    }

    // Retorna índice do irmão do elemento na posição i de H. Retorna 0 se chamado para a raiz ou para uma folha sem irmão
    int Sibling(int i)
    {
      if(i == 1) return 0; // Raiz não tem irmão

      if(i % 2 == 0) // Elemento na posição i é filho esquerdo
      {
        if(i + 1 < H.size()) return i+1; // Tem irmão
        else return 0; // Não tem irmão
      }
      else // Elemento na posição i é filho direito
      {
        return i-1;
      }

    }

    // Sobe elemento na posição i do vetor H para a sua posição correta no heap
    //void Swim(int i)
    //{
    //  if(i/2 < 1) return;

    //  int u_id = H[i];
    //  int u_parent_id = H[i/2];

    //  int u_key = Key(u_id, now);
    //  int u_parent_key = Key(u_parent_id, now);
    //  
    //  if(u_key > u_parent_key) 
    //  { 
    //    Swap(i, i/2); 
    //    Swim(i/2);
    //  }
    //}

    // Dado um id de um elemento, calcula a chave no tempo t
    double Key(int id, double t)
    {
      double x0 = std::get<0>(m[id]);
      double v = std::get<1>(m[id]);
      return x0 + v*t;
    }

    
    // Troca de posição os elemetos nos índices i e j em H
    void Swap(const int i, const int j)
    {
      // Atualiza mapeamento de índice
      int i_id = H[i]; std::get<2>(m[i_id]) = j;
      int j_id = H[j]; std::get<2>(m[j_id]) = i;

      H[i] = j_id;
      H[j] = i_id;
    }

    // Desce o elemento na posição i do vetor H para a sua posição correta no heap cinético
    void Sink(int i)
    {
      int current_heap_size = H.size() - 1;
      if(2*i > current_heap_size) {
        return; // Folha ou NULL
      }
      else if (2*i + 1 > current_heap_size) // Só tem filho esquerdo
      {
        if(Key(H[i], now) < Key(H[2*i], now))
        {
          Swap(i, 2*i);
          return;
        }
      }
      else // Tem os dois filhos
      {
        int u = H[i];
        int left_child = H[2*i];
        int right_child = H[2*i + 1];

        if(Key(left_child, now) > Key(u, now) && Key(left_child, now) > Key(right_child, now))
        {
          Swap(i, 2*i);
          Sink(2*i);
        }

        if(Key(right_child, now) > Key(u, now) && Key(right_child, now) > Key(left_child, now))
        {
          Swap(i, 2*i + 1);
          Sink(2*i + 1);
        }
      }
    }
   
    std::vector<int> H; // Vetor que representa o heap. Guarda os ids dos elementos
    Heap* Q; // MinHeap de certificados
    std::map<int, std::tuple<double, double, int>> m; // Mapeia ids dos elementos para posição inicial, velocidade e posição no vetor H
    double now; // Instante atual
};

// Teste inicial para construção do KinHeap vazio e inserção de alguns elementos. Parece OK
void Teste1()
{
  KinHeap kh = KinHeap();
  kh.Insert(1, 17, -1);
  kh.Insert(2, 10, 1);
  kh.Insert(3, 3, 5);
  kh.Insert(4, 0, 2.1);
  kh.Insert(5, 3, 0);

  std::cout << "******************************\n";
  std::cout << "Inicialmente:\n";
  kh.Print(); 
  std::cout << "certificados: \n";
  kh.Q->Print();
  //std::cout << "(" << kh.Q->Min().first << ", " << kh.Q->Min().second << ")\n";
  std::cout << "Max = " << kh.Max() << "\n";

  int t = 10;
  std::cout << "******************************\n";
  std::cout << "avançando para t = " << t << ":\n";
  kh.Advance(t);

  kh.Print(); 
  std::cout << "certificados: \n";
  kh.Q->Print();
  std::cout << "Max = " << kh.Max() << "\n";
}

// Testa construir Heap a partir dos elementos iniciais. Parece OK
void Teste2()
{
  int id[] = {2, 5, 1, 3, 4};
  double x0[] = {2.2, -7.4, 0, 1.1, 12.0};
  double v[] = {-1.2, 1.1, 5.5, 2.1, -0.1};
  int n = (int) (sizeof(id)/sizeof(id[0]));
  KinHeap kh = KinHeap(id, x0, v, n);

  kh.Print();
  std::cout << "certificados: \n";
  kh.Q->Print();
  std::cout << "Max = " << kh.Max() << "\n";

  std::cout << "***********************\n";
  int t = 3;
  std::cout << "Avançando até t = " << t << "\n";
  kh.Advance(t);
  kh.Print();
  std::cout << "certificados: \n";
  kh.Q->Print();
  std::cout << "Max = " << kh.Max() << "\n";
}

// Cliente principal
void Teste()
{
  int n; std::cin >> n;
  int id[n];
  double x0[n];
  double v[n];
  for(int i = 0; i < n; i++) {
    std::cin >> x0[i] >> v[i];
    id[i] = i + 1;
  }

  KinHeap kh = KinHeap(id, x0, v, n);

  for(std::string line; std::getline(std::cin, line) ; ) {
    int opt; double a, b, c; 
    std::istringstream iss = std::istringstream(line);
    iss >> opt >> a >> b >> c;
    switch (opt) {
      case 1:
        kh.Advance(a);
        break;
      case 2:
        kh.Change(a, b);
        break;
      case 3:
        kh.Insert(a, b, c);
        break;
      case 4:
        std::cout << kh.Max() << "\n";
        break;
      case 5:
        kh.DeleteMax();
        break;
      case 6:
        kh.Delete(a);
        break;
      case 7:
        kh.Print();
        break;
    }
  }
}

// Loop infinito na entrada de teste 
void Teste3()
{
  int id[] = {1, 2, 3, 4, 5};
  double x0[] = {8, 3, 17, 16, 5};
  double v[] = {5, 4, -1, 1, 0};
  int n = (int) (sizeof(id)/sizeof(id[0]));
  KinHeap kh = KinHeap(id, x0, v, n);

  //kh.Print();
  //kh.Q->Print();
  kh.Advance(4);
  kh.Print();
}

// Desempate. Os três elementos colidem em t = 1.5. Não consegui quebrar ainda
void Teste4()
{
  int id[] = {1, 2, 3};
  double x0[] = {8, 17, -2.65};
  double v[] = {5, -1, 12.1};
  int n = (int) (sizeof(id)/sizeof(id[0]));
  KinHeap kh = KinHeap(id, x0, v, n);

  kh.Print();
  kh.Q->Print();
  std::cout << "Max = " << kh.Max() << "\n";

  kh.Advance(4);
  kh.Print();
  std::cout << "Max = " << kh.Max() << "\n";
}

void Teste5()
{
  int id[] = {1, 2, 3, 4, 5};
  double x0[] = {8, 3, 17, 16, 5};
  double v[] = {5, 4, -1, 1, 0};
  int n = (int) (sizeof(id)/sizeof(id[0]));
  KinHeap kh = KinHeap(id, x0, v, n);

  kh.Print();

  kh.DeleteMax();
  kh.Print();

  kh.DeleteMax();
  kh.Print();

  kh.DeleteMax();
  kh.Print();

  kh.DeleteMax();
  kh.Print();
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

//int main(int argc, char** argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}

