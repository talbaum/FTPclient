CFLAGS:=-c -Wall -Weffc++ -g -Wno-multichar -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lboost_locale -lboost_thread

DEPS =include/connectionHandler.h include/encDec.h
OBJ = bin/connectionHandler.o bin/echoClient.o bin/encDec.o

bin/%.o: src/%.cpp $(DEPS)
	g++ $(CFLAGS) -c -o $@ $<
	
bin/TFTPclient: $(OBJ)
	g++ $(CFLAGS) -o $@ $^

clean:
	rm -f bin/*