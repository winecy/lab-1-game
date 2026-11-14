// библиотеки---------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "utils.h"
#include "player.h"
#include "map.h"

// переменные---------------------------------------------------------------

int current_day = 1;
int current_hour = 8;
int userAction;

int map_width  = 0;
int map_height = 0;
int hero_x = 0;
int hero_y = 0;
int location_type = LOC_FOREST;
int distance_from_home = 0;
int campfire_x = -1;
int campfire_y = -1;

char **map = NULL;
char item_names[MAX_ITEMS][MAX_NAME_LEN];


// функции(меню)--------------------------------------------------------------

void case1_time()
{
  printf("Текущее время: %d день %d час\n", current_day, current_hour);
}


void case2_work()
{
  int work_time;
  printf("Сколько работаем?\n");
  scanf("%d", &work_time);
  if (work_time>=0){
    current_hour += work_time;
    printf("Работаем...\n");
    if (current_hour >= 24) 
        {
        current_day += current_hour / hours_per_day;
        current_hour = current_hour % hours_per_day;
        }
  }else{
    printf("Такое невозможно. Введите корректное количество часов.\n");
  }
}

void create_map(int h, int w) {
    map_height = h;
    map_width  = w;

    map = malloc(h * sizeof(char *));
    if (map == NULL) return;

    for (int i = 0; i < h; i++) {
        map[i] = malloc(w * sizeof(char));
        if (map[i] == NULL) {
            for (int j = 0; j < i; j++) free(map[j]);
            free(map);
            map = NULL;
            return;
        }
    }
}

void free_map() {
    if (map == NULL) return;
    for (int i = 0; i < map_height; i++) {
        free(map[i]);
    }
    free(map);
    map = NULL;
    map_width  = 0;
    map_height = 0;
}

void generate_forest() {
    int h = MIN_FOREST_SIZE + rand() % (MAX_FOREST_SIZE - MIN_FOREST_SIZE + 1);
    int w = MIN_FOREST_SIZE + rand() % (MAX_FOREST_SIZE - MIN_FOREST_SIZE + 1);

    create_map(h, w);
    location_type = LOC_FOREST;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (rand() % 100 < 20)
                map[y][x] = (rand() % 2 == 0) ? CELL_TREE : CELL_ROCK;
            else
                map[y][x] = CELL_FLOOR;
        }
    }

    int hx = w / 2, hy = h / 2;
    if (map[hy][hx] != CELL_FLOOR) {
        int found = 0;
        for (int y = 0; y < h && !found; y++)
            for (int x = 0; x < w && !found; x++)
                if (map[y][x] == CELL_FLOOR) { hx = x; hy = y; found = 1; }
        if (!found) map[hy][hx] = CELL_FLOOR;
    }
    map[hy][hx] = CELL_HERO;
    hero_x = hx;
    hero_y = hy;

    campfire_x = -1;
    campfire_y = -1;
    for (int attempt = 0; attempt < 200 && campfire_x == -1; attempt++) {
        int cx = rand() % w;
        int cy = rand() % h;
        if (map[cy][cx] == CELL_FLOOR) {
            map[cy][cx] = CELL_CAMP;
            campfire_x = cx;
            campfire_y = cy;
        }
    }
    if (campfire_x == -1) {
        for (int y = 0; y < h && campfire_x == -1; y++)
            for (int x = 0; x < w; x++)
                if (map[y][x] == CELL_FLOOR) {
                    map[y][x] = CELL_CAMP;
                    campfire_x = x;
                    campfire_y = y;
                    break;
                }
    }

    int caves = 1 + rand() % 2;
    for (int i = 0; i < caves; i++) {
        for (int attempt = 0; attempt < 200; attempt++) {
            int ox = rand() % w;
            int oy = rand() % h;
            if (map[oy][ox] == CELL_FLOOR) {
                map[oy][ox] = CELL_CAVE;
                break;
            }
        }
    }
}

void generate_cave() {
    int h = MIN_CAVE_SIZE + rand() % (MAX_CAVE_SIZE - MIN_CAVE_SIZE + 1);
    int w = MIN_CAVE_SIZE + rand() % (MAX_CAVE_SIZE - MIN_CAVE_SIZE + 1);

    create_map(h, w);
    location_type = LOC_CAVE;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (x == 0 || y == 0 || x == w - 1 || y == h - 1)
                map[y][x] = CELL_WALL;
            else if (rand() % 100 < 25)
                map[y][x] = CELL_ROCK;
            else
                map[y][x] = CELL_FLOOR;
        }
    }

    int free_count = 0;
    for (int y = 1; y < h - 1; y++)
        for (int x = 1; x < w - 1; x++)
            if (map[y][x] == CELL_FLOOR) free_count++;
    for (int y = 1; y < h - 1 && free_count < 2; y++)
        for (int x = 1; x < w - 1 && free_count < 2; x++)
            if (map[y][x] != CELL_FLOOR) {
                map[y][x] = CELL_FLOOR;
                free_count++;
            }

    int hx = -1, hy = -1;
    for (int y = 1; y < h - 1 && hx == -1; y++)
        for (int x = 1; x < w - 1; x++)
            if (map[y][x] == CELL_FLOOR) { hx = x; hy = y; break; }
    map[hy][hx] = CELL_HERO;
    hero_x = hx;
    hero_y = hy;

    int best = -1, ox = -1, oy = -1;
    for (int y = 1; y < h - 1; y++)
        for (int x = 1; x < w - 1; x++)
            if (map[y][x] == CELL_FLOOR) {
                int d = (x - hx) * (x - hx) + (y - hy) * (y - hy);
                if (d > best) { best = d; ox = x; oy = y; }
            }
    if (ox == -1) {
        for (int y = 1; y < h - 1 && ox == -1; y++)
            for (int x = 1; x < w - 1; x++)
                if (!(x == hx && y == hy)) {
                    map[y][x] = CELL_FLOOR;
                    ox = x;
                    oy = y;
                    break;
                }
    }
    map[oy][ox] = CELL_CAVE;
}

void print_map() {
    printf("\n");
    for (int y = 0; y < map_height; y++) {
        for (int x = 0; x < map_width; x++)
            printf("%c", map[y][x]);
        printf("\n");
    }
}

int move_hero(int dx, int dy) {
    int new_x = hero_x + dx;
    int new_y = hero_y + dy;

    int out_x = (new_x < 0 || new_x >= map_width);
    int out_y = (new_y < 0 || new_y >= map_height);

    if (out_x || out_y) {
        if (location_type == LOC_FOREST) return 2;
        return 0;
    }

    char c = map[new_y][new_x];
    if (c == CELL_TREE || c == CELL_ROCK || c == CELL_WALL) return 0;

    if (location_type == LOC_FOREST &&
        hero_x == campfire_x && hero_y == campfire_y) {
        map[hero_y][hero_x] = CELL_CAMP;
    } else {
        map[hero_y][hero_x] = CELL_FLOOR;
    }

    hero_x = new_x;
    hero_y = new_y;

    if (c == CELL_CAVE) return 4;
    map[hero_y][hero_x] = CELL_HERO;
    if (c == CELL_CAMP) return 3;
    return 1;
}

void case10_adventure() {
    distance_from_home = 0;
    generate_forest();

    int running = 1;
    while (running) {
        print_map();

        printf("Ход > ");
        char cmd;
        if (scanf(" %c", &cmd) != 1) break;
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) { }
        cmd = (char)toupper((unsigned char)cmd);

        if (cmd == 'Q') {
            if (location_type == LOC_FOREST &&
                hero_x == campfire_x && hero_y == campfire_y) {
                printf("Вы вернулись домой! Пройдено лесов: %d\n", distance_from_home);
                running = 0;
            } else if (location_type == LOC_FOREST) {
                printf("Домой можно только стоя на костре ^.\n");
            } else {
                printf("Из пещеры нет прямого пути домой. Найдите O.\n");
            }
            continue;
        }

        int dx = 0, dy = 0;
        if      (cmd == 'W') dy = -1;
        else if (cmd == 'S') dy =  1;
        else if (cmd == 'A') dx = -1;
        else if (cmd == 'D') dx =  1;
        else { printf("Неизвестная команда.\n"); continue; }

        int result = move_hero(dx, dy);

        if (result == 0) {
            printf("Туда нельзя.\n");
        }
        else if (result == 2) {
            int old_x = hero_x, old_y = hero_y;
            free_map();
            distance_from_home++;
            generate_forest();

            int cx = hero_x, cy = hero_y;
            map[cy][cx] = CELL_FLOOR;

            int go_x = old_x, go_y = old_y;
            if (dy == -1) go_y = map_height - 1;
            if (dy ==  1) go_y = 0;
            if (dx == -1) go_x = map_width  - 1;
            if (dx ==  1) go_x = 0;

            if (go_x < 0) go_x = 0;
            if (go_x >= map_width)  go_x = map_width - 1;
            if (go_y < 0) go_y = 0;
            if (go_y >= map_height) go_y = map_height - 1;

            if (map[go_y][go_x] != CELL_FLOOR) {
                go_x = cx;
                go_y = cy;
                map[go_y][go_x] = CELL_FLOOR;
            }
            hero_x = go_x;
            hero_y = go_y;
            map[hero_y][hero_x] = CELL_HERO;

            printf("Вы ушли глубже в лес. Расстояние: %d\n", distance_from_home);
        }
        else if (result == 3) {
            printf("Вы у костра. Нажмите Q, чтобы вернуться домой.\n");
        }
        else if (result == 4) {
            int was_forest = (location_type == LOC_FOREST);
            free_map();
            if (was_forest) {
                generate_cave();
                printf("Вы вошли в пещеру...\n");
            } else {
                distance_from_home++;
                generate_forest();
                printf("Вы вышли из пещеры в новый лес. Расстояние: %d\n",
                       distance_from_home);
            }
        }
    }

    free_map();
}

// главаная функция-----------------------------------------------------------------

int main() 
{
  srand((unsigned)time(NULL));

  load_item_names(item_names);

  struct Player *player = NULL;
  int userAction;

  while (1)    // меню
  {
    printf("Выбери пункт:\n");
    printf("[0] Выход\n");
    printf("[1] Посмотреть на часы\n");
    printf("[2] Промотать время (Поработать)\n");
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

    userAction = read_int_safe("Выбор: ", 0, 12);

    switch (userAction)
    {
    case 0:
      free_player(player);
      printf("Пока-пока!\n");
      return 0;
    case 1:  case1_time(); break;
    case 2:  case2_work(); break;
    case 3:  show_inventory(player, item_names); break;
    case 4:  put_item(player); break;
    case 5:  drop_item(player); break;
    case 6:  unique_items(player, item_names); break;
    case 7:  search_item(player, item_names); break;
    case 8:  save_diary(player, current_day, current_hour, item_names); break;
    case 9:  analyze_logs(); break;
    case 10: case10_adventure(); break;

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
      show_player_card(player, current_day, current_hour, item_names);
      break;

    default:
      printf("Неверный ввод\n");
      break;
    }
  }
}