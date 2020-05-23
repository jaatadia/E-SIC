COMPILER=gcc

target: run

#TODO find how to to parameterize this, %(lib).o: %(lib).h %(lib).c
circularOrderedArray.o: circularOrderedArray.h circularOrderedArray.c
	$(COMPILER) -c circularOrderedArray.c -o circularOrderedArray.o

circularOrderedArrayTest.o: circularOrderedArray.h circularOrderedArray.c
	$(COMPILER) -DCIRCULAR_ORDERED_ARRAY_MAX_SIZE=10 -c circularOrderedArray.c -o circularOrderedArrayTest.o

circularOrderedArrayTestCase: circularOrderedArrayTestCase.c circularOrderedArrayTest.o
	$(COMPILER) -DCIRCULAR_ORDERED_ARRAY_MAX_SIZE=10 circularOrderedArrayTestCase.c circularOrderedArrayTest.o -o circularOrderedArrayTestCase

linearfit.o: linearfit.h linearfit.c
	$(COMPILER) -c linearfit.c -o linearfit.o

microtime.o: microtime.h microtime.c
	$(COMPILER) -c microtime.c -o microtime.o

sic.o: sic.h sic.c
	$(COMPILER) -c sic.c -o sic.o

test: test.c sic.o linearfit.o
	$(COMPILER) test.c sic.o linearfit.o -o test

client: client.c microtime.o
	$(COMPILER) client.c microtime.o -o client

server: server.c microtime.o
	$(COMPILER) server.c microtime.o -o server

run: client server
	./client

runtests: test
	./circularOrderedArrayTestCase

clear:
	-rm main client server *.o *TestCase

clean: clear


