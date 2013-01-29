.PHONY: all clean
all:
	
BIN=uniqueheights stash describefield extractfield

CFLAGS+=-std=c99 -D_GNU_SOURCE
CFLAGS+=-MMD -MP -g

extractfield:LDLIBS+=-lnetcdf
$(BIN):fieldsfile.o
extractfield:list.o

all:$(BIN)
clean:
	$(RM) *.d *.o $(BIN)

-include $(wildcard *.d)
