# Costume-Conundrum

## Build pre-reqs

Download raylib https://github.com/raysan5/raylib/releases/tag/5.5

unzip the `include` and `lib` folders into their respective coutnerparts here

## Build for windows/linux

`make`

## Build for web

Raylib must be compiled with emscripten, producing the libraylib.a file. This should be compiled for your arch and placed in libwasm folder

`make web`