all: serverSocket serverSocketUnbounded serverSocketBounded serverSocketThreads clientSocket launchClient

launchClient: launchClient.c
	gcc -o launchClient launchClient.c

serverSocket: serverSocket.c socketMng.o
	gcc -o serverSocket -g serverSocket.c socketMng.o

serverSocketUnbounded: serverSocketUnbounded.c socketMng.o
	gcc -o serverSocketUnbounded -g serverSocketUnbounded.c socketMng.o

serverSocketBounded: serverSocketBounded.c socketMng.o
	gcc -o serverSocketBounded -g serverSocketBounded.c socketMng.o

serverSocketThreads: serverSocketThreads.c socketMng.o
	gcc -pthread -o serverSocketThreads -g serverSocketThreads.c socketMng.o

clientSocket: clientSocket.c socketMng.o
	gcc -o clientSocket -g clientSocket.c socketMng.o

socketMng.o: socketMng.c
	gcc -c -g socketMng.c

clean: 
	rm -f serverSocket serverSocketUnbounded serverSocketBounded serverSocketThreads clientSocket socketMng.o launchClient
