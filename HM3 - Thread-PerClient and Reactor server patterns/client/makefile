CFLAGS:=-c -Wall -Weffc++ -g -std=c++14 -Iinclude
LDFLAGS:=-lboost_system -lpthread

all: EchoClient
all: StompWCIClient

EchoClient: bin/ConnectionHandler.o bin/echoClient.o
	g++ -o bin/EchoClient bin/ConnectionHandler.o bin/echoClient.o $(LDFLAGS)

StompWCIClient: bin/ConnectionHandler.o bin/event.o bin/GameReports.o bin/StompProtocol.o bin/StompClient.o
	g++ -o bin/StompWCIClient bin/ConnectionHandler.o bin/event.o bin/GameReports.o bin/StompProtocol.o bin/StompClient.o $(LDFLAGS)

bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/echoClient.o: src/echoClient.cpp
	g++ $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp

bin/event.o: src/event.cpp
	g++ $(CFLAGS) -o bin/event.o src/event.cpp

bin/StompClient.o: src/StompClient.cpp
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

bin/GameReports.o: src/GameReports.cpp
	g++ $(CFLAGS) -o bin/GameReports.o src/GameReports.cpp

bin/StompProtocol.o: src/StompProtocol.cpp
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

.PHONY: clean
clean:
	rm -f bin/*
	