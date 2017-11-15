CC=g++
CFLAGS=-std=c++11 -I -Wall -lpthread -lstdc++ -lgcrypt -lgpg-error
DEPS = pastry.h
OBJ = pastryNode.o pastryNodeServer.o 

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

pastry: $(OBJ)
	g++ -o $@ $^ $(CFLAGS)