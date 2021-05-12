TESTFILES := $(wildcard *.crona)
TESTS := $(TESTFILES:.crona=.test)

.PHONY: all

all: $(TESTS)

%.test:
	@echo "Testing $*.crona"
	@touch $*.out #Creates out file to diff in case of no output
	@touch $*.err #The @ means don't show the command being invoked
	-@../../cronac $*.crona -n $*.out 2> $*.err ;\
	echo "diff output...";\
	diff -w $*.out $*.out.expected; \
	echo "diff error...";\
	diff -w $*.err $*.err.expected;
clean:
	rm -f *.out *.err
