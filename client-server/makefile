COMPILER=gcc -g #-g for debugging info
DOT_O=circularOrderedArray.o linearfit.o halfSampleMode.o sic.o esic.o microtime.o node.o
TEST_DOT_O=cunit.o parser.o


target: compile

%.o: %.c %.h
	$(COMPILER) -c $< -o $@

%TestCase: %TestCase.c $(DOT_O) $(TEST_DOT_O)
	$(COMPILER) $^ -o $@

%: %.c $(DOT_O)
	$(COMPILER) $^ -o $@

compile: client server

runtests: circularOrderedArrayTestCase halfSampleModeTestCase linearfitTestCase esicTestCase
	./circularOrderedArrayTestCase
	./halfSampleModeTestCase
	./linearfitTestCase
	./esicTestCase

clear:
	-rm -f main client server *.o *TestCase *.py

clean: clear


