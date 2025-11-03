all:
.SILENT:
.SUFFIXES:
PRECMD=echo "  $@" ; mkdir -p $(@D) ;

EGG_SDK:=../egg2

SRCFILES:=$(shell find src -type f)

edit:;$(EGG_SDK)/out/eggdev serve --htdocs=/data:src/data --htdocs=EGG_SDK/src/web --htdocs=EGG_SDK/src/editor --writeable=src/data --project=.

assets:;mkdir -p out/data/image && cp src/data/image/* out/data/image
all:assets

# This builds the slicer. Running it is manual only, and its output doesn't get cleaned. XXX Not used.
SLICER_CFILES:=$(filter src/tool/slicer/%.c,$(SRCFILES))
SLICER_OFILES:=$(patsubst src/%.c,mid/%.o,$(SLICER_CFILES))
-include $(SLICER_OFILES:.o=.d)
mid/tool/slicer/%.o:src/tool/slicer/%.c;$(PRECMD) gcc -c -MMD -DUSE_mswin=1 -O3 -Isrc -o$@ $<
SLICER_EXE:=out/slicer
$(SLICER_EXE):$(SLICER_OFILES);$(PRECMD) gcc -o$@ $^ -lz

# Ah, but what we really need is a *dicer*. A tool to take our data files and produce compilable C files from them. Easiest way to pack assets.
DICER_CFILES:=$(filter src/tool/dicer/%.c,$(SRCFILES))
DICER_OFILES:=$(patsubst src/%.c,mid/%.o,$(DICER_CFILES))
-include $(DICER_OFILES:.o=.d)
mid/tool/dicer/%.o:src/tool/dicer/%.c;$(PRECMD) gcc -c -MMD -O3 -Isrc -o$@ $<
DICER_EXE:=out/dicer
$(DICER_EXE):$(DICER_OFILES);$(PRECMD) gcc -o$@ $^ -lz

# Then infer the dicer's output and roll those into SRCFILES.
# Violating my usual no-outputs-in-mid rule so we don't have to deal with inputs under mid.
DATA_FILES_SRC:=$(filter src/data/%,$(filter-out %.c,$(SRCFILES)))
DATA_FILES_C:=$(patsubst %,%.c,$(DATA_FILES_SRC))
src/data/%.c:src/data/% $(DICER_EXE);$(PRECMD) $(DICER_EXE) -o$@ $<
SRCFILES:=$(filter-out src/data/%,$(SRCFILES)) $(DATA_FILES_C)

# Andy's Linux build.
ifeq ($(USER),andy)
  RAYLIB_SDK:=../thirdparty/raylib-5.5_linux_amd64
  LINUX_CFILES:=$(filter src/game/%.c src/data/%.c,$(SRCFILES))
  LINUX_OFILES:=$(patsubst src/%.c,mid/linux/%.o,$(LINUX_CFILES))
  -include $(LINUX_OFILES:.o=.d)
  mid/linux/%.o:src/%.c;$(PRECMD) gcc -c -MMD -O3 -Isrc -I$(RAYLIB_SDK)/include -o$@ $<
  LINUX_EXE:=out/costume-conundrum-linux
  $(LINUX_EXE):$(LINUX_OFILES);$(PRECMD) gcc -o$@ $^ $(CFLAGS) $(RAYLIB_SDK)/lib/libraylib.a -lm
  all:$(LINUX_EXE)
  run:$(LINUX_EXE) assets;$(LINUX_EXE)
# Build with Raylib for Windows.
else
  GAME_CFILES:=$(filter src/game/%.c src/data/%.c,$(SRCFILES))
  GAME_OFILES:=$(patsubst src/%.c,mid/windows/%.o,$(GAME_CFILES))
  -include $(GAME_OFILES:.o=.d)
  mid/windows/game/%.o:src/game/%.c;$(PRECMD) gcc -c -MMD -DUSE_mswin=1 -O3 -Isrc -Iinclude -o$@ $<
  GAME_EXE:=out/costume-conundrum.exe
  $(GAME_EXE):$(GAME_OFILES);$(PRECMD) gcc -mwindows -Wl,-static -o$@ $^ -L lib -lraylib -lgdi32 -lwinmm
  all:$(GAME_EXE)
endif

GAME_WASMCFILES:=$(filter src/game/%.c src/data/%.c,$(SRCFILES))
WASM_EXE:=out/costume-conundrum.html
WASM_LIBA:=libwasm/libraylib.a
$(WASM_EXE):$(GAME_WASMCFILES);$(PRECMD) emcc -o$@ $^ -Os -Wall $(WASM_LIBA) -Isrc/game -Iinclude -L lib -lraylib -s USE_GLFW=3 --shell-file ../raylib/src/minshell.html -DPLATFORM_WEB
web:assets
web:$(WASM_EXE)

clean:;rm -rf mid out $(DATA_FILES_C)
