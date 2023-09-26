// Tarefa t3 - Abb com suporte a K-th() e Top()
// Paulo Henrique Albuquerque
// 12542251

#include <iostream>
#include <utility>


#define MAX_PRIORITY 1001
#define INFTY 12345
#define GARBAGE_VALUE -42
#define SEED 5

bool verbose;
#define DEBUG_MODE 0

#if DEBUG_MODE
#define DEBUG(x) if(verbose) std::cout << x
#else
#define DEBUG(x)
#endif

#define _rand() (std::rand() % MAX_PRIORITY)

class Node {
  public:
    int key;
    int priority;
    int value;
    int operation; // push = +1 e pop = -1
    int max; // maior chave na subárvore
    int smax; // maior sufixo na subárvore
    int sum; // soma de créditos na subárvore


    Node* left;
    Node* right;
   
    Node(const int _key, int _priority, int _value, int _operation, int _max, int _smax, int _sum, Node* l, Node* r): key(_key), priority(_priority), value(_value), operation(_operation), max(_max), smax(_smax), sum(_sum), left(l), right(r) {}
};

class Abb {

  public:
    
    Abb() {std::srand(SEED); root = nullptr;}

    // Insere folha de operação op (de valor x para pushs) na árvore no instante t. 
    void Insert(const int t, const int op, const int x) { root = _Insert(root, t, op, x); }

    // Remove folha com chave t da árvore 
    void Delete(const int t) { root = _Delete(root, t); }
    
    // Retorna a soma dos "créditos" das folhas com chave <= t
    int Sum(const int t) { return _Sum(root, t); }

    // Retorna k-ésimo elemento da pilha do instante t
    int K(const int t, const int k)
    {
      std::pair p = _FindSubtree(root, t, k);
      return _K(p.second, p.first);
    }

    // Retorna o elemento do topo da pilha do instante t
    int Top(const int t){ return K(t, 1); }

    // Imprime a árvore deitada. Para depuração
    void Print() const { _Print(root, 0); }

  private:
    Node* root;

    // Acha valor do "k-ésimo" push() mais à direita da árvore enraizada em r. Pressupõe que tal push() exista em r
    int _K(Node* r, int k)
    {
      if(r->right == nullptr && r->left == nullptr) return r->value; // Cheguei numa folha: a lógica da função impõe que esse nó é o push() com o valor desejado.

      if(r->right->smax >= k){ // Tento ir sempre primeiro para a direita
        return _K(r->right, k);
      }
      else return _K(r->left, k - r->right->sum);
    }

    // Retorna a soma dos "créditos" das folhas com chave <= t na árvore enraizada em r
    int _Sum(Node* r, const int t)
    {
      if(r->left == nullptr && r->right == nullptr){
        if(t >= r->key) return r->operation;
        else return 0;
      } 

      if(t > r->left->max) return r->left->sum + _Sum(r->right, t);
      else return _Sum(r->left, t);
    }

    // Acha e retorna subárvore que contém o "push certo". Retorna par (k, p) conforme dica do fórum
    std::pair<int, Node*> _FindSubtree(Node* r, const int t, const int k)
    {
      if(r->left == nullptr && r->right == nullptr) // Folha
      {
        // Ajusta k de acordo com a operação da folha
        if(t >= r->key)
        {
          if(r->operation == 1) return std::make_pair(k-1, (k-1 == 0 ? r : nullptr));
          else return std::make_pair(2 + k-1, nullptr);
        }

        else return std::make_pair(k, nullptr);
      }

      // Nó interno
      std::pair<int, Node*> p;
      if(t > r->left->max)
      { 
        p = _FindSubtree(r->right, t, k);
        if(p.second == nullptr) // Ainda não achei
        { 
          if(r->left->smax >= p.first){ // Achei agora
            return std::make_pair(p.first, r->left);
          }
          else return std::make_pair(p.first - r->left->sum, nullptr);
        }
        else // Já achei a subárvore, retorna para cima
         return p; 
      }
      else return _FindSubtree(r->left, t, k);
    }

    // Insere uma nova chave x na árvore enraizada em r
    Node* _Insert(Node* r, const int t, const int op, const int x)
    {
      if(r == nullptr) return new Node(t, -INFTY, x, op, t, (op == -1 ? 0 : 1), op, nullptr, nullptr); // Árvore vazia 
      
      if(r->left == nullptr && r->right == nullptr)
      { 
        DEBUG("Folha. Criando novo nó splitado\n");
        // r é folha. Cria novo nó interno pai de r e da nova folha a ser adicionada
        Node* new_leaf = new Node(t, -INFTY, x, op, t, (op == -1 ? 0 : 1), op, nullptr, nullptr);
        Node* new_parent;

        if(t > r->key)
        {
          int smax = std::max(std::max(0, op), op + r->operation); // Máximo entre os três sufixos possivéis
          new_parent = new Node(GARBAGE_VALUE, _rand(), GARBAGE_VALUE, GARBAGE_VALUE, t, smax, op + r->operation, r, new_leaf); 
        }

        else 
        {
          int smax = std::max(std::max(0, r->operation), op + r->operation); // Máximo entre os três sufixos possivéis
          new_parent = new Node(GARBAGE_VALUE, _rand(), GARBAGE_VALUE, GARBAGE_VALUE, r->key, smax, op + r->operation, new_leaf, r);
        }

        DEBUG("Prioridade do novo nó : ");
        DEBUG(new_parent->priority);
        DEBUG("\n");
        return new_parent;
      }

      // r é nó interno
      DEBUG("Nó interno\n");
      r->max = std::max(r->max, t);
      if(t > r->left->max) 
      {
        DEBUG("Adicionando à direita\n");
        r->right = _Insert(r->right, t, op, x);

        r->sum = r->left->sum + r->right->sum;
        r->smax = std::max(r->right->smax, r->right->sum + r->left->smax);

        // Rotação da treap
        if(r->right->priority > r->priority)
        {
          DEBUG("Rotacionando para a esquerda\n");
          Node* child = r->right;
          RotateLeft(r, child);
          r = child;
        }
      }
     
      else 
      {
        DEBUG("Adicionando à esquerda\n");
        r->left = _Insert(r->left, t, op, x);

        r->sum = r->left->sum + r->right->sum;
        r->smax = std::max(r->right->smax, r->right->sum + r->left->smax);

        // Rotação da treap
        if(r->left->priority > r->priority)
        {
          DEBUG("Rotacionando para a direita\n");
          Node* child = r->left;
          RotateRight(r, child);
          r = child;
        }
    }

    return r;
    }
    
    // Deleta folha de chave t da árvore enraizada em r. Devolve a árvore resultante.
    Node* _Delete(Node* r, int const t)
    {
      if(r == nullptr) return nullptr; // Árvore vazia
      if(r->left == nullptr && r->right == nullptr) 
      {
        if(r->key == t) return nullptr; // Chave encontrada
        else return r; // Chave não existe na árvore. Não faz nada
      }

      if(t > r->left->max)
      {
        r->right = _Delete(r->right, t);
        if(r->right == nullptr) return r->left; // Shortcut
      }

      else
      {
        r->left =_Delete(r->left, t); 
        if(r->left == nullptr) return r->right; // Shortcut
      }

      // Atualiza campos
      r->max = std::max(r->left->max, r->right->max);
      r->sum = r->left->sum + r->right->sum;
      r->smax = std::max(r->right->smax, r->right->sum + r->left->smax);

      return r;
    }

    // v é o pai e u é o filho
    void RotateLeft(Node* v, Node* u)
    {
      // Como as rotações só acontecem entre nós internos, todos os campos estão devidamente inicializados nesse ponto
      int new_v_max = std::max(u->left->max, v->left->max);
      int new_v_sum = u->left->sum + v->left->sum;
      int new_v_smax = std::max(u->left->smax, u->left->sum + v->left->smax); 

      int new_u_max = v->max;
      int new_u_sum = v->sum;
      int new_u_smax = v->smax;

      // Atualiza campos
      v->max = new_v_max;
      v->sum = new_v_sum;
      v->smax = new_v_smax;
      u->max = new_u_max;
      u->sum = new_u_sum;
      u->smax = new_u_smax;

      // Atualiza ponteiros
      v->right = u->left;
      u->left = v;
    }

    // u é o pai e v é o filho
    void RotateRight(Node* u, Node* v)
    {
      // Como as rotações só acontecem entre nós internos, todos os campos estão devidamente inicializados nesse ponto
      int new_u_max = std::max(u->right->max, v->right->max);
      int new_u_sum = u->right->sum + v->right->sum;
      int new_u_smax = std::max(u->right->smax, u->right->sum + v->right->smax); 

      int new_v_max = u->max;
      int new_v_sum = u->sum;
      int new_v_smax = u->smax; 

      // Atualiza campos
      u->max = new_u_max;
      u->sum = new_u_sum;
      u->smax = new_u_smax;
      v->max = new_v_max;
      v->sum = new_v_sum;
      v->smax = new_v_smax;

      // Atualiza ponteiros
      u->left = v->right;
      v->right = u;
    }

    // Imprime árvore enraizada em u deitada, após i espaços em branco 
    void _Print(Node* u, int i) const
    {
      if(u == nullptr) return;
      _Print(u->left, i + 4); 
      std::string space (i, ' '); 
      std::cout << space;
      if(u->left == nullptr && u->right == nullptr) std::cout << (u->operation == 1 ? "push(" + std::to_string(u->value) + ")" : "pop()") << "\n";
      else std::cout << u->sum << " " << u->smax << " " << u->priority<< "\n";
      _Print(u->right, i + 4);
    }

};
