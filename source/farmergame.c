#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"
#include "player.h"
#include "map.h"
#include "inventory_weight.h"

// ----- Пункты главного меню -----------------------------------------------

enum MenuAction {
    MENU_EXIT          = 0,
    MENU_INVENTORY     = 1,
    MENU_PUT_ITEM      = 2,
    MENU_DROP_ITEM     = 3,
    MENU_UNIQUE        = 4,
    MENU_SEARCH        = 5,
    MENU_DIARY         = 6,
    MENU_ANALYZE_LOGS  = 7,
    MENU_ADVENTURE     = 8,
    MENU_CREATE_PLAYER = 9,
    MENU_PLAYER_CARD   = 10,
    MENU_TIME          = 11,
    MENU_BACKPACK      = 12
};

// ----- Пункты подменю времени ---------------------------------------------

enum TimeAction {
    TIME_BACK  = 0,
    TIME_SHOW  = 1,
    TIME_WAIT  = 2,
    TIME_SLEEP = 3
};

// ----- Названия предметов -------------------------------------------------

static void load_item_names(char item_names[][MAX_NAME_LEN])
{
    const char *names[MAX_ITEMS] = {
        "пусто", "факел", "верёвка", "доспех", "свиток",
        "тяжёлый камень", "зелье", "булыжник", "еда", "слиток"
    };
    for (int i = 0; i < MAX_ITEMS; i++) {
        strncpy(item_names[i], names[i], MAX_NAME_LEN - 1);
        item_names[i][MAX_NAME_LEN - 1] = '\0';
    }
}

// ----- Подменю времени ----------------------------------------------------

static void time_menu(int *day, int *hour, struct Player *player)
{
    int choice = read_int_safe(
        "[1] Посмотреть время\n[2] Ждать\n[3] Лечь спать\n[0] Назад\nВыбор: ",
        0, 3);

    switch ((enum TimeAction)choice) {
    case TIME_SHOW:
        printf("Текущее время: день %d, час %d:00\n", *day, *hour);
        break;

    case TIME_WAIT: {
        int wait = read_int_safe("Сколько часов ждать (1..24): ", 1, HOURS_PER_DAY);
        int target = *hour + wait;
        if (*hour < NIGHT_HOUR && target > NIGHT_HOUR) {
            printf("Нельзя пройти границу 02:00 без сна!\n");
            return;
        }
        *hour += wait;
        while (*hour >= HOURS_PER_DAY) { *hour -= HOURS_PER_DAY; (*day)++; }
        printf("Время: день %d, час %d:00\n", *day, *hour);
        break;
    }

    case TIME_SLEEP:
        if (!player) { printf("Сначала создайте персонажа!\n"); return; }
        (*day)++;
        *hour = SLEEP_HOUR;
        rest_player(player);
        printf("Новый день: день %d, час %d:00\n", *day, *hour);
        break;

    case TIME_BACK:
    default:
        break;
    }
}

// ----- Подменю рюкзака ----------------------------------------------------

static void backpack_menu(struct Player *player,
                          char item_names[][MAX_NAME_LEN])
{
    if (!player) { printf("Сначала создайте персонажа!\n"); return; }

    enum { BP_BACK = 0, BP_SHOW = 1, BP_BUY = 2 };

    int sub = read_int_safe(
        "[1] Показать рюкзак\n"
        "[2] Купить подсумок (+10 ёмкости за 50 золота)\n"
        "[0] Назад\nВыбор: ", 0, 2);

    switch (sub) {
    case BP_SHOW: backpack_show(&player->backpack, item_names); break;
    case BP_BUY:  backpack_buy_pouch(&player->backpack, &player->gold); break;
    case BP_BACK:
    default: break;
    }
}

// ----- main ---------------------------------------------------------------

int main(void)
{
    srand((unsigned)time(NULL));

    char item_names[MAX_ITEMS][MAX_NAME_LEN];
    load_item_names(item_names);

    struct Player *player = NULL;
    int day = 1, hour = 8;

    while (1) {
        printf("\nВыбери пункт:\n");
        printf("[%d]  Выход\n",                       MENU_EXIT);           //0
        printf("[%d]  Посмотреть инвентарь\n",        MENU_INVENTORY);      //1
        printf("[%d]  Положить предмет\n",            MENU_PUT_ITEM);       //2
        printf("[%d]  Выбросить предмет\n",           MENU_DROP_ITEM);      //3
        printf("[%d]  Уникальные находки\n",          MENU_UNIQUE);         //4
        printf("[%d]  Поиск предмета\n",              MENU_SEARCH);         //5
        printf("[%d]  Запись в дневник\n",            MENU_DIARY);          //6
        printf("[%d]  Анализ логов\n",                MENU_ANALYZE_LOGS);   //7
        printf("[%d] Отправиться в приключение\n",    MENU_ADVENTURE);      //8
        printf("[%d] Создать/пересоздать персонажа\n", MENU_CREATE_PLAYER); //9
        printf("[%d] Карточка персонажа\n",           MENU_PLAYER_CARD);    //10
        printf("[%d] Управление временем\n",          MENU_TIME);           //11
        printf("[%d] Рюкзак / купить подсумок\n",     MENU_BACKPACK);       //12

        int userAction = read_int_safe("Выбор: ", MENU_EXIT, MENU_BACKPACK);

        switch ((enum MenuAction)userAction) {
        case MENU_EXIT:
            free_player(player);
            printf("Пока-пока!\n");
            return 0;

        case MENU_INVENTORY:     show_inventory(player, item_names); break;
        case MENU_PUT_ITEM:      put_item(player); break;
        case MENU_DROP_ITEM:     drop_item(player); break;
        case MENU_UNIQUE:        unique_items(player, item_names); break;
        case MENU_SEARCH:        search_item(player, item_names); break;
        case MENU_DIARY:         save_diary(player, day, hour, item_names); break;
        case MENU_ANALYZE_LOGS:  analyze_logs(); break;
        case MENU_ADVENTURE:     run_adventure(player, &day, &hour); break;

        case MENU_CREATE_PLAYER:
            if (player != NULL) {
                int confirm = read_int_safe(
                    "Удалить текущего персонажа? (1-да, 0-нет): ", 0, 1);
                if (!confirm) break;
                free_player(player);
            }
            player = create_player(item_names);
            break;

        case MENU_PLAYER_CARD:
            show_player_card(player, day, hour, item_names);
            break;

        case MENU_TIME:
            time_menu(&day, &hour, player);
            break;

        case MENU_BACKPACK:
            backpack_menu(player, item_names);
            break;

        default:
            printf("Неверный ввод\n");
            break;
        }
    }
}
