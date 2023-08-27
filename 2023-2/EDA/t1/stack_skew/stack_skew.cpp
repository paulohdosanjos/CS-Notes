// Esse arquivo é incluído por stack_skew.h

template<class T> 
Stack<T>::Stack(Node<T> *n) {top = n;}

template<class T>
Stack<T>::Stack(){ 
  Node<T>* u = new Node<T>(); 
  top = u;
}

template<class T>
Stack<T> Stack<T>::Push(const T x){ 
  unsigned int depth = top->depth + 1;
  Node<T>* v = new Node<T>(x,depth);
  BuildJumpPointer(v,top);

  return Stack(v);
}

// Supõe char
template<class T>
std::string Stack<T> :: Show() {
  unsigned int size = Size();

  std::string output = std::string();

  output = output + "(" + std::to_string(size) + ") ";
  Node<char> *x = top;
  for(int i = 0; i < size; i++){
    output = output + x->val + " ";
    x = x->parent;
  }
  output = output + "\n";

  return output;
}

// Supõe int
template<class T>
std::string Stack<T> :: Show_Int() {
  unsigned int size = Size();

  std::string output = std::string();

  output = output + "(" + std::to_string(size) + ") ";
  Node<T> *x = top;
  for(int i = 0; i < size; i++){
    output = output + std::to_string(x->val) + " ";
    x = x->parent;
  }
  output = output + "\n";

  return output;
}


template<class T>
Stack<T> Stack<T> :: Pop() { // Pop numa pilha vazia deve retornar erro ou outra pilha vazia?
  if(IsEmpty()){}
  else{ return Stack(top->parent);}
}

template<class T>
T Stack<T> :: Top() const { 
  if(IsEmpty()){} // Erro, acessar o valor da raiz não é permitido, pois seu valor é lixo}
  else{return top->val;}
}

template<class T>
bool Stack<T> :: IsEmpty() const {
  return (top->depth == 0 ? true : false); // O campo depth sempre é inicializado
}

template<class T>
unsigned int Stack<T> :: Size() const { return top->depth; }

// Implementação usando representação skew binary
template<class T>
T Stack<T> :: K(unsigned  int k) const {
  Node<T>* x = top;
  unsigned int desired_depth = x->depth - k;

  while (x->depth != desired_depth) {
    if(x->jump->depth >= desired_depth) 
      x = x->jump;
    else 
      x = x->parent;
  }

  return x->val;
} 

template<class T>
void Stack<T> :: BuildJumpPointer(Node<T>* u, Node<T>* parent) {
   u->parent = parent;

   Node<T>* x = parent;
   if (x->jump != nullptr && x->jump->depth != 0 && x->depth - x->jump->depth == x->jump->depth - x->jump->jump->depth)
     u->jump = x->jump->jump;
   else
     u->jump = x;
 }

template<class T>
std::string Stack<T> :: PrintJumpPointers() const {
  Node<T>* x = top;
  std::string output = std::string();
  unsigned int size = Size();

  for(int i = 0; i < size; i++){
    output = output + x->val + " : " + x->jump->val + "\n";
    x = x->parent;
  }

  return output;
}

