all:uniqueheights stash describefield

CFLAGS+=-std=c99 -D_GNU_SOURCE
CFLAGS+=-MMD -MP

uniqueheights stash describefield:fieldsfile.o

-include $(wildcard *.d)
