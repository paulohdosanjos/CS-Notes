// Tarefa t3 - Fila retroativa
// Paulo Henrique Albuquerque
// 12542251

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "abb.cpp"

class RetroQueue {

  public:
    
    RetroQueue() {enq = Abb(); deq = Abb();}

    void InsertEnqueue(const int t, const int x) { enq.Insert(t, x); }

    void InsertDequeue(const int t) { deq.Insert(t, GARBAGE); }

    void Delete(const int t) { enq.Delete(t); deq.Delete(t); }

    int K(const int t, const int k)
    {
      int l = deq.Count(t);
      return enq.K(l+k);
    }

    int First(const int t) { return K(t, 1); }

    void Print()
    {
      int size = enq.Count(INFTY) - deq.Count(INFTY); // Tamanho da fila atual
      //std::cout << "size " << size << "\n"; 
      for(int i = 1; i <= size; i++) std::cout << K(INFTY, i) << " ";
      std::cout << "\n";
    } 

  private:
    Abb enq; 
    Abb deq; 
 
};

// Teste dos slides. Parece OK 
void Teste1()
{
  RetroQueue q = RetroQueue();
  q.InsertEnqueue(10, 1); 
  q.Print(); // 1
  q.InsertEnqueue(3, 2); 
  q.Print(); // 2 1
  q.InsertEnqueue(5, 3);
  q.Print(); // 2 3 1
  q.InsertDequeue(7);
  q.Print(); // 3 1
  q.Delete(7);
  q.Print(); // 2 3 1
}

// Depura Delete com chave inexistente
void Teste21()
{
  RetroQueue q = RetroQueue();
  q.InsertEnqueue(10, 1); 
  q.Print();
  q.Delete(10);
  q.Print();

}

// Cliente teste da tarefa
int main()
{
  Teste1();
  //Teste21();
}
