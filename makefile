PROG     := game
CFLAGS   := -Wall -std=c99 -O2 -I./include $(shell sdl2-config --cflags --libs)
DBGFLAGS := -Wall -std=c99 -ggdb -Og -I./include $(shell sdl2-config --cflags --libs)
LFLAGS   := -lm
SOURCES  := $(wildcard ./src/*.c)

.PHONY: $(PROG)

$(PROG):
	gcc -o $(PROG) $(SOURCES) $(LFLAGS) $(CFLAGS)

debug:
	gcc -o game.debug $(SOURCES) $(LFLAGS) $(DBGFLAGS)
