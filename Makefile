CFLAGS = -std=gnu11 -Wall -Wextra
LDFLAGS = -Wl,--as-needed
LDLIBS = -lncurses
OBJECTS = main.o minesweeper.o config.o visual.o
TESTS = test_config
BINARY = minecurses

INSTALL = install -m755

PREFIX = /usr/local

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -Og -g
else
	CFLAGS += -flto -O2
	LDFLAGS += -flto -O2
endif

all: $(BINARY)

minecurses: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

main.o: main.c minesweeper.h
minesweeper.o: minesweeper.c minesweeper.h

check: $(TESTS)
	@./test_config

test_config: config.o test_config.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

test_config.o: test_config.c config.c config.h

install: $(BINARY)
	$(INSTALL) $^ $(DESTDIR)$(PREFIX)/bin

uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/bin/$(BINARY)

TAGS:
	ctags -R .

clean:
	$(RM) $(OBJECTS) $(BINARY) tags

.PHONY: all install clean
