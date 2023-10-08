// Tarefa t5 - Heap cinético 
// Paulo Henrique Albuquerque
// 12542251

#include "heap.cpp"
#include <map>

#define INFTY 123456

class KinHeap {
  public:
    KinHeap(std::vector<int> id, std::vector<int> x0, std::vector<int> speed, int n)
    {
      H = std::vector<int>(); H.push_back(-1);
      Q = new Heap();

      BuildKinHeap(id, x0, speed, n);
      BuildCertificates();
    }

    void Advance(const int t)
    {
      while(Q->Min().second <= t)
      {
        now = Q->Min().second;
        Event(Q->Min());
      }
      now = t;
    }

    void Change(const int id, const int v)
    {
      int x0 = std::get<0>(m[id]);
      int v0 = std::get<1>(m[id]);
      int i = std::get<2>(m[id]);
      int new_x0 = x0 + now * (v0 - v); 

      m[id] = std::make_tuple(new_x0, v, i);

      if(i > 1) UpdateCert(i);
      if(2*i < H.size()) UpdateCert(2*i);
      if(2*i + 1 < H.size()) UpdateCert(2*i + 1);
    }

    void Insert(const int id, const int xnow, const int v)
    {

    }

    int Max()
    {

    }

    void DeleteMax()
    {

    }

    void DeleteId(const int id)
    {

    }

    void Print()
    {

    }

  private:
    
    // Atualiza o certificado i
    void UpdateCert(int i)
    {
      int u = H[i];
      int u_parent = H[i/2];

      double x_u = std::get<0>(m[u]);
      double v_u = std::get<1>(m[u]);
      double x_u_parent = std::get<0>(m[u_parent]);
      double v_u_parent = std::get<1>(m[u_parent]);

      double t;
      if(v_u == v_u_parent) t = INFTY;
      else t = (x_u - x_u_parent) / (v_u_parent - v_u);

      Q->Update(i, t);
    }

    // Certificado (i, t) expirou
    void Event(int i, int t)
    {
      Swap(i, i/2);
      
      if(i > 1) UpdateCert(i/2);
      UpdateCert(i);
      if(2*i < H.size()) UpdateCert(2*i);
      if(2*i + 1 < H.size()) UpdateCert(2*i + 1);
      int s = Sibling(i);
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
    void Swim(int i)
    {
      if(i/2 < 1) return;

      int u_id = H[i];
      int u_parent_id = H[i/2];

      int u_key = Key(u_id, now);
      int u_parent_key = Key(u_parent_id, now);
      
      if(u_key > u_parent_key) 
      { 
        Swap(i, i/2); 
        Swim(i/2);
      }
    }

    // Dado um id, calcula a chave no tempo t
    int Key(int id, int t)
    {
      int x0 = std::get<0>(m[id]);
      int v = std::get<1>(m[id]);
      return x0 + v*t;
    }

    
    // Troca a posição dos elemetos nas posições i e j
    void Swap(const int i, const int j)
    {
      int tmp = H[i];
      H[i] = H[j];
      H[j] = tmp;
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
   
    std::vector<int> H; // Vetor que representa o heap
    Heap* Q; // MinHeap de certificados
    std::map<int, std::tuple<double, double, int>> m; // Mapeia ids para posição inicial e velocidade e posição no vetor H
    int now; // Instante atual

};


