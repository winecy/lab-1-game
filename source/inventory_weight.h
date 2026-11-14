#ifndef INVENTORY_WEIGHT_H
#define INVENTORY_WEIGHT_H

#include "utils.h"

// состояние загрузки-------------------------------------------------------

enum LoadState {
    LOAD_LIGHT = 0,   // Налегке
    LOAD_NORMAL,      // Норма
    LOAD_OVERLOAD     // Перегруз
};

// константы рюкзака--------------------------------------------------------

#define MAX_SLOTS      MAX_ITEMS
#define BASE_CAPACITY  30
#define POUCH_CAPACITY 10
#define POUCH_COST     50

// структура рюкзака--------------------------------------------------------

struct Backpack {
    int  slots[MAX_SLOTS];      // id предметов, -1 = пусто
    int  count;                 // сколько слотов занято
    int  current_weight;        // текущий общий вес
    int  max_capacity;          // максимальная грузоподъёмность
    enum LoadState state;       // статус загрузки
};

// прототипы (реализация — inventory_weight.c)------------------------------

void  backpack_init(struct Backpack *bp);
int   inventory_weight(int item_id);
void  backpack_recalc(struct Backpack *bp);
enum LoadState backpack_get_state(int weight, int capacity);
const char *load_state_name(enum LoadState s);

int   backpack_add(struct Backpack *bp, int item_id);
int   backpack_remove(struct Backpack *bp, int slot);
void  backpack_show(const struct Backpack *bp, char item_names[][MAX_NAME_LEN]);
void  backpack_buy_pouch(struct Backpack *bp, int *gold);

int   step_stamina_cost(const struct Backpack *bp);

#endif // INVENTORY_WEIGHT_H
