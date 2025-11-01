all:
.SILENT:
PRECMD=echo "  $@" ; mkdir -p $(@D) ;

EGG_SDK:=../egg2

SRCFILES:=$(shell find src -type f)

edit:;$(EGG_SDK)/out/eggdev serve --htdocs=/data:src/data --htdocs=EGG_SDK/src/web --htdocs=EGG_SDK/src/editor --writeable=src/data

# This builds the slicer. Running it is manual only, and its output doesn't get cleaned.
SLICER_CFILES:=$(filter src/tool/slicer/%.c,$(SRCFILES))
SLICER_OFILES:=$(patsubst src/%.c,mid/%.o,$(SLICER_CFILES))
-include $(SLICER_OFILES:.o=.d)
mid/tool/slicer/%.o:src/tool/slicer/%.c;$(PRECMD) gcc -c -MMD -DUSE_mswin=1 -O3 -Isrc -o$@ $<
SLICER_EXE:=out/slicer
$(SLICER_EXE):$(SLICER_OFILES);$(PRECMD) gcc -o$@ $^ -lz

clean:;rm -rf mid out

GAME_CFILES:=$(filter src/game/%.c,$(SRCFILES))
GAME_OFILES:=$(patsubst src/%.c,mid/windows/%.o,$(GAME_CFILES))
-include $(GAME_OFILES:.o=.d)
mid/windows/game/%.o:src/game/%.c;$(PRECMD) gcc -c -MMD -DUSE_mswin=1 -O3 -Isrc -Iinclude -o$@ $<
GAME_EXE:=out/costume-conundrum.exe
$(info $(GAME_OFILES))
$(GAME_EXE):$(GAME_OFILES);$(PRECMD) gcc -mwindows -Wl,-static -o$@ $^ -L lib -lraylib -lgdi32 -lwinmm
all:$(GAME_EXE)

GAME_WASMCFILES:=$(filter src/game/%.c,$(SRCFILES))
GAME_WASMOFILES:=$(patsubst src/%.c,mid/wasm/%.o,$(GAME_WASMCFILES))