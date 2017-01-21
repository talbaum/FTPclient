INC_DIR=include
SRC_DIR=src
OBJ_DIR=bin
CC = g++
LDFLAGS:=-lboost_system -lboost_locale -lboost_thread
CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
DEPS = $(INC_DIR)/connectionHandler.h $(INC_DIR)/encDec.h
OBJ =  $(OBJ_DIR)/echoClient.o $(OBJ_DIR)/connectionHandler.o $(OBJ_DIR)/encDec.o

all: bin/TFTPclient
	g++ -o bin/TFTPclient $(OBJ) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<
	
bin/TFTPclient: $(OBJ)

.PHONY: clean
clean:
	rm -f bin/*