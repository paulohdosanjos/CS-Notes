//basic stack implementation

#include <iostream>
#include <gtest/gtest.h>

using namespace std;

template<typename T> class Node {
    public:
      T val;
      Node *parent;
      unsigned int depth;

      Node(const T val, Node *parent){
        this->val = val;
        this->parent = parent;
        parent == nullptr? this->depth = 0 : this->depth = parent->depth + 1; 
      }
};

int paulo = 3;


template<typename T> class Stack{
  
  public:
    Stack(Node<T> *n){
      this->first = n;
    }

    Stack() : first(nullptr) {};

    Stack Push(const T x){
      return Stack(new Node(x, first));
    }

    Stack Pop(){
      return Stack<T>(first->parent);
    }

    T Top() const{
      return this->first->val;
    }

    unsigned int Size() const{
      return (first == nullptr? 0 : first->depth + 1);
    }

  private:
    Node<T> *first;

};

TEST(BasicStack, Example) {
  Stack<int> p0;
	EXPECT_EQ(p0.Size(), 0);
	Stack<int> p1 = p0.Push(5);
	Stack<int> p2 = p1.Push(7);
	Stack<int> p3 = p2.Push(6);

	////p3 = p3; // This may break stuff
	EXPECT_EQ(p3.Size(), 3);
	Stack<int> tmp = p3;
	for(int x : {6, 7, 5}) {
		EXPECT_EQ(tmp.Top(), x);
	  tmp = tmp.Pop();
	}
	Stack<int> p4 = p2.Pop();
	EXPECT_EQ(p3.Top(), 6);
	Stack<int> p5 = p4.Push(9);
	EXPECT_EQ(p4.Top(), 5);
	Stack<int> p6 = p0.Push(5);
	EXPECT_EQ(p5.Top(), 9);
	EXPECT_EQ(p6.Top(), 5);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


