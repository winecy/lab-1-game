// библиотеки---------------------------------------------------------------

#include <stdio.h>  
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// константы----------------------------------------------------------------

#define inventory_size 10  // размер инвенторя
#define player_name_max 32 // максимальная длина имени фермера(юзера)
#define MAX_ITEMS 10       // максимальное количество предметов
#define MAX_NAME_LEN 32    // максимальная длина названия предмета
#define hours_per_day 24

#define ITEMS_FILE   "items.txt"
#define DIARY_FILE   "diary.txt"
#define INPUT_FILE   "input.txt"
#define OUTPUT_FILE  "output.txt"

#define MIN_FOREST_SIZE  8
#define MAX_FOREST_SIZE  20
#define MIN_CAVE_SIZE    5
#define MAX_CAVE_SIZE    8

#define LOC_FOREST  0
#define LOC_CAVE    1

#define CELL_FLOOR '.'
#define CELL_WALL  '#'
#define CELL_TREE  'T'
#define CELL_ROCK  'R'
#define CELL_HERO  '@'
#define CELL_CAMP  '^'
#define CELL_CAVE  'O'

// переменные---------------------------------------------------------------

int current_day = 1;    // стартовое значение дней
int current_hour = 8;   // стартовое значение часов
int userAction;         // выбор в меню
int work_time;          // длительность работы
int item_slot;          // слот предмета
int item_id;            // айди предмета

int map_width  = 0;
int map_height = 0;
int hero_x = 0;
int hero_y = 0;
int location_type = LOC_FOREST;
int distance_from_home = 0;
int campfire_x = -1;
int campfire_y = -1;

char **map = NULL;
char player_name[player_name_max];         // имя
char item_names[MAX_ITEMS][MAX_NAME_LEN];  // названия предметов


// массивы------------------------------------------------------------------

int inventory[inventory_size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};     // инвентарь 



// функции(меню)--------------------------------------------------------------

void case1_time()    // вывод времени
{
  printf("Текущее время: %d день %d час\n", current_day, current_hour);
}


void case2_work()    // работа
{
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


void case3_show_inventory()    // вывод инвентаря
{
  printf("Инвентарь:\n");
  for (int i = 0; i < inventory_size; i++) 
    {
    int id = inventory[i];
    
    if (id >= 0 && id < MAX_ITEMS) 
    {
        printf("Слот %d: [%d] - %s\n", i, id, item_names[id]);
    } 
    else 
    {
        printf("Слот %d: [%d] - неизвестный предмет.\n", i, id);
    }
  }
}


void case4_put_item()    // положить предмет 
{
  printf("Выберите ячейку инвентаря:\n");
  
  if (scanf("%d", &item_slot) != 1) 
    {
    printf("Ошибка ввода! Введите число от 0 до 9.\n");
    while (getchar() != '\n'); // чистка буфера
    return; 
    }
  
  if (item_slot > 9 || item_slot < 0) 
    {
    printf("Неверный номер слота! Введите значение от 0 до 9\n");
    return;
    } else {
    
    printf("Выберите id предмета:\n");
    
    if (scanf("%d", &item_id) != 1) 
      {
      printf("Ошибка ввода! Введите число от 0 до 9.\n");
      while (getchar() != '\n'); // чистка буфера
      return;
      }
    
    if (item_id > 9 || item_id < 0) 
      {
      printf("Такого предмета нет!Введите id предмета от 0 до 9\n");
      while (getchar() != '\n'); // чистка буфера
      return;
      } else {
      
      inventory[item_slot] = item_id;
      return;
      }
  }
}


void case5_drop_item()    // выбросить предмет 
{
  printf("Выберите ячейку инвентаря:\n");
  
  if (scanf("%d", &item_slot) != 1) 
    {
    printf("Ошибка ввода! Введите число от 0 до 9\n");
    while (getchar() != '\n'); // чистка буфера
    return;
    }
  
  if (item_slot > 9 || item_slot < 0) 
    {
    printf("Неверный номер слота!Введите значение от 0 до 9\n");
    return;
    } else {
      inventory[item_slot] = 0;
      return;
    }
}


void case6_unique()    // уникальные находки 
{
  int unique_items = 0;
  int items[10] = {0};
  for (int i = 0; i < inventory_size; i++) 
    {
    int id = inventory[i];
    if (id >= 0 && id <= 9) 
      {
      items[id]++;
      }
    }
  for (int id = 1; id <= 9; id++) // начинаем с 1, чтобы проигнорировать 0
    {
    if (items[id] > 0) 
      {
      printf("Предмет ID %d: %d штук\n", id, items[id]);
      unique_items = unique_items + 1;
      }
    }
    printf("Всего уникальных предметов: %d\n", unique_items);
    if (unique_items == 0) 
      {
      printf("Инвнтарь пуст.\n");
      }
}


void case7_search_item()     // поиск предмета в инвентаре по названию     
{
  char search_name[MAX_NAME_LEN];
  printf("Введите название предмета: ");
  while (getchar() != '\n'); // Очистка буфера перед вводом строки
  fgets(search_name, MAX_NAME_LEN, stdin);
  search_name[strcspn(search_name, "\n")] = '\0'; // Удаляем перенос строки

  int target_id = -1;
  for (int i = 0; i < MAX_ITEMS; i++) 
  {
    if (strcmp(item_names[i], search_name) == 0) 
    {
      target_id = i;
      break;
    }
  }

  if (target_id == -1) 
  {
    printf("Предмет \"%s\" не найден.\n", search_name);
    return;
  }

  // проверяем наличие найденного ID в рюкзаке
  int found = 0;
  printf("Предмет \"%s\" (ID: %d) найден в слотах: ", search_name, target_id);
  for (int i = 0; i < inventory_size; i++) 
  {
    if (inventory[i] == target_id) 
    {
      printf("%d ", i);
      found = 1;
    }
  }

  if (!found) 
  {
    printf("В рюкзаке этого нет.\n");
  } else {
    printf("\n");
  }
}

void case8_save_diary() 
{
  FILE *file = fopen("diary.txt", "a");
  if (file == NULL) 
  {
    printf("Ошибка: не удалось открыть или создать файл diary.txt!\n");
    return;
  }

  fprintf(file, "=== Запись в дневнике ===\n");
  fprintf(file, "Фермер: %s\n", player_name);
  fprintf(file, "Время: День %d, Час %d:00\n", current_day, current_hour);
  fprintf(file, "Содержимое инвентаря:\n");
  
  for (int i = 0; i < inventory_size; i++) 
  {
    int id = inventory[i];
    if (id >= 0 && id < MAX_ITEMS) 
    {
      fprintf(file, "  Слот %d: %s\n", i, item_names[id]);
    } else {
      fprintf(file, "  Слот %d: Неизвестный предмет\n", i);
    }
  }
  fprintf(file, "-------------------------\n\n");

  fclose(file);
  printf("Состояние успешно сохранено в дневник (diary.txt).\n");
}

void case9_analyze_logs() 
{
  FILE *in = fopen("input.txt", "r");
  if (in == NULL) 
  {
    printf("Ошибка: не удалось открыть файл input.txt для чтения!\n");
    return;
  }

  FILE *out = fopen("output.txt", "w");
  if (out == NULL) 
  {
    printf("Ошибка: не удалось создать файл output.txt для записи!\n");
    fclose(in);
    return;
  }

  char line[256];
  int line_number = 0;
  int info_count = 0, warn_count = 0, error_count = 0;

  printf("\n--- Обработка логов ---\n");

  while (fgets(line, sizeof(line), in) != NULL) 
  {
    line_number++;
    
    // Считаем общую статистику и обрабатываем строки
    if (strncmp(line, "[INFO]", 6) == 0) 
    {
      info_count++;
    } 
    else if (strncmp(line, "[WARN]", 6) == 0) 
    {
      warn_count++;
      char *message = line + 6; // Смещаем указатель, убирая тег
      // Вывод на экран
      printf("[%d] Строка %d: %s", warn_count + error_count, line_number, message);
      // Запись в файл
      fprintf(out, "Строка %d: %s", line_number, message);
    } 
    else if (strncmp(line, "[ERROR]", 7) == 0) 
    {
      error_count++;
      char *message = line + 7; // Смещаем указатель, убирая тег
      // Вывод на экран
      printf("[%d] Строка %d: %s", warn_count + error_count, line_number, message);
      // Запись в файл
      fprintf(out, "Строка %d: %s", line_number, message);
    }
  }

  // Выводим статистику на экран
  printf("\n=== ОБЩАЯ СТАТИСТИКА ЛОГОВ ===\n");
  printf("Всего строк обработано: %d\n", line_number);
  printf("[INFO]:  %d\n", info_count);
  printf("[WARN]:  %d\n", warn_count);
  printf("[ERROR]: %d\n", error_count);
  printf("==============================\n");

  fclose(in);
  fclose(out);
  printf("Результаты фильтрации успешно сохранены в файл output.txt.\n");
}

void item_catalog() {
    // пытаемся открыть файл 
    FILE *file = fopen("items.txt", "r");
    
    // стандартные названия
    char default_names[MAX_ITEMS][MAX_NAME_LEN] = {
        "Пусто", "Дерево", "Камень", "Семена", "Картошка", 
        "Кирка", "Пшено", "Грибы", "Седло", "Морковь"
    };

    // если файл не найден
    if (file == NULL) {
        printf("\n[Предупреждение]: Файл items.txt не найден! Загружены названия по умолчанию.\n");
        for (int i = 0; i < MAX_ITEMS; i++) {
            strcpy(item_names[i], default_names[i]); // копируем строки из дефолтных
        }
        return; // выходим из функции
    }

    // Если файл открылся
    int id;
    char name[MAX_NAME_LEN];
    int count = 0;

    //пока fscanf возвращает 2
    while (fscanf(file, "%d %31s", &id, name) == 2) {
    if (id >= 0 && id < MAX_ITEMS) {
        strncpy(item_names[id], name, MAX_NAME_LEN - 1);
        item_names[id][MAX_NAME_LEN - 1] = '\0';
        count++;
    }
}

    // Закрываем
    fclose(file);
    printf("Каталог предметов успешно загружен(%d предметов).\n", count);
}

void create_map(int h, int w) {
    map_height = h;
    map_width  = w;

    map = malloc(h * sizeof(char *));        /* массив указателей на строки */
    if (map == NULL) return;

    for (int i = 0; i < h; i++) {
        map[i] = malloc(w * sizeof(char));   /* каждая строка отдельно */
        if (map[i] == NULL) {
            /* не хватило памяти — освобождаем уже выделенное */
            for (int j = 0; j < i; j++) free(map[j]);
            free(map);
            map = NULL;
            return;
        }
    }
}

/* Освобождает память карты: сначала строки, потом массив указателей. */
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

    /* 1. Заполняем препятствиями — БЕЗ границ из стен */
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (rand() % 100 < 20)
                map[y][x] = (rand() % 2 == 0) ? CELL_TREE : CELL_ROCK;
            else
                map[y][x] = CELL_FLOOR;
        }
    }

    /* 2. Ставим героя в центр (или на первую свободную клетку) */
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

    /* 3. Один костёр на карту — только на оставшийся FLOOR */
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
    /* страховка: если за 200 попыток не нашли — берём первую свободную */
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

    /* 4. 1–2 входа в пещеры — тоже только на FLOOR */
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

    /* стены по краям + случайные камни внутри */
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

    /* гарантируем минимум 2 свободные клетки (под героя и выход) */
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

    /* 1. Герой — на первую свободную */
    int hx = -1, hy = -1;
    for (int y = 1; y < h - 1 && hx == -1; y++)
        for (int x = 1; x < w - 1; x++)
            if (map[y][x] == CELL_FLOOR) { hx = x; hy = y; break; }
    map[hy][hx] = CELL_HERO;
    hero_x = hx;
    hero_y = hy;

    /* 2. Выход — на самую дальнюю от героя свободную клетку */
    int best = -1, ox = -1, oy = -1;
    for (int y = 1; y < h - 1; y++)
        for (int x = 1; x < w - 1; x++)
            if (map[y][x] == CELL_FLOOR) {
                int d = (x - hx) * (x - hx) + (y - hy) * (y - hy);
                if (d > best) { best = d; ox = x; oy = y; }
            }
    /* страховка: если вдруг не нашли — берём любую, кроме позиции героя */
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

/* Рисует карту в консоль. */
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
        if (location_type == LOC_FOREST) return 2; /* выход за край — новый лес */
        return 0;                                   /* из пещеры нельзя выйти */
    }

    char c = map[new_y][new_x];
    if (c == CELL_TREE || c == CELL_ROCK || c == CELL_WALL) return 0;

    /* если под героем БЫЛ костёр — возвращаем его, а не '.' */
    if (location_type == LOC_FOREST &&
        hero_x == campfire_x && hero_y == campfire_y) {
        map[hero_y][hero_x] = CELL_CAMP;
    } else {
        map[hero_y][hero_x] = CELL_FLOOR;
    }

    hero_x = new_x;
    hero_y = new_y;

    if (c == CELL_CAVE) return 4;     /* наступили на вход/выход */
    map[hero_y][hero_x] = CELL_HERO;
    if (c == CELL_CAMP) return 3;     /* встали на костёр */
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
        if (scanf(" %c", &cmd) != 1) break;   /* Ctrl+D → выходим */
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) { }
        cmd = (char)toupper((unsigned char)cmd);

        /* Q — домой (только в лесу и только на костре) */
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
            /* выход за край леса → новая карта, счётчик +1 */
            int old_x = hero_x, old_y = hero_y;
            free_map();
            distance_from_home++;
            generate_forest();

            /* переставляем героя к противоположному краю новой карты */
            int cx = hero_x, cy = hero_y;
            map[cy][cx] = CELL_FLOOR;

            int go_x = old_x, go_y = old_y;
            if (dy == -1) go_y = map_height - 1;  /* ушёл вверх — пришёл снизу */
            if (dy ==  1) go_y = 0;
            if (dx == -1) go_x = map_width  - 1;
            if (dx ==  1) go_x = 0;

            if (go_x < 0) go_x = 0;
            if (go_x >= map_width)  go_x = map_width - 1;
            if (go_y < 0) go_y = 0;
            if (go_y >= map_height) go_y = map_height - 1;

            if (map[go_y][go_x] != CELL_FLOOR) {  /* на краю препятствие */
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
            /* смена локации: лес ↔ пещера */
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

    free_map();   /* ОБЯЗАТЕЛЬНО освобождаем память */
}

// главаная функция-----------------------------------------------------------------

int main() 
{
  srand((unsigned)time(NULL));
  item_catalog();

  while (1) // ввод имени
  {
      printf("Введите имя: ");
      
      fgets(player_name, player_name_max, stdin);
      
      player_name[strcspn(player_name, "\n")] = '\0';

      if (strlen(player_name) > 0) 
      {
          printf("Имя установлено: %s.\n\n", player_name);
          break;
      }
    
      printf("Вы должны дать себе имя, чтобы продолжить...\n");
  }


  while (1)    // меню
  {
    printf("Привет, %s ! \n", player_name);
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
    
    if (scanf("%d", &userAction) != 1) 
    {
    
      printf("Ошибка ввода! Введите число.\n");
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) { }
    if (ch == EOF) break;
    continue;
    
    }
    


    switch (userAction) // действия в зависимости от выбора
    {
    
    case 0:   // завершение работы проги
      {
      printf("Пока-пока!\n");
      return 0;
      break;
      }
    
    case 1:   // вывести время
      {
      case1_time(); 
      break;
      }
    
    case 2:    // поработать
      {
      case2_work(); 
      break;
      }
    
    case 3:    // вывод инвентаря
      {
      case3_show_inventory();
      break;
      }
    
    case 4:    // положить предмет
      {
      case4_put_item();
      break;
      }
      
    case 5:    // выбросить предмет
      {
      case5_drop_item();
      break;
      }

    case 6:    // уникальные находки
      {
      case6_unique();
      break;
      }

    case 7:    // поиск предмета
      {
      case7_search_item();
      break;
      }

    case 8:    // записи в дневнике
      {
      case8_save_diary();
      break;
      }
      
    case 9:    // анализ логов
      {
      case9_analyze_logs();
      break;
      }

    case 10:   // приключение
      {
      case10_adventure();
      break;
      }

    default:    // если ввод неправильный
    {
      printf("Неверный ввод\n");
      break;
    }
    }
  }
}