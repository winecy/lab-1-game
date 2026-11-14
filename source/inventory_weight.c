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
void backpack_init(struct Backpack *bp)
{
    if (bp == NULL) return;
    memset(bp, 0, sizeof(*bp));
    for (int i = 0; i < MAX_SLOTS; i++)
        bp->slots[i] = -1;
    bp->count          = 0;
    bp->current_weight = 0;
    bp->max_capacity   = BASE_CAPACITY;
    bp->state          = LOAD_LIGHT;
}

// ----- Статус загрузки ----------------------------------------------------

enum LoadState backpack_get_state(int weight, int capacity)
{
    if (capacity <= 0) return LOAD_OVERLOAD;
    int pct = weight * 100 / capacity;
    if (pct < 50)   return LOAD_LIGHT;
    if (pct <= 100) return LOAD_NORMAL;
    return LOAD_OVERLOAD;
}

const char *load_state_name(enum LoadState s)
{
    switch (s) {
        case LOAD_LIGHT:    return "Налегке";
        case LOAD_NORMAL:   return "Норма";
        case LOAD_OVERLOAD: return "Перегруз";
    }
    return "?";
}

// ----- Пересчёт веса и статуса -------------------------------------------

void backpack_recalc(struct Backpack *bp)
{
    if (bp == NULL) return;
    bp->current_weight = 0;
    for (int i = 0; i < bp->count; i++)
        bp->current_weight += inventory_weight(bp->slots[i]);
    bp->state = backpack_get_state(bp->current_weight, bp->max_capacity);
}

// ----- Добавление / удаление ---------------------------------------------

int backpack_add(struct Backpack *bp, int item_id)
{
    if (bp == NULL) return -1;
    if (item_id < 0 || item_id >= MAX_ITEMS) return -1;
    if (bp->count >= MAX_SLOTS) return -1;
    bp->slots[bp->count++] = item_id;
    backpack_recalc(bp);
    return 0;
}

int backpack_remove(struct Backpack *bp, int slot)
{
    if (bp == NULL) return -1;
    if (slot < 0 || slot >= bp->count) return -1;
    for (int i = slot; i < bp->count - 1; i++)
        bp->slots[i] = bp->slots[i + 1];
    bp->slots[--bp->count] = -1;
    backpack_recalc(bp);
    return 0;
}

// ----- Показ --------------------------------------------------------------

void backpack_show(const struct Backpack *bp, char item_names[][MAX_NAME_LEN])
{
    if (bp == NULL) { printf("Рюкзак отсутствует.\n"); return; }
    printf("--- Рюкзак ---\n");
    printf("Вес: %d / %d  [%s]\n",
           bp->current_weight, bp->max_capacity,
           load_state_name(bp->state));
    if (bp->count == 0) { printf("  (пусто)\n"); return; }
    for (int i = 0; i < bp->count; i++) {
        int id = bp->slots[i];
        printf("  [%d] %s (%d ед.)\n",
               i, item_names[id], inventory_weight(id));
    }
}

// ----- Покупка подсумка ---------------------------------------------------

void backpack_buy_pouch(struct Backpack *bp, struct Player *player)
{
    if (bp == NULL || player == NULL) { printf("Нет персонажа!\n"); return; }
    int gold = player_gold(player);
    if (gold < POUCH_COST) {
        printf("Не хватает золота (нужно %d, есть %d).\n", POUCH_COST, gold);
        return;
    }
    player_add_gold(player, -POUCH_COST);
    bp->max_capacity += POUCH_CAPACITY;
    backpack_recalc(bp);
    printf("Куплен подсумок. Грузоподъёмность: %d\n", bp->max_capacity);
}

// ----- Стоимость шага по выносливости ------------------------------------

int step_stamina_cost(const struct Backpack *bp)
{
    if (bp == NULL) return 1;
    return (bp->state == LOAD_OVERLOAD) ? 3 : 1;
}
