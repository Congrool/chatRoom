all: test testClient

test: test.o Socket.o ErrorLog.o
	g++ test.o ./base/Socket.o ./base/ErrorLog.o -o test

testClient: testClient.o  Socket.o  ErrorLog.o
	g++ testClient.o ./base/Socket.o ./base/ErrorLog.o -o testClient

testClient.o: testClient.cpp
	g++ -c testClient.cpp -lpthread -I"./" -o testClient.o

test.o: test.cpp
	g++ -c test.cpp -lpthread -I"./" -o test.o

Socket.o: ./base/Socket.cpp
	g++ -c ./base/Socket.cpp -lpthread -I"./" -o ./base/Socket.o

ErrorLog.o: ./base/ErrorLog.cpp
	g++ -c ./base/ErrorLog.cpp -I"./" -o ./base/ErrorLog.o

clean:
	rm -rf test.o testClient.o ./base/Socket.o ./base/ErrorLog.o test testClient