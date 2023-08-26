all: stack.o stack

stack.o: stack.cpp
	g++ -c stack.cpp

stack: stack.o test.cpp
	g++ stack.o test.cpp -o stack

clean:
	rm stack stack.o
