/**
 * @file drlg_l4.h
 *
 * Interface of the hell level generation algorithms.
 */
#ifndef __DRLG_L4_H__
#define __DRLG_L4_H__

extern int diabquad1x;
extern int diabquad1y;
extern int diabquad2x;
extern int diabquad2y;
extern int diabquad3x;
extern int diabquad3y;
extern int diabquad4x;
extern int diabquad4y;
std::optional<uint32_t> CreateL4Dungeon(DWORD rseed, int entry, DungeonMode mode);
void DRLG_PreLoadDiabQuads();
void DRLG_FreeDiabQuads();

#endif /* __DRLG_L4_H__ */
