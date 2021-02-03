CC = clang
CXX = clang++

CFLAGS := -march=native -mtune=native -O2 -Wall -flto
CFLAGS += -fno-asynchronous-unwind-tables -fno-unwind-tables

CXXFLAGS := -fno-exceptions -fno-rtti

LDFLAGS := -s -flto -fuse-ld=lld -Wl,-icf=all,--gc-sections,--plugin-opt=O3,-O3
LIBS := -L /usr/local/lib/ -l:liburing.a

## Objects to build
SRCS := $(shell find . -name '*.c' -a ! -wholename './test/*' -a ! -wholename './benchmark/*')
OBJS := $(SRCS:.c=)

## Build rules
all: $(OBJS)

%: %.c Makefile
	$(CC) -std=c11 $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@ $<

%: %.cc Makefile
	$(CXX) -std=c++17 $(CXXFLAGS) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@ $<

clean:
	rm -f $(OBJS)

.PHONY: clean all
