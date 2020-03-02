COMPILER=gcc

target: run

linearfit.o: linearfit.h linearfit.c
	$(COMPILER) -c linearfit.c -o linearfit.o

sic.o: sic.h sic.c
	$(COMPILER) -c sic.c -o sic.o

test: test.c sic.o linearfit.o
	$(COMPILER) test.c sic.o linearfit.o -o test

main: main.c sic.o linearfit.o
	$(COMPILER) main.c sic.o linearfit.o -o main

run: main
	./main

runtests: test
	./test

clear:
	-rm main test *.o

clean: clear


