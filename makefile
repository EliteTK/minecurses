CC := gcc
CFLAGS := -O2 -c -Wall -Werror
LDFLAGS := -lncurses -lm

SRCPATH := ./src
BUILDPATH := ./build

SOURCES := minesweeper.c main.c
OBJECTS := $(addprefix $(BUILDPATH)/,$(SOURCES:%.c=%.o))
SOURCES := $(addprefix $(SRCPATH)/,$(SOURCES))

OUTFILE := minecurses

DESTDIR := /
PREFIX := /usr
BINDIR := /bin
INSPATH := $(DESTDIR)$(PREFIX)$(BINDIR)

$(OUTFILE) : $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(OUTFILE) $^

$(BUILDPATH)/%.o : $(SRCPATH)/%.c
	$(CC) $(CFLAGS) -o $@ $^

install : $(OUTFILE)
	mkdir -p "$(INSPATH)"
	cp "$(OUTFILE)" "$(INSPATH)/$(OUTFILE)"

uninstall :
	rm "$(INSPATH)/$(OUTFILE)"

clean :
	rm $(BUILDPATH)/*
	rm $(OUTFILE)
