PROG    := game
CFLAGS  := -Wall -std=c99 -Og -I./include $(shell sdl2-config --cflags --libs)
LFLAGS  := -L/usr/local/lib -lSDL2_mixer -lm
SOURCES := $(wildcard ./src/*.c)

.PHONY: $(PROG)

$(PROG):
	gcc -o $(PROG) $(SOURCES) $(LFLAGS) $(CFLAGS)

