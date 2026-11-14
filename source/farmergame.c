// библиотеки---------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "utils.h"
#include "player.h"
#include "map.h"

// управление временем------------------------------------------------------

void time_menu(int *day, int *hour, struct Player *player)
{
    int choice = read_int_safe(
        "[1] Посмотреть время\n[2] Ждать\n[3] Лечь спать\n[0] Назад\nВыбор: ",
        0, 3);

    if (choice == 1)
    {
        printf("Текущее время: день %d, час %d:00\n", *day, *hour);
    }
    else if (choice == 2)
    {
        int wait = read_int_safe("Сколько часов ждать (1..24): ", 1, HOURS_PER_DAY);
        int target = *hour + wait;
        if (*hour < NIGHT_HOUR && target > NIGHT_HOUR)
        {
            printf("Нельзя пройти границу 02:00 без сна!\n");
            return;
        }
        *hour += wait;
        while (*hour >= HOURS_PER_DAY) { *hour -= HOURS_PER_DAY; (*day)++; }
        printf("Время: день %d, час %d:00\n", *day, *hour);
    }
    else if (choice == 3)
    {
        if (player == NULL) { printf("Сначала создайте персонажа!\n"); return; }
        (*day)++;
        *hour = SLEEP_HOUR;
        rest_player(player);
        printf("Новый день: день %d, час %d:00\n", *day, *hour);
    }
}

// главаная функция-----------------------------------------------------------------

int main()
{
    srand((unsigned)time(NULL));

    char item_names[MAX_ITEMS][MAX_NAME_LEN];
    load_item_names(item_names);

    struct Player *player = NULL;
    int day = 1, hour = 8;
    int userAction;

    while (1)    // меню
    {
        printf("Выбери пункт:\n");
        printf("[0] Выход\n");
        printf("[3] Посмотреть инвентарь\n");
        printf("[4] Положить предмет в слот\n");
        printf("[5] Выбросить предмет\n");
        printf("[6] Уникальные находки\n");
        printf("[7] Поиск предмета по названию\n");
        printf("[8] Запись в дневник\n");
        printf("[9] Анализ логов\n");
        printf("[10] Отправиться в приключение\n");
        printf("[11] Создать / пересоздать персонажа\n");
        printf("[12] Карточка персонажа\n");
        printf("[13] Управление временем\n");

        userAction = read_int_safe("Выбор: ", 0, 13);

        switch (userAction)
        {
        case 0:
            free_player(player);
            printf("Пока-пока!\n");
            return 0;
        case 3:  show_inventory(player, item_names); break;
        case 4:  put_item(player); break;
        case 5:  drop_item(player); break;
        case 6:  unique_items(player, item_names); break;
        case 7:  search_item(player, item_names); break;
        case 8:  save_diary(player, day, hour, item_names); break;
        case 9:  analyze_logs(); break;
        case 10: run_adventure(player, &day, &hour); break;

        case 11:
            if (player != NULL)
            {
                int confirm = read_int_safe(
                    "Удалить текущего персонажа? (1-да, 0-нет): ", 0, 1);
                if (!confirm) break;
                free_player(player);
            }
            player = create_player(item_names);
            break;

        case 12:
            show_player_card(player, day, hour, item_names);
            break;

        case 13:
            time_menu(&day, &hour, player);
            break;

        default:
            printf("Неверный ввод\n");
            break;
        }
    }
}