.PHONY: all clean
all:
	
BIN=uniqueheights stash describefield

CFLAGS+=-std=c99 -D_GNU_SOURCE
CFLAGS+=-MMD -MP

$(BIN):fieldsfile.o

all:$(BIN)
clean:
	$(RM) *.d *.o $(BIN)

-include $(wildcard *.d)
