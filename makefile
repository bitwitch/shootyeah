PROG     := game
CFLAGS   := -Wall -std=c99 -O2 -I./include $(shell sdl2-config --cflags --libs)
DBGFLAGS := -Wall -std=c99 -ggdb -Og -I./include $(shell sdl2-config --cflags --libs)
LFLAGS   := -lm
SOURCES  := $(wildcard ./src/*.c)

WASM_CFLAGS      := -Wall -std=c99 -O2 -I./include -DBUILD_MODE_WASM
WASM_OUTPUT_DIR  := ./web

.PHONY: $(PROG)

$(PROG):
	gcc -o $(PROG) $(SOURCES) $(LFLAGS) $(CFLAGS)

debug:
	gcc -o game.debug $(SOURCES) $(LFLAGS) $(DBGFLAGS)

wasm: $(SOURCES)
	emcc $(SOURCES) $(WASM_CFLAGS) -s WASM=1 -s USE_SDL=2 --preload-file assets -o $(WASM_OUTPUT_DIR)/index.js -s ASYNCIFY
