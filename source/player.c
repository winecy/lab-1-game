#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "player.h"
#include "inventory_weight.h"
#include "utils.h"

// создание игрока----------------------------------------------------------

struct Player *create_player(char item_names[MAX_ITEMS][MAX_NAME_LEN])
{
    (void)item_names;
    struct Player *p = (struct Player *)malloc(sizeof(struct Player));
    if (p == NULL)
    {
        printf("Ошибка: не удалось выделить память под игрока!\n");
        return NULL;
    }
    read_string_safe("Введите имя: ", p->name, PLAYER_NAME_MAX);

    p->hp          = START_HP;
    p->max_hp      = START_HP;
    p->stamina     = START_STAMINA;
    p->max_stamina = START_STAMINA;
    p->gold        = START_GOLD;
    p->x = 0;
    p->y = 0;

    backpack_init(&p->backpack);

    printf("Персонаж \"%s\" создан!\n", p->name);
    return p;
}

void free_player(struct Player *player)
{
    if (player == NULL) return;
    free(player);
}

// карточка-----------------------------------------------------------------

void show_player_card(struct Player *player, int day, int hour,
                      char item_names[MAX_ITEMS][MAX_NAME_LEN])
{
    if (player == NULL)
    {
        printf("Сначала создайте персонажа!\n");
        return;
    }
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timebuf[64];
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", t);

    printf("\n=== КАРТОЧКА ПЕРСОНАЖА ===\n");
    printf("Имя: %s\n", player->name);
    printf("HP: %d/%d\n", player->hp, player->max_hp);
    printf("Выносливость: %d/%d\n", player->stamina, player->max_stamina);
    printf("Золото: %d\n", player->gold);
    printf("Игровое время: день %d, час %d:00\n", day, hour);
    printf("Реальное время: %s\n", timebuf);

    backpack_show(&player->backpack, item_names);

    printf("===========================\n");
}

// инвентарь---------------------------------------------------------------

void show_inventory(struct Player *player,
                    char item_names[MAX_ITEMS][MAX_NAME_LEN])
{
    if (player == NULL) { printf("Сначала создайте персонажа!\n"); return; }
    backpack_show(&player->backpack, item_names);
}

void put_item(struct Player *player)
{
    if (player == NULL) { printf("Сначала создайте персонажа!\n"); return; }
    int id = read_int_safe("Выберите id предмета (0-9): ", 0, MAX_ITEMS - 1);
    if (backpack_add(&player->backpack, id) != 0)
        printf("Рюкзак переполнен!\n");
    else
        printf("Добавлено. Вес: %d/%d [%s]\n",
               player->backpack.current_weight,
               player->backpack.max_capacity,
               load_state_name(player->backpack.state));
}

void drop_item(struct Player *player)
{
    if (player == NULL) { printf("Сначала создайте персонажа!\n"); return; }
    if (player->backpack.count == 0) { printf("Рюкзак пуст.\n"); return; }
    int slot = read_int_safe("Выберите номер слота: ",
                             0, player->backpack.count - 1);
    backpack_remove(&player->backpack, slot);
    printf("Слот %d очищен. Вес: %d/%d [%s]\n",
           slot,
           player->backpack.current_weight,
           player->backpack.max_capacity,
           load_state_name(player->backpack.state));
}

void unique_items(struct Player *player,
                  char item_names[MAX_ITEMS][MAX_NAME_LEN])
{
    if (player == NULL) { printf("Сначала создайте персонажа!\n"); return; }
    int unique = 0;
    int items[MAX_ITEMS] = {0};
    for (int i = 0; i < player->backpack.count; i++)
    {
        int id = player->backpack.slots[i];
        if (id >= 0 && id < MAX_ITEMS) items[id]++;
    }
    for (int id = 1; id < MAX_ITEMS; id++)
    {
        if (items[id] > 0)
        {
            printf("Предмет ID %d (%s): %d штук\n", id, item_names[id], items[id]);
            unique++;
        }
    }
    printf("Всего уникальных предметов: %d\n", unique);
    if (unique == 0) printf("Инвентарь пуст.\n");
}

void search_item(struct Player *player,
                 char item_names[MAX_ITEMS][MAX_NAME_LEN])
{
    if (player == NULL) { printf("Сначала создайте персонажа!\n"); return; }
    char search_name[MAX_NAME_LEN];
    read_string_safe("Введите название предмета: ", search_name, MAX_NAME_LEN);

    int target_id = -1;
    for (int i = 0; i < MAX_ITEMS; i++)
        if (strcmp(item_names[i], search_name) == 0) { target_id = i; break; }

    if (target_id == -1)
    {
        printf("Предмет \"%s\" не найден.\n", search_name);
        return;
    }
    int found = 0;
    printf("Предмет \"%s\" (ID: %d) найден в слотах: ", search_name, target_id);
    for (int i = 0; i < player->backpack.count; i++)
        if (player->backpack.slots[i] == target_id) { printf("%d ", i); found = 1; }
    if (!found) printf("В рюкзаке этого нет.\n");
    else printf("\n");
}

void save_diary(struct Player *player, int day, int hour,
                char item_names[MAX_ITEMS][MAX_NAME_LEN])
{
    if (player == NULL) { printf("Сначала создайте персонажа!\n"); return; }
    FILE *file = fopen(DIARY_FILE, "a");
    if (file == NULL)
    {
        printf("Ошибка: не удалось открыть или создать файл %s!\n", DIARY_FILE);
        return;
    }
    fprintf(file, "=== Запись в дневнике ===\n");
    fprintf(file, "Фермер: %s\n", player->name);
    fprintf(file, "Время: День %d, Час %d:00\n", day, hour);
    fprintf(file, "Вес: %d/%d [%s]\n",
            player->backpack.current_weight,
            player->backpack.max_capacity,
            load_state_name(player->backpack.state));
    fprintf(file, "Содержимое инвентаря:\n");
    for (int i = 0; i < player->backpack.count; i++)
    {
        int id = player->backpack.slots[i];
        fprintf(file, "  Слот %d: [%d] %s\n", i, id, item_names[id]);
    }
    fprintf(file, "-------------------------\n\n");
    fclose(file);
    printf("Состояние успешно сохранено в дневник (%s).\n", DIARY_FILE);
}

void rest_player(struct Player *player)
{
    if (player == NULL) return;
    player->hp = player->max_hp;
    player->stamina = player->max_stamina;
    printf("Вы отдохнули! HP: %d/%d, Выносливость: %d/%d\n",
           player->hp, player->max_hp, player->stamina, player->max_stamina);
}
