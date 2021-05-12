TESTFILES := $(wildcard *.crona)
TESTS := $(TESTFILES:.crona=.test)

.PHONY: all

all: $(TESTS)

%.test:
	@rm -f $*.err $*.out
	@touch $*.err $*.out
	@echo "TEST $*"
	@../../cronac $*.crona -t $*.out 2> $*.err;\
	PROG_EXIT_CODE=$$?;\
	echo "Comparing token output for $*.crona...";\
	diff -B --ignore-all-space $*.out $*.out.expected;\
	TAC_DIFF_EXIT=$$?;

clean:
	rm -f *.out *.err
