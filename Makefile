all:
.SILENT:
PRECMD=echo "  $@" ; mkdir -p $(@D) ;

EGG_SDK:=../egg2

SRCFILES:=$(shell find src -type f)

edit:;$(EGG_SDK)/out/eggdev serve --htdocs=/data:src/data --htdocs=EGG_SDK/src/web --htdocs=EGG_SDK/src/editor --writeable=src/data

# This builds the slicer. Running it is manual only, and its output doesn't get cleaned.
SLICER_CFILES:=$(filter src/tool/slicer/%.c,$(SRCFILES))
SLICER_OFILES:=$(patsubst src/%.c,mid/%.o,$(SLICER_CFILES))
mid/tool/slicer/%.o:src/tool/slicer/%.c;$(PRECMD) gcc -c -MMD -O3 -Isrc -o$@ $<
SLICER_EXE:=out/slicer
$(SLICER_EXE):$(SLICER_OFILES);$(PRECMD) gcc -o$@ $^ -lz
all:$(SLICER_EXE)

clean:;rm -rf mid out
