#ifndef PLAYER_H
#define PLAYER_H

#include "utils.h"

// перечисление пунктов меню------------------------------------------------

enum MenuAction {
    MENU_EXIT          = 0,
    MENU_TIME          = 1,
    MENU_WAIT          = 2,
    MENU_SHOW_INV      = 3,
    MENU_PUT_ITEM      = 4,
    MENU_DROP_ITEM     = 5,
    MENU_UNIQUE        = 6,
    MENU_SEARCH        = 7,
    MENU_DIARY         = 8,
    MENU_LOGS          = 9,
    MENU_ADVENTURE     = 10,
    MENU_CREATE_PLAYER = 11,
    MENU_PLAYER_CARD   = 12,
    MENU_TIME_CONTROL  = 13
};

// структура игрока---------------------------------------------------------

struct Player {
    char name[PLAYER_NAME_MAX];
    int  hp;
    int  max_hp;
    int  stamina;
    int  max_stamina;
    int  gold;
    int  x, y;                          // координаты на карте
    int  inventory[INVENTORY_SIZE];     // id предметов
};

// прототипы (реализация — player.c в коммите 2)----------------------------

struct Player *create_player(char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void free_player(struct Player *player);
void show_player_card(struct Player *player, int day, int hour,
                      char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void show_inventory(struct Player *player,
                    char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void put_item(struct Player *player);
void drop_item(struct Player *player);
void unique_items(struct Player *player,
                  char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void search_item(struct Player *player,
                 char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void save_diary(struct Player *player, int day, int hour,
                char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void rest_player(struct Player *player);

#endif // PLAYER_H