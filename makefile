CC := gcc
CFLAGS := -O2 -c
LDFLAGS := -lncurses

SRCPATH := ./src
BUILDPATH := ./build

SOURCES := minesweeper.c
OBJECTS := $(addprefix $(BUILDPATH)/,$(SOURCES:%.c=%.o))
SOURCES := $(addprefix $(SRCPATH)/,$(SOURCES))

OUTFILE := minecurses

DESTDIR := /usr/bin

all : $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(OUTFILE) $^

$(BUILDPATH)/%.o : $(SRCPATH)/%.c
	$(CC) $(CFLAGS) -o $@ $^
