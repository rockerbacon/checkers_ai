CXX=g++ -std=c++11
OBJS=sprite.o window.o Main.o Board.o Minimax.o
LFLAGS=-lSDL2 -lSDL2_image
CFLAGS=-O3
DEPS=Lab309_ADT_Container.h Lab309_ADT_DoublyLinkedList.h Lab309_ADT_Matrix.h Board.h

%.o: %.cpp $(DEPS)
	$(CXX) $(CFLAGS) -c $< $(LFLAGS)
	
main: $(OBJS)
	$(CXX) $(CFLAGS) -o main $(OBJS) $(LFLAGS)
	
clean:
	rm -f *.o
	rm -f main
