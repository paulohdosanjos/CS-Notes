#include <iostream>
#include "./stack_skew.h"
#include <chrono>

typedef Stack<char> pilha;

void Teste1() {
  // Testes genéricos
  pilha p0 = pilha();
  std::cout << "p0: "; //p0: 
  std::cout << p0.Show();

  pilha p1 = p0.Push('a'); //p1: a
  std::cout << "p1: ";
  std::cout << p1.Show();

  pilha p2 = p1.Push('b'); //p2: b a
  std::cout << "p2: ";
  std::cout << p2.Show();

  pilha p3 = p2.Push('c'); //p3: c b a
  std::cout << "p3: ";
  std::cout << p3.Show();

  pilha p4 = p3.Pop();
  std::cout << "p4: "; //p4: b a
  std::cout << p4.Show();

  pilha p5 = p4.Pop(); //p5: a
  std::cout << "p5: ";
  std::cout << p5.Show();

  pilha p6 = p5.Pop(); //p6: 
  std::cout << "p6: ";
  std::cout << p6.Show();

  pilha p7 = p3.Push('d'); //p7: d c b a
  std::cout << "p7: ";
  std::cout << p7.Show();

  //Resultado: OK
}


void Teste2() {

  // Testa K-th()
  pilha p = pilha();
  p = p.Push('a');
  p = p.Push('b');
  p = p.Push('c');
  p = p.Push('d');
  p = p.Push('e');
  p = p.Push('f');
  p = p.Push('g');
  p = p.Push('h');
  p = p.Push('i');
  p = p.Push('j');
  p = p.Push('k');
  p = p.Push('l');
  p = p.Push('m');
  p = p.Push('n');
  p = p.Push('o');
  p = p.Push('p');
  p = p.Push('q');
  p = p.Push('r');
  p = p.Push('s');
  p = p.Push('t');
  p = p.Push('u');
  p = p.Push('v');
  p = p.Push('w');
  p = p.Push('x');
  p = p.Push('y');
  p = p.Push('z');
  std::cout << p.PrintJumpPointers();

  // Algumas queries de LA
  std::cout << p.K(1) << std::endl;
  std::cout << p.K(2) << std::endl;
  std::cout << p.K(3) << std::endl;
  std::cout << p.K(4) << std::endl;
  std::cout << p.K(5) << std::endl;
  std::cout << p.K(6) << std::endl;
  std::cout << p.K(7) << std::endl;
  std::cout << p.K(8) << std::endl;
  std::cout << p.K(9) << std::endl;
  std::cout << p.K(10) << std::endl;
  std::cout << p.K(11) << std::endl;
  std::cout << p.K(12) << std::endl;
  std::cout << p.K(13) << std::endl;
  std::cout << p.K(14) << std::endl;
  std::cout << p.K(15) << std::endl;
  std::cout << p.K(16) << std::endl;
  std::cout << p.K(17) << std::endl;
  std::cout << p.K(18) << std::endl;
  std::cout << p.K(19) << std::endl;
  std::cout << p.K(20) << std::endl;
  std::cout << p.K(21) << std::endl;
  std::cout << p.K(22) << std::endl;
  std::cout << p.K(23) << std::endl;
  std::cout << p.K(24) << std::endl;
  std::cout << p.K(25) << std::endl;

  // Resultado: OK
}

void Teste3(int SIZE) {
  // Teste K-th() para pilha grande


  Stack<int> p = Stack<int>();
  for(int i = 0 ; i < SIZE ; i++){
    p = p.Push(i);
  }

  auto start = std::chrono::high_resolution_clock::now();

  std::cout << p.K(1) << std::endl;
  std::cout << p.K((int)SIZE/2) << std::endl;
  std::cout << p.K((int)(2*SIZE/3)) << std::endl;
  std::cout << p.K(SIZE-1) << std::endl;
      
  auto end = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

  std::cout << "tempo das queries: " << duration << " micro segundos" << std::endl;

  // Resultado: Rápido independente do tamanho da pilha. O que é de esperar pois K-th roda em tempo O(log(k))

}


int main() {
  Teste1();
  //Teste2();
  //Teste3(1000000);

}
