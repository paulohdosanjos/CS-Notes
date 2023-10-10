// Tarefa t5 - Heap cinético 
// Paulo Henrique Albuquerque
// 12542251

#include "limpo-heap.cpp"

class KinHeap {
  public:

    // Constrói KinHeap vazio
    KinHeap ()
    {
      now = 0;
      M = std::map<int, std::tuple<double, double, int>>();
      H = std::vector<int>(); H.push_back(-1); // Não utilizo primeira posição
      Q = new Heap();
    }

    // Constrói KinHeap a partir de valores de entrada
    KinHeap (const int id[], const double x0[], const double v[], const int n)
    {
      now = 0;
      M = std::map<int, std::tuple<double, double, int>>();
      Build_H(id, x0, v, n); // Constrói H
      Build_Q(); // Constrói Q
    }

    // Insere o elemento id, cujo valor atual é xnow com velocidade v
    void Insert (const int id, const double xnow, const double v)
    {
      // Adiciona elemento id no MaxHeap H
      H.push_back(id);
      int i = H.size() - 1;
      double new_x0 = xnow - v * now; // Valor do elemento em t = 0 
      M[id] = std::make_tuple(new_x0, v, i); // Adiciona nova entrada na tabela de símbolos

      if(i / 2  == 0) return; // É raiz, nada mais a fazer 

      Make_Parent_Certificate(i, true); 

      /* Sobe no heap, atualizando certificados afetados. 
       Caso o elemento esteja fora de sua posição, um certificado correspondente é gerado com validade t = now, 
       e a troca é feita agora */
      while(Q->Min().second == now) // Enquanto o elemento está fora de posição
      {
        Event(Q->Min());
        int i = std::get<2>(M[id]);

        if(i / 2 == 0) break;

        Make_Parent_Certificate(i, false);
      }
    }

    // Avança o tempo para o instante t
    void Advance (const double t)
    {
      while(Q->Min().second <= t)
      {
        now = Q->Min().second;
        Event(Q->Min());
      }
      now = t;
    }

    // Muda a velocidade do elemento id para v
    void Change (const int id, const double v)
    {
      // Atualiza tabela de símbolos
      double x0 = std::get<0>(M[id]);
      double v0 = std::get<1>(M[id]);
      int i     = std::get<2>(M[id]);

      double new_x0 = x0 + now * (v0 - v); 
      M[id] = std::make_tuple(new_x0, v, i);

      if(i > 1) Update_Certificate(i);
      if(2*i < H.size()) Update_Certificate(2*i);
      if(2*i + 1 < H.size()) Update_Certificate(2*i + 1);
    }

    // Retorna id do elemento com maior valor no instante atual
    int Max () const { return H[1]; }

    // Imprime representação de árvore do heap 
    void Print () { _PrintTree(0, 1); }

    // Remove elemento id do heap
    void Delete (const int id)
    {
      int i = std::get<2>(M[id]);
      Swap(i, H.size() - 1);
      H.pop_back();

      int _id = H[i];
      Make_Child_Certificate(i);
      
      while(Q->Min().second == now)
      {
        Event(Q->Min());
        
        int i = std::get<2>(M[_id]);

        Make_Child_Certificate(i);

      }
    }

    // Remove o elemento de maior valor do heap
    void DeleteMax () { Delete(H[1]); }

  //private:

    /* Verifica se algum filho tem valor maior que o elemento em i. 
     Caso tenha, gera um certificado com validade t = now para que a troca aconteça. 
     Caso contrário, gera certificado normal */
    void Make_Child_Certificate (const int i)
    {
      if(2*i > H.size() - 1) // Não tem filho
      {
        return;
      }
      else if(2*i + 1 > H.size() - 1) // Só tem filho esquerdo
      {
        int id = H[i];
        int left_child_id = H[2*i];
        if(Key(id, now) < Key(left_child_id, now)) Q->Update(2*i, now);
        else Q->Update(2*i, Failure_Time(2*i));
      }
      else // Tem os dois filhos 
      {
        int id = H[i];
        int left_child_id = H[2*i];
        int right_child_id = H[2*i + 1];

        if(Key(left_child_id, now) > Key(id, now) &&
           Key(left_child_id, now) > Key(right_child_id, now)) // Filho esquerdo tem maior valor
             { Q->Update(2*i, now); }
        else { Q->Update(2*i, Failure_Time(2*i)); }

        if(Key(right_child_id, now) > Key(id, now) &&
           Key(right_child_id, now) > Key(left_child_id, now)) // Filho direito tem maior valor
             { Q->Update(2*i + 1, now); }
        else { Q->Update(2*i + 1, Failure_Time(2*i + 1)); }
      }
    }


    /* Verifica se o valor do elemento em i é maior do que o pai. 
     Caso seja, produz um certificado com validade t = now para que a troca aconteça. 
     Caso contrário, gera um certificado normal
     insert = true insere em Q, e insert = false só atualiza
     */
     
    void Make_Parent_Certificate (const int i, bool insert)
    {
      if(i / 2 == 0) return; // Raiz. Nada a fazer
      int id = H[i];
      int parent_id = H[i/2];

      if(Key(id, now) > Key(parent_id, now))
      {
        if(insert) Q->Insert(i, now);
        else Q->Update(i, now);
      }
      else
      {
        if(insert) Q->Insert(i, Failure_Time(i));
        else Q->Update(i, Failure_Time(i));
      }
    }

    void _PrintTree (const int s, const int i) 
    {
      if(i > H.size() - 1) return;

      _PrintTree(s + 3, 2*i);

      std::string space (s, ' ');
      std::cout << space;

      int id = H[i];
      double xnow = Key(id, now);
      double x0 = std::get<0>(M[id]);
      double v  = std::get<1>(M[id]);

      std::cout << xnow << " (" << x0 << " " << v << ")\n";
      _PrintTree(s + 3, 2*i + 1);
    }

    // Retorna prazo de validade para certificado i
    double Failure_Time (const int i) 
    {
      int u = H[i];
      int u_parent = H[i/2];

      double x_u        = std::get<0>(M[u]);
      double v_u        = std::get<1>(M[u]);
      double x_u_parent = std::get<0>(M[u_parent]);
      double v_u_parent = std::get<1>(M[u_parent]);

      if(v_u == v_u_parent) return INFTY;

      double t = (x_u - x_u_parent) / (v_u_parent - v_u);
      if(t == now && v_u_parent > v_u) t = INFTY; // Já foram trocados
      if(t < now) return INFTY;
      else return t;
    }
    
    // Constrói heap cinético a partir de valores de entrada
    void Build_H (const int _id[], const double _x0[], const double _v[], const int n)
    {
      H.push_back(-1); // Não utilizo primeira posição
    
      // Copia ids para H
      for(int i = 0; i < n; i++)
      {
        int id = _id[i];
        double x0 = _x0[i];
        double v = _v[i];

        H.push_back(id);

        // Atualiza tabela de símbolos
        std::get<0>(M[id]) = x0;
        std::get<1>(M[id]) = v;
        std::get<2>(M[id]) = H.size() - 1;
      }
      
      // Restaura propriedade de heap
      for(int i = H.size()/2; i > 0; i--) Sink(i);
    }

    // Constrói MinHeap Q de certificados
    void Build_Q ()
    {
      Q = new Heap();
      int num_cert = H.size() - 2; // num_cert = num_elementos - 1 = H.size() - 1 - 1 = H.size() - 2

      for(int cert_id = 2; cert_id <= 2 + num_cert - 1; cert_id++) 
        Q->Insert(cert_id, Failure_Time(cert_id));
    }

    // Atualiza o certificado i
    void Update_Certificate (const int i)
    {
      if(i / 2 == 0) return; // Raiz. Nada a se fazer

      double t = Failure_Time(i);

      Q->Update(i, t); // Atualiza certificado no heap de certificados Q
    }

    // Certificado (i, t) expirado. Troca posição dos elementos em questão e atualiza certificados afetados
    void Event (const cert c)
    {
      int i = c.first;
      double t = c.second;
      int id = H[i];
      int parent_id = H[i/2];

      Swap(i, i/2);
      
      if(i > 1) Update_Certificate(i/2);

      Update_Certificate(i);

      if(2*i < H.size()) Update_Certificate(2*i);

      if(2*i + 1 < H.size()) Update_Certificate(2*i + 1);

      int s = Sibling(i);
      if(s > 0) Update_Certificate(s);
    }

    // Retorna posíção do irmão do elemento na posição i de H. Retorna 0 se chamado para a raiz ou para uma folha sem irmão
    int Sibling (const int i) const
    {
      if(i / 2 == 0) return 0; // Raiz. Não tem irmão

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

    // Dado um id de um elemento, calcula seu valor no tempo t
    double Key (const int id, const double t) 
    {
      double x0 = std::get<0>(M[id]);
      double v  = std::get<1>(M[id]);
      return x0 + v*t;
    }

    // Troca de posição os elemetos nas posições i e j de H
    void Swap (const int i, const int j)
    {
      // Atualiza mapeamento de posição na tabela de símbolos
      int i_id = H[i]; std::get<2>(M[i_id]) = j;
      int j_id = H[j]; std::get<2>(M[j_id]) = i;

      H[i] = j_id;
      H[j] = i_id;
    }

    // Desce o elemento na posição i do vetor H para a sua posição correta no heap cinético
    void Sink (const int i)
    {
      if(2*i > H.size() - 1) { // Não tem filho
        return; 
      }
      else if (2*i + 1 > H.size() - 1) // Só tem filho esquerdo
      {
        int id = H[i];
        int left_child_id = H[2*i];
        if(Key(id, now) < Key(left_child_id, now))
        {
          Swap(i, 2*i);
          return;
        }
      }
      else // Tem os dois filhos
      {
        int id = H[i];
        int left_child_id = H[2*i];
        int right_child_id = H[2*i + 1];

        // Filho esquerdo tem maior valor
        if(Key(left_child_id, now) > Key(id, now) && 
           Key(left_child_id, now) > Key(right_child_id, now))
        {
          Swap(i, 2*i);
          Sink(2*i);
        }

        // Filho direito tem maior valor
        if(Key(right_child_id, now) > Key(id, now) && 
           Key(right_child_id, now) > Key(left_child_id, now))
        {
          Swap(i, 2*i + 1);
          Sink(2*i + 1);
        }
      }
    }
   
    std::vector<int> H; // Vetor que representa o heap. Guarda os ids dos elementos

    Heap* Q; // MinHeap de certificados

    std::map<int, std::tuple<double, double, int>> M; // Mapeia ids dos elementos para posição inicial, velocidade e posição no vetor H

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
  //kh.Q->Print();
  //std::cout << "(" << kh.Q->Min().first << ", " << kh.Q->Min().second << ")\n";
  std::cout << "Max = " << kh.Max() << "\n";

  int t = 10;
  std::cout << "******************************\n";
  std::cout << "avançando para t = " << t << ":\n";
  kh.Advance(t);

  kh.Print(); 
  std::cout << "certificados: \n";
  //kh.Q->Print();
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
  //kh.Q->Print();
  std::cout << "Max = " << kh.Max() << "\n";

  std::cout << "***********************\n";
  int t = 3;
  std::cout << "Avançando até t = " << t << "\n";
  kh.Advance(t);
  kh.Print();
  std::cout << "certificados: \n";
  //kh.Q->Print();
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

// Desempate. Os três elementos colidem em t = 1. Parece OK
void Teste4()
{
  int id[] = {1, 2, 3};
  double x0[] = {8, 9, 10};
  double v[] = {2, 1, 0};
  int n = (int) (sizeof(id)/sizeof(id[0]));
  KinHeap kh = KinHeap(id, x0, v, n);

  std::cout << "Antes da colisão:\n";
  kh.Print();
  kh.Q->Print();
  std::cout << "Max = " << kh.Max() << "\n";
  std::cout << "********************************************\n";

  std::cout << "Depois da colisão:\n";
  kh.Advance(2);
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
  Teste4();
  //Teste5();
  //Teste();
}

