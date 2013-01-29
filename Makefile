.PHONY: all clean
all:
	
BIN=uniqueheights stash describefield extractfield

CFLAGS+=-std=c99 -D_GNU_SOURCE
CFLAGS+=-MMD -MP -g

extractfield:LDLIBS+=-lnetcdf
$(BIN):obj/fieldsfile.o
extractfield:obj/list.o

all:$(BIN)
clean:
	$(RM) *.d *.o $(BIN)

obj/%.o:src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<
%:obj/%.o
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

-include $(wildcard obj/*.d)
