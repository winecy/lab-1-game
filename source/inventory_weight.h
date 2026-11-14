#ifndef INVENTORY_WEIGHT_H          
#define INVENTORY_WEIGHT_H


enum LoadState {
    LOAD_LIGHT = 0,
    LOAD_NORMAL,
    LOAD_OVERLOAD
};

#define MAX_SLOTS      MAX_ITEMS
#define BASE_CAPACITY  30
#define POUCH_CAPACITY 10
#define POUCH_COST     50

struct Backpack {
    int  slots[MAX_SLOTS];
    int  count;
    int  current_weight;
    int  max_capacity;
    enum LoadState state;
};

void  backpack_init(struct Backpack *bp);
int   inventory_weight(int item_id);
void  backpack_recalc(struct Backpack *bp);
enum LoadState backpack_get_state(int weight, int capacity);
const char *load_state_name(enum LoadState s);

int   backpack_add(struct Backpack *bp, int item_id);
int   backpack_remove(struct Backpack *bp, int slot);
void  backpack_show(const struct Backpack *bp, char item_names[][MAX_NAME_LEN]);
void  backpack_buy_pouch(struct Backpack *bp, struct Player *player);

int   step_stamina_cost(const struct Backpack *bp);

#endif
