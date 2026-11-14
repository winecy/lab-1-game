#ifndef UTILS_H
#define UTILS_H

// константы----------------------------------------------------------------

#define PLAYER_NAME_MAX   32   // максимальная длина имени фермера
#define MAX_ITEMS         10   // максимальное количество предметов
#define MAX_NAME_LEN      32   // максимальная длина названия предмета
#define INVENTORY_SIZE    10   // размер инвентаря
#define HOURS_PER_DAY     24

#define SLEEP_HOUR        8    // час подъёма после сна
#define NIGHT_HOUR        2    // граница ночи (нельзя пройти без сна)

#define ITEMS_FILE   "items.txt"
#define DIARY_FILE   "diary.txt"
#define INPUT_FILE   "input.txt"
#define OUTPUT_FILE  "output.txt"

#define GOLD_MULTIPLIER     10  // базовый множитель награды за этаж
#define STEP_STAMINA_COST    1  // выносливость за шаг
#define FLOOR_STAMINA_COST   5  // выносливость за переход на этаж
#define FLOOR_TIME_COST      1  // часы за переход на этаж
#define GOLD_EVAC_PENALTY    2  // делитель золота при эвакуации

#define START_HP       100
#define START_STAMINA   50
#define START_GOLD       0


// прототипы утилит (реализация — utils.c в коммите 2)----------------------

int  read_int_safe(const char *prompt, int min, int max);
void read_string_safe(const char *prompt, char *buffer, int size);
void load_item_names(char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void analyze_logs(void);

#endif // UTILS_H