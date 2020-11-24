COMPILER=gcc #-DDEBUG

target: compile

#TODO find how to to parameterize this, %(lib).o: %(lib).h %(lib).c

cunit.o: cunit.h cunit.c
	$(COMPILER) -c cunit.c -o cunit.o


circularOrderedArray.o: circularOrderedArray.h circularOrderedArray.c
	$(COMPILER) -c circularOrderedArray.c -o circularOrderedArray.o

circularOrderedArrayTest.o: circularOrderedArray.h circularOrderedArray.c
	$(COMPILER) -DCIRCULAR_ORDERED_ARRAY_MAX_SIZE=10 -c circularOrderedArray.c -o circularOrderedArrayTest.o

circularOrderedArrayTestCase: circularOrderedArrayTestCase.c circularOrderedArrayTest.o cunit.o
	$(COMPILER) -DCIRCULAR_ORDERED_ARRAY_MAX_SIZE=10 circularOrderedArrayTestCase.c circularOrderedArrayTest.o cunit.o -o circularOrderedArrayTestCase



linearfit.o: linearfit.h linearfit.c
	$(COMPILER) -c linearfit.c -o linearfit.o

linearfitTest.o: linearfit.h linearfit.c
	$(COMPILER) -DCICRULAR_LINEAR_FIT_ARRAY_MAX_SIZE=4 -c linearfit.c -o linearfitTest.o

linearfitTestCase: linearfitTestCase.c linearfitTest.o cunit.o
	$(COMPILER) -DCICRULAR_LINEAR_FIT_ARRAY_MAX_SIZE=4 linearfitTestCase.c linearfitTest.o cunit.o -o linearfitTestCase


halfSampleMode.o: halfSampleMode.h halfSampleMode.c
	$(COMPILER) -c halfSampleMode.c -o halfSampleMode.o

halfSampleModeTestCase: halfSampleModeTestCase.c halfSampleMode.o cunit.o
	$(COMPILER) halfSampleModeTestCase.c halfSampleMode.o cunit.o -o halfSampleModeTestCase


sic.o: sic.h sic.c
	$(COMPILER) -c sic.c -o sic.o

sicTestCase: sicTestCase.c sic.o linearfit.o circularOrderedArray.o halfSampleMode.o cunit.o
	$(COMPILER) sicTestCase.c sic.o linearfit.o circularOrderedArray.o halfSampleMode.o cunit.o -o sicTestCase


microtime.o: microtime.h microtime.c
	$(COMPILER) -c microtime.c -o microtime.o



client: client.c microtime.o
	$(COMPILER) client.c microtime.o -o client

server: server.c microtime.o
	$(COMPILER) server.c microtime.o -o server


update: linearfit.h linearfit.c circularOrderedArray.h circularOrderedArray.c sic.h sic.c microtime.h microtime.c halfSampleMode.h halfSampleMode.c
	cp linearfit.h linearfit.c circularOrderedArray.h circularOrderedArray.c sic.h sic.c microtime.h microtime.c halfSampleMode.h halfSampleMode.c ../esp32/main/tictoc

compile: client server


runtests: circularOrderedArrayTestCase halfSampleModeTestCase linearfitTestCase sicTestCase
	./circularOrderedArrayTestCase
	./halfSampleModeTestCase
	./linearfitTestCase
	./sicTestCase

clear:
	-rm main client server *.o *TestCase

clean: clear


