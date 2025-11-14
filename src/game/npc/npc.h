/* npc.h
 * Every sprite that isn't Dot or a kid.
 */
 
#ifndef NPC_H
#define NPC_H

// type NPC defined in map.h
// type FrameTimer defined in frame_timer.h

int get_npcs(NPC **v);

/* (decalid) isn't just the face we show; it also drives specific exceptions.
 * Think animation, position offset, any other behavior.
 * (x,y) in meters, from the poi.
 */
NPC *init_npc(int x,int y,int decalid);
void sort_npcs();

void update_npc(NPC *npc,FrameTimer *ftimer);
void draw_npc(NPC *npc);
void reap_defunct_npcs();
void clear_all_npcs();

#endif
