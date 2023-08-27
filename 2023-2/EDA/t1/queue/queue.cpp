#include <iostream>
#include <string>
#include "queue.h"
#include "../stack_skew/stack_skew.h"


template<class T>
Queue<T> :: Queue() {
  s = Stack<T>();
  number_of_removed_elements = 0;
}

template<class T>
Queue<T> :: Queue(Stack<T> stack, unsigned int x) {
  s = stack;
  //std::cout << s.Show();
  number_of_removed_elements = x;
}

template<class T>
std::string Queue<T> :: ShowStack() {
  return s.Show();
}

template<class T>
Queue<T> Queue<T> :: Enqueue(const T x) {
  return Queue(s.Push(x), number_of_removed_elements);
}

template<class T>
Queue<T> Queue<T> :: Dequeue() {
  return Queue(s, number_of_removed_elements + 1);
}

template<class T>
unsigned int Queue<T> :: Size() {
  return s.Size() - number_of_removed_elements;
}

template<class T>
bool Queue<T> :: IsEmpty() {
  return Size() == 0;
}

template<class T>
T Queue<T> :: First() {
  return K(1);
}

template<class T>
T Queue<T> :: K(unsigned int k) {
  return s.K(Size() - (k-1)); // Para o primeiro elemento (k = 1), o argumento deve ser jumps_to_first 
}

// Supõe char
template<class T>
std::string Queue<T> :: Show() {
  unsigned int size = Size();
  Queue<T> q = Queue<T>(s,number_of_removed_elements);


  std::string output = std::string();
  for(int i = 0 ; i < size ; i++){
    output = output + q.First() + " ";
    q = q.Dequeue();
  }
  output = output + "\n";

  return output;
}

typedef Queue<char> fila;

void Teste1() {
  fila f0 = fila();
  std::cout << "f0: "; //f0: 
  std::cout << f0.Show();

  fila f1 = f0.Enqueue('a'); //f1: a
  std::cout << "f1: ";
  std::cout << f1.Show();

  fila f2 = f1.Enqueue('b'); //f2: a b
  std::cout << "f2: ";
  std::cout << f2.Show();

  fila f3 = f2.Enqueue('c'); //f3: a b c 
  std::cout << "f3: ";
  std::cout << f3.Show();

  fila f4 = f3.Dequeue();
  std::cout << "f4: "; //f4: b c
  std::cout << f4.Show();

  fila f5 = f4.Dequeue(); //f5: c
  std::cout << "f5: ";
  std::cout << f5.Show();

  fila f6 = f5.Dequeue(); //f6: 
  std::cout << "f6: ";
  std::cout << f6.Show();

  fila f7 = f3.Enqueue('d'); //f7: a b c d 
  std::cout << "f7: ";
  std::cout << f7.Show();

}

void Teste2() {

  // Testa K-th()
  fila f = fila();
  f = f.Enqueue('a');
  f = f.Enqueue('b');
  f = f.Enqueue('c');
  f = f.Enqueue('d');
  f = f.Enqueue('e');
  f = f.Enqueue('f');
  f = f.Enqueue('g');
  f = f.Enqueue('h');
  f = f.Enqueue('i');
  f = f.Enqueue('j');
  f = f.Enqueue('k');
  f = f.Enqueue('l');
  f = f.Enqueue('m');
  f = f.Enqueue('n');
  f = f.Enqueue('o');
  f = f.Enqueue('p');
  f = f.Enqueue('q');
  f = f.Enqueue('r');
  f = f.Enqueue('s');
  f = f.Enqueue('t');
  f = f.Enqueue('u');
  f = f.Enqueue('v');
  f = f.Enqueue('w');
  f = f.Enqueue('x');
  f = f.Enqueue('y');
  f = f.Enqueue('z');
  std::cout << f.Show();

  // Algumas queries de K-th()
  std::cout << f.K(1) << std::endl;
  std::cout << f.K(2) << std::endl;
  std::cout << f.K(3) << std::endl;
  std::cout << f.K(4) << std::endl;
  std::cout << f.K(5) << std::endl;
  std::cout << f.K(6) << std::endl;
  std::cout << f.K(7) << std::endl;
  std::cout << f.K(8) << std::endl;
  std::cout << f.K(9) << std::endl;
  std::cout << f.K(10) << std::endl;
  std::cout << f.K(11) << std::endl;
  std::cout << f.K(12) << std::endl;
  std::cout << f.K(13) << std::endl;
  std::cout << f.K(14) << std::endl;
  std::cout << f.K(15) << std::endl;
  std::cout << f.K(16) << std::endl;
  std::cout << f.K(17) << std::endl;
  std::cout << f.K(18) << std::endl;
  std::cout << f.K(19) << std::endl;
  std::cout << f.K(20) << std::endl;
  std::cout << f.K(21) << std::endl;
  std::cout << f.K(22) << std::endl;
  std::cout << f.K(23) << std::endl;
  std::cout << f.K(24) << std::endl;
  std::cout << f.K(25) << std::endl;
  std::cout << f.K(26) << std::endl;

  // Resultado: OK
}

int main (int argc, char *argv[]) {
  //Teste1();  
  Teste2();
  //Teste3();
}
