# Costume-Conundrum

## Build pre-reqs

Download raylib https://github.com/raysan5/raylib/releases/tag/5.5

unzip the `include` and `lib` folders into their respective directories here

## Build for windows/linux

Just run `make` from the root directory.
MinGW64 is required for windows.

## Build for web

Raylib must be compiled with emscripten, producing the libraylib.a file. This should be compiled for your arch and placed in libwasm folder. 

After that, just run `make web` which will produce an html page, a js file, and a wasm file. Open the html file in a browser to run the game.

## Licensing

The code, images, and sound effects in this project are licensed under the GNU GPLv3. This includes everything in the project except the music in src/data/song, which is licensed by Aster Kanke under the CC-BY-NC-ND 4.0 license.