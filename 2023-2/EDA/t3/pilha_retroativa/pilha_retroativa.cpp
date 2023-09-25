#include <iostream>

#include "abb.cpp"

class RetroStack
{
  public:
    RetroStack() { a = Abb(); }

    void InsertPush(const int t, const int x) { a.Insert(t, 1, x); }

    void InsertPop(const int t) { a.Insert(t, -1, GARBAGE); }

    void Delete(const int t) { a.Delete(t); }

    int Size(const int t) { return a.Sum(t); }

    int Top(const int t) { return a.Top(t); }

    int K(const int t, const int k) { return a.K(t, k); }

    void Print(const int t)
    {
      int size = Size(t);
      for(int i = 1; i <= size; i++) std::cout << a.K(t, i) << " ";
      std::cout << "\n";

    }

  private:
    Abb a;
};

int main() {
  RetroStack s = RetroStack(); 

  for(std::string line; std::getline(std::cin, line) ; ) {
    int opt, t, x;
    std::istringstream iss = std::istringstream(line);
    iss >> opt >> t >> x;

    switch (opt) {
      case 1:
        s.InsertPush(t, x);
        break;
      case 2:
        s.InsertPop(t);
        break;
      case 3:
        s.Delete(t);
        break;
      case 4:
        std::cout << s.Size(t) << "\n";
        break;
      case 5:
        std::cout << s.Top(t) << "\n";
        break;
      case 6:
        std::cout << s.K(t, x) << "\n";
        break;
      case 7:
        s.Print(t);
        break;
    }
  }
}
