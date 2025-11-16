# Costume-Conundrum

Play as Dot Vine the witch in Costume Conundrum!

Traverse the neighborhood in search of lost costume parts, so the kids can complete their costumes and get back to trick or treating. This game is a submission to LightBearSoftware's Uplifting Game Jam 2025

![alt text](etc/comic-920px.png)

Controls: 

Move left/right - A / D / D-pad / joy

Jump: Space / A (xbox) / X (PS)

Play in browser: https://hachitomo.itch.io/costume-conundrum

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

The code, images, and sound effects in this project are licensed under the GNU GPLv3. This includes everything in the project except the music in src/data/song and etc/music, which is licensed by Aster Kanke under the CC-BY-NC-ND 4.0 license.