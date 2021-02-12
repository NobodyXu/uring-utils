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

%.out: %.c utility.hpp Makefile
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

%.out: %.cc utility.hpp Makefile
	$(CXX) $(CXXFLAGS) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OUTS)

.PHONY: test clean

## Dependencies
test_parser.out: ../src/parser.c ../src/parser.h
