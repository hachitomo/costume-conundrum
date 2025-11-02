/* shared_symbols.h
 * This file is a quirk of Egg Editor, in theory it defines symbols accessible to both game and editor.
 * Costume Conundrum is not using Egg, just Andy is using the Egg editor to make maps and decalsheets.
 * So this thing lives under src/game/ but it's not part of the build.
 */
 
#ifndef SHARED_SYMBOLS_H
#define SHARED_SYMBOLS_H

/* Background color for map editor.
 * At runtime, it's image:sky. This color is selected from about the middle of that pic.
 */
#define NS_sys_bgcolor 0x374456

#endif
