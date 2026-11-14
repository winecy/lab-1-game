#ifndef MAP_H
#define MAP_H

#include "player.h"

// размеры карт-------------------------------------------------------------

#define MIN_FOREST_SIZE  8
#define MAX_FOREST_SIZE  20
#define MIN_CAVE_SIZE    5
#define MAX_CAVE_SIZE    8

// символы клеток-----------------------------------------------------------

#define CELL_FLOOR '.'
#define CELL_WALL  '#'
#define CELL_TREE  'T'
#define CELL_ROCK  'R'
#define CELL_HERO  '@'
#define CELL_CAMP  '^'
#define CELL_CAVE  'O'

// перечисление типов локаций-----------------------------------------------

enum LocationType {
    LOC_FOREST = 0,
    LOC_CAVE   = 1
};

// перечисление типов клеток-----------------------------------------------

enum CellType {
    CELLT_FLOOR = 0,
    CELLT_WALL,
    CELLT_TREE,
    CELLT_ROCK,
    CELLT_CAMP,
    CELLT_CAVE_ENTRANCE,
    CELLT_HERO
};

// перечисление результатов шага-------------------------------------------

enum MoveResult {
    MOVE_BLOCKED        = 0,
    MOVE_OK             = 1,
    MOVE_NEW_FOREST     = 2,
    MOVE_CAMP           = 3,
    MOVE_CAVE_ENTRANCE  = 4
};

// структура карты----------------------------------------------------------

struct Map {
    char **cells;                 // двумерный динамический массив
    int    width;
    int    height;
    int    floors_passed;         // счётчик пройденных этажей/локаций
    enum LocationType type;
    int    campfire_x;
    int    campfire_y;
};

// прототипы (реализация — map.c)-------------------------------------------

struct Map *create_map(int h, int w);
void free_map(struct Map *m);
void generate_forest(struct Map *m, struct Player *player);
void generate_cave(struct Map *m, struct Player *player);
void print_map(struct Map *m);
int  move_hero(struct Map *m, struct Player *player, int dx, int dy);
void run_adventure(struct Player *player, int *day, int *hour);

#endif // MAP_H
