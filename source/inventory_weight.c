#include <stdio.h>
#include <string.h>

#include "inventory_weight.h"
#include "utils.h"

// Таблица весов: РОВНО MAX_ITEMS элементов (у тебя 10 → индексы 0..9)
static const int weights[MAX_ITEMS] = {
    /* 0 */  0,   // пустой слот
    /* 1 */  5,   // факел
    /* 2 */  2,   // верёвка
    /* 3 */ 15,   // доспех
    /* 4 */  1,   // свиток
    /* 5 */ 25,   // тяжёлый камень
    /* 6 */  3,   // зелье
    /* 7 */ 40,   // булыжник
    /* 8 */  2,   // еда
    /* 9 */ 30    // слиток
};

int inventory_weight(int item_id)
{
    if (item_id < 0 || item_id >= MAX_ITEMS) return 0;
    return weights[item_id];
}
