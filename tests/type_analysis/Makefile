TESTFILES := $(wildcard *.crona)
TESTS := $(TESTFILES:.crona=.test)

.PHONY: all

all: $(TESTS)

%.test:
	@echo "Testing $*.crona"
	@touch $*.out #Creates out file to diff in case of no output
	@touch $*.err #The @ means don't show the command being invoked
	-@../../cronac $*.crona -c 2> $*.err ;\
	echo "diff error...";\
	diff -wB $*.err $*.err.expected;

clean:
	rm -f *.out *.err
