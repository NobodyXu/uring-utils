CC := clang
CXX := clang++

CFLAGS := -Wall -g -fsanitize=address
LDFLAGS := -fuse-ld=lld

CXXFLAGS := 

## Objects to build
C_SRCS := $(wildcard *.c)
CXX_SRCS := $(wildcard *.cc)

OUTS := $(C_SRCS:.c=.out) $(CXX_SRCS:.cc=.out)

## Build rules
test: $(OUTS)
	./run_tests.sh "$(OUTS)"

%.out: %.c Makefile
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

%.out: %.cc Makefile
	$(CXX) $(CXXFLAGS) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OUTS)

.PHONY: test clean

## Dependencies
test_parser.o: test_parser.cc Makefile ../src/parser.c ../src/parser.h
