CFLAGS = -std=gnu11 -Wall -Wextra
LDFLAGS = -Wl,--as-needed
LDLIBS = -lncurses
OBJECTS = main.o minesweeper.o
BINARY = minecurses

INSTALL = install -m755

PREFIX = /usr/local

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -Og -g
else
	CFLAGS += -flto -O2 -DNDEBUG
	LDFLAGS += -flto -O2
endif

all: $(BINARY)

minecurses: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

main.o: main.c minesweeper.h
minesweeper.o: minesweeper.c minesweeper.h

install: $(BINARY)
	$(INSTALL) $^ $(DESTDIR)$(PREFIX)/bin

uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/bin/$(BINARY)

TAGS:
	ctags -R .

clean:
	$(RM) $(OBJECTS) $(BINARY) tags

.PHONY: all install clean
