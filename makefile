COMPILER=gcc

target: run

#TODO find how to to parameterize this, %(lib).o: %(lib).h %(lib).c

linearfit.o: linearfit.h linearfit.c
	$(COMPILER) -c linearfit.c -o linearfit.o

microtime.o: microtime.h microtime.c
	$(COMPILER) -c microtime.c -o microtime.o

sic.o: sic.h sic.c
	$(COMPILER) -c sic.c -o sic.o

test: test.c sic.o linearfit.o
	$(COMPILER) test.c sic.o linearfit.o -o test

client: client.c 
	$(COMPILER) client.c microtime.o -o client

server: server.c 
	$(COMPILER) server.c microtime.o -o server

run: client server
	./client

runtests: test
	./test

clear:
	-rm main test client server *.o

clean: clear


