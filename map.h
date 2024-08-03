#ifndef _MAP_
#define _MAP_

#include "util.h" // u32

/*
 * WALL   0000 0001
 * BLOCK  0000 0010
 * PLAYER 0000 0100
 *
 * LIGHT  0001 0000
 * DARK   0010 0000
 *
 * 16x16 map
 */

u32 map[18][18] = {
    {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
    {  1, 18, 18, 18, 18, 18, 18, 18, 18, 34, 34, 34, 34, 34, 34, 34, 34,  1 },
    {  1, 18, 18, 18, 18, 18, 18, 18, 18, 34, 34, 34, 34, 34, 34, 34, 34,  1 },
    {  1, 18, 18, 18, 18, 18, 18, 18, 18, 34, 34, 34, 34, 34, 34, 34, 34,  1 },
    {  1, 18, 18, 18, 18, 18, 18, 18, 18, 34, 34, 34, 34, 34, 34, 34, 34,  1 },
    {  1, 18, 18, 18, 18, 18, 18, 18, 18, 34, 34, 34, 34, 34, 34, 34, 34,  1 },
    {  1, 18, 18, 18, 18, 18, 18, 18, 18, 34, 34, 34, 34, 34, 34, 34, 34,  1 },
    {  1, 18, 18, 18, 18, 18, 18, 18, 18, 34, 34, 34, 34, 34, 34, 34, 34,  1 },
    {  1, 36, 18, 18, 18, 18, 18, 18, 18, 34, 34, 34, 34, 34, 34, 34, 34,  1 },
    {  1, 18, 18, 18, 18, 18, 18, 18, 18, 34, 34, 34, 34, 34, 34, 34, 20,  1 },
    {  1, 18, 18, 18, 18, 18, 18, 18, 18, 34, 34, 34, 34, 34, 34, 34, 34,  1 },
    {  1, 18, 18, 18, 18, 18, 18, 18, 18, 34, 34, 34, 34, 34, 34, 34, 34,  1 },
    {  1, 18, 18, 18, 18, 18, 18, 18, 18, 34, 34, 34, 34, 34, 34, 34, 34,  1 },
    {  1, 18, 18, 18, 18, 18, 18, 18, 18, 34, 34, 34, 34, 34, 34, 34, 34,  1 },
    {  1, 18, 18, 18, 18, 18, 18, 18, 18, 34, 34, 34, 34, 34, 34, 34, 34,  1 },
    {  1, 18, 18, 18, 18, 18, 18, 18, 18, 34, 34, 34, 34, 34, 34, 34, 34,  1 },
    {  1, 18, 18, 18, 18, 18, 18, 18, 18, 34, 34, 34, 34, 34, 34, 34, 34,  1 },
    {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
};

#endif