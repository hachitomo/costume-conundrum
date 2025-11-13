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
#define NS_sys_bgcolor 0x4e6d8e

#define EGGDEV_ignoreData "*.c,*.bak"

#define CMD_map_hero       0x20 /* u16:position */
#define CMD_map_ghost      0x21 /* u16:position */
#define CMD_map_princess   0x22 /* u16:position */
#define CMD_map_pumpkin    0x23 /* u16:position ; The kid dressed up as a pumpkin, not an actual pumpkin. Do not use for pie. */
#define CMD_map_robot      0x24 /* u16:position */
#define CMD_map_clown      0x25 /* u16:position */
#define CMD_map_lightbear  0x26 /* u16:position */
#define CMD_map_cat        0x27 /* u16:position */
#define CMD_map_jack       0x28 /* u16:position ; "O'Lantern", the small decoration. */
#define CMD_map_pumpkinhat 0x29 /* u16:position */

#define NS_physics_vacant 0
#define NS_physics_solid 1
#define NS_physics_oneway 2

/* Names of decals.
 */
#define NS_decal_dot_walk1 1 /* 1..8 */
#define NS_decal_dot_walk2 2
#define NS_decal_dot_walk3 3
#define NS_decal_dot_walk4 4
#define NS_decal_dot_walk5 5
#define NS_decal_dot_walk6 6
#define NS_decal_dot_walk7 7
#define NS_decal_dot_walk8 8
#define NS_decal_dot_idle 9
#define NS_decal_dot_blink 10
#define NS_decal_dot_arm 11
#define NS_decal_dot_jump1 12 /* 1,2 */
#define NS_decal_dot_fall1 13 /* 1,2 */
#define NS_decal_crown 14
#define NS_decal_ghost_wrong 15
#define NS_decal_ghost_right 16
#define NS_decal_ghost_walk1 17 /* 1,2,3,4 */
#define NS_decal_ghost_walk2 18
#define NS_decal_ghost_walk3 19
#define NS_decal_ghost_walk4 20
#define NS_decal_princess_wrong 21
#define NS_decal_princess_right 22
#define NS_decal_princess_walk1 23 /* 1,2,3,4 */
#define NS_decal_princess_walk2 24
#define NS_decal_princess_walk3 25
#define NS_decal_princess_walk4 26
#define NS_decal_pumpkin_wrong 27
#define NS_decal_pumpkin_right 28
#define NS_decal_pumpkin_walk1 29 /* walk1,walk2,**walk1**,walk3 */
#define NS_decal_pumpkin_walk2 30
#define NS_decal_pumpkin_walk3 31
#define NS_decal_robot_wrong 32
#define NS_decal_robot_right 33
#define NS_decal_robot_walk1 34 /* walk1,walk2,**walk1**,walk3 */
#define NS_decal_robot_walk2 35
#define NS_decal_robot_walk3 36
#define NS_decal_clown_wrong 37
#define NS_decal_clown_right 38
#define NS_decal_clown_walk1 39 /* 1,2,3,4 */
#define NS_decal_clown_walk2 40
#define NS_decal_clown_walk3 41
#define NS_decal_clown_walk4 42
#define NS_decal_dot_jump2 43
#define NS_decal_dot_fall2 44
#define NS_decal_ghost_query 45
#define NS_decal_princess_query 46
#define NS_decal_pumpkin_query 47
#define NS_decal_lightbear_dig1 48
#define NS_decal_lightbear_dig2 49
#define NS_decal_lightbear_eat1 50
#define NS_decal_lightbear_eat2 51
#define NS_decal_pumpkinhat 52
#define NS_decal_robotmask 53
#define NS_decal_clownmask 54
#define NS_decal_jack1 55
#define NS_decal_jack2 56
#define NS_decal_cat1 57
#define NS_decal_cat2 58
#define NS_decal_cat3 59
#define NS_decal_cat4 60
#define NS_decal_ghost_query_up 61

#endif
