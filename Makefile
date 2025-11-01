all:
.SILENT:

EGG_SDK:=../egg2

edit:;$(EGG_SDK)/out/eggdev serve --htdocs=/data:src/data --htdocs=EGG_SDK/src/web --htdocs=EGG_SDK/src/editor --writeable=src/data
