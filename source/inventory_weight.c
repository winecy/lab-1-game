#include <stdio.h>
#include <string.h>

#include "inventory_weight.h"
#include "utils.h"

// таблица весов предметов (индекс = id)------------------------------------

static const int weights[MAX_ITEMS] = {
    /* 0 Пусто     */  0,
    /* 1 Дерево    */  3,
    /* 2 Камень    */  8,
    /* 3 Семена    */  1,
    /* 4 Картошка  */  1,
    /* 5 Кирка     */ 10,
    /* 6 Пшено     */  2,
    /* 7 Грибы     */  1,
    /* 8 Седло     */ 15,
    /* 9 Морковь   */  1
};

// вес предмета по id-------------------------------------------------------

int inventory_weight(int item_id)
{
    if (item_id < 0 || item_id >= MAX_ITEMS) return 0;
    return weights[item_id];
}

// инициализация рюкзака----------------------------------------------------

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

// статус загрузки----------------------------------------------------------

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

// пересчёт веса и статуса--------------------------------------------------

void backpack_recalc(struct Backpack *bp)
{
    if (bp == NULL) return;
    bp->current_weight = 0;
    for (int i = 0; i < bp->count; i++)
        bp->current_weight += inventory_weight(bp->slots[i]);
    bp->state = backpack_get_state(bp->current_weight, bp->max_capacity);
}

// добавление предмета------------------------------------------------------

int backpack_add(struct Backpack *bp, int item_id)
{
    if (bp == NULL) return -1;
    if (item_id < 0 || item_id >= MAX_ITEMS) return -1;
    if (bp->count >= MAX_SLOTS) return -1;
    bp->slots[bp->count++] = item_id;
    backpack_recalc(bp);
    return 0;
}

// удаление предмета по индексу слота---------------------------------------

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

// показ рюкзака------------------------------------------------------------

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

// покупка подсумка (+10 ёмкости за 50 золота)------------------------------

void backpack_buy_pouch(struct Backpack *bp, int *gold)
{
    if (bp == NULL || gold == NULL) { printf("Нет персонажа!\n"); return; }
    if (*gold < POUCH_COST) {
        printf("Не хватает золота (нужно %d, есть %d).\n", POUCH_COST, *gold);
        return;
    }
    *gold -= POUCH_COST;
    bp->max_capacity += POUCH_CAPACITY;
    backpack_recalc(bp);
    printf("Куплен подсумок. Грузоподъёмность: %d\n", bp->max_capacity);
}

// стоимость шага по выносливости (3 при Перегрузе, иначе 1)---------------

int step_stamina_cost(const struct Backpack *bp)
{
    if (bp == NULL) return 1;
    return (bp->state == LOAD_OVERLOAD) ? 3 : 1;
}
