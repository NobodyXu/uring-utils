CC = clang
CXX = clang++

CFLAGS := -march=native -mtune=native -Oz -Wall -flto
CFLAGS += -fno-asynchronous-unwind-tables -fno-unwind-tables

CXXFLAGS := -fno-exceptions -fno-rtti

LDFLAGS := -s -flto -fuse-ld=lld -Wl,-icf=all,--gc-sections,--plugin-opt=O3,-O3
LIBS := -L /usr/local/lib/ -l:liburing.a

## Objects to build
SRCS := $(shell find . -name '*.c' -a ! -wholename './test/*' -a ! -wholename './benchmark/*')
OBJS := $(SRCS:.c=.o)

## Build rules
uring-utils: $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) $^ -o $@

%.o: %.c Makefile
	$(CC) -std=c11 -c $(CFLAGS) -o $@ $<

%.o: %.c %.h Makefile
	$(CC) -std=c11 -c $(CFLAGS) -o $@ $<

%.o: %.cc %.hpp Makefile
	$(CXX) -std=c++17 -c $(CXXFLAGS) $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) uring-utils

.PHONY: clean all
