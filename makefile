CC := gcc
CFLAGS := -O2 -c -Wall -Werror
LDFLAGS := -lncurses -lm

SRCPATH := ./src
BUILDPATH := ./build

SOURCES := minesweeper.c main.c
OBJECTS := $(addprefix $(BUILDPATH)/,$(SOURCES:%.c=%.o))
SOURCES := $(addprefix $(SRCPATH)/,$(SOURCES))

OUTFILE := minecurses

DESTDIR := /usr/bin

$(OUTFILE) : $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(OUTFILE) $^

$(BUILDPATH)/%.o : $(SRCPATH)/%.c
	$(CC) $(CFLAGS) -o $@ $^

install : $(OUTFILE)
	cp "$(OUTFILE)" "$(DESTDIR)/$(OUTFILE)"

uninstall :
	rm "$(DESTDIR)/$(OUTFILE)"

clean :
	rm $(BUILDPATH)/*
	rm $(OUTFILE)
