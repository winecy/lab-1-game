#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "map.h"
#include "inventory_weight.h"

// выделение памяти---------------------------------------------------------

struct Map *create_map(int h, int w)
{
    struct Map *m = (struct Map *)malloc(sizeof(struct Map));
    if (m == NULL) return NULL;
    m->cells = (char **)malloc(h * sizeof(char *));
    if (m->cells == NULL) { free(m); return NULL; }
    for (int i = 0; i < h; i++)
    {
        m->cells[i] = (char *)malloc(w * sizeof(char));
        if (m->cells[i] == NULL)
        {
            for (int j = 0; j < i; j++) free(m->cells[j]);
            free(m->cells);
            free(m);
            return NULL;
        }
    }
    m->height = h;
    m->width = w;
    m->floors_passed = 0;
    m->type = LOC_FOREST;
    m->campfire_x = -1;
    m->campfire_y = -1;
    return m;
}

void free_map(struct Map *m)
{
    if (m == NULL) return;
    for (int i = 0; i < m->height; i++) free(m->cells[i]);
    free(m->cells);
    free(m);
}

// генерация леса-----------------------------------------------------------

void generate_forest(struct Map *m, struct Player *player)
{
    int h = MIN_FOREST_SIZE + rand() % (MAX_FOREST_SIZE - MIN_FOREST_SIZE + 1);
    int w = MIN_FOREST_SIZE + rand() % (MAX_FOREST_SIZE - MIN_FOREST_SIZE + 1);
    m->height = h;
    m->width = w;
    m->type = LOC_FOREST;
    m->campfire_x = -1;
    m->campfire_y = -1;

    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            if (rand() % 100 < 20)
                m->cells[y][x] = (rand() % 2 == 0) ? CELL_TREE : CELL_ROCK;
            else
                m->cells[y][x] = CELL_FLOOR;

    int hx = w / 2, hy = h / 2;
    if (m->cells[hy][hx] != CELL_FLOOR)
    {
        int found = 0;
        for (int y = 0; y < h && !found; y++)
            for (int x = 0; x < w && !found; x++)
                if (m->cells[y][x] == CELL_FLOOR) { hx = x; hy = y; found = 1; }
        if (!found) m->cells[hy][hx] = CELL_FLOOR;
    }
    m->cells[hy][hx] = CELL_HERO;
    player->x = hx;
    player->y = hy;

    for (int attempt = 0; attempt < 200 && m->campfire_x == -1; attempt++)
    {
        int cx = rand() % w, cy = rand() % h;
        if (m->cells[cy][cx] == CELL_FLOOR)
        {
            m->cells[cy][cx] = CELL_CAMP;
            m->campfire_x = cx;
            m->campfire_y = cy;
        }
    }
    if (m->campfire_x == -1)
        for (int y = 0; y < h && m->campfire_x == -1; y++)
            for (int x = 0; x < w; x++)
                if (m->cells[y][x] == CELL_FLOOR)
                {
                    m->cells[y][x] = CELL_CAMP;
                    m->campfire_x = x;
                    m->campfire_y = y;
                    break;
                }

    int caves = 1 + rand() % 2;
    for (int i = 0; i < caves; i++)
        for (int attempt = 0; attempt < 200; attempt++)
        {
            int ox = rand() % w, oy = rand() % h;
            if (m->cells[oy][ox] == CELL_FLOOR)
            {
                m->cells[oy][ox] = CELL_CAVE;
                break;
            }
        }
}

// генерация пещеры---------------------------------------------------------

void generate_cave(struct Map *m, struct Player *player)
{
    int h = MIN_CAVE_SIZE + rand() % (MAX_CAVE_SIZE - MIN_CAVE_SIZE + 1);
    int w = MIN_CAVE_SIZE + rand() % (MAX_CAVE_SIZE - MIN_CAVE_SIZE + 1);
    m->height = h;
    m->width = w;
    m->type = LOC_CAVE;

    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
        {
            if (x == 0 || y == 0 || x == w - 1 || y == h - 1)
                m->cells[y][x] = CELL_WALL;
            else if (rand() % 100 < 25)
                m->cells[y][x] = CELL_ROCK;
            else
                m->cells[y][x] = CELL_FLOOR;
        }

    int free_count = 0;
    for (int y = 1; y < h - 1; y++)
        for (int x = 1; x < w - 1; x++)
            if (m->cells[y][x] == CELL_FLOOR) free_count++;
    for (int y = 1; y < h - 1 && free_count < 2; y++)
        for (int x = 1; x < w - 1 && free_count < 2; x++)
            if (m->cells[y][x] != CELL_FLOOR) { m->cells[y][x] = CELL_FLOOR; free_count++; }

    int hx = -1, hy = -1;
    for (int y = 1; y < h - 1 && hx == -1; y++)
        for (int x = 1; x < w - 1; x++)
            if (m->cells[y][x] == CELL_FLOOR) { hx = x; hy = y; break; }
    m->cells[hy][hx] = CELL_HERO;
    player->x = hx;
    player->y = hy;

    int best = -1, ox = -1, oy = -1;
    for (int y = 1; y < h - 1; y++)
        for (int x = 1; x < w - 1; x++)
            if (m->cells[y][x] == CELL_FLOOR)
            {
                int d = (x - hx) * (x - hx) + (y - hy) * (y - hy);
                if (d > best) { best = d; ox = x; oy = y; }
            }
    if (ox == -1)
        for (int y = 1; y < h - 1 && ox == -1; y++)
            for (int x = 1; x < w - 1; x++)
                if (!(x == hx && y == hy)) { m->cells[y][x] = CELL_FLOOR; ox = x; oy = y; break; }
    m->cells[oy][ox] = CELL_CAVE;
}

// печать карты-------------------------------------------------------------

void print_map(struct Map *m)
{
    printf("\n");
    for (int y = 0; y < m->height; y++)
    {
        for (int x = 0; x < m->width; x++) printf("%c", m->cells[y][x]);
        printf("\n");
    }
}

// движение героя-----------------------------------------------------------

int move_hero(struct Map *m, struct Player *player, int dx, int dy)
{
    int new_x = player->x + dx;
    int new_y = player->y + dy;
    int out_x = (new_x < 0 || new_x >= m->width);
    int out_y = (new_y < 0 || new_y >= m->height);
    if (out_x || out_y)
    {
        if (m->type == LOC_FOREST) return MOVE_NEW_FOREST;
        return MOVE_BLOCKED;
    }
    char c = m->cells[new_y][new_x];
    if (c == CELL_TREE || c == CELL_ROCK || c == CELL_WALL) return MOVE_BLOCKED;

    if (m->type == LOC_FOREST &&
        player->x == m->campfire_x && player->y == m->campfire_y)
        m->cells[player->y][player->x] = CELL_CAMP;
    else
        m->cells[player->y][player->x] = CELL_FLOOR;

    player->x = new_x;
    player->y = new_y;
    if (c == CELL_CAVE) return MOVE_CAVE_ENTRANCE;
    m->cells[player->y][player->x] = CELL_HERO;
    if (c == CELL_CAMP) return MOVE_CAMP;
    return MOVE_OK;
}

// полный режим приключения-------------------------------------------------

void run_adventure(struct Player *player, int *day, int *hour)
{
    if (player == NULL)
    {
        printf("Сначала создайте персонажа!\n");
        return;
    }
    if (player->stamina <= 0)
    {
        printf("Нет выносливости для похода! Отдохните.\n");
        return;
    }
    if (*hour >= NIGHT_HOUR && *hour < SLEEP_HOUR)
    {
        printf("Сейчас ночь (02:00–08:00), фермер должен спать!\n");
        return;
    }

    struct Map *m = create_map(MIN_FOREST_SIZE, MIN_FOREST_SIZE);
    if (m == NULL) { printf("Ошибка выделения памяти!\n"); return; }

    generate_forest(m, player);
    int running = 1;
    int evacuate = 0;

    while (running)
    {
        print_map(m);

        printf("HP: %d/%d | Выносливость: %d/%d | Золото: %d | Этаж: %d | Время: %d:00\n",
               player->hp, player->max_hp,
               player->stamina, player->max_stamina,
               player->gold, m->floors_passed + 1, *hour);
        printf("Вес: %d/%d [%s]\n",
               player->backpack.current_weight,
               player->backpack.max_capacity,
               load_state_name(player->backpack.state));

        printf("Ход (WASD / Q) > ");
        char cmd;
        if (scanf(" %c", &cmd) != 1) break;
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) { }
        cmd = (char)toupper((unsigned char)cmd);

        if (cmd == 'Q')
        {
            if (m->type == LOC_FOREST &&
                player->x == m->campfire_x && player->y == m->campfire_y)
            {
                printf("Вы вернулись домой! Пройдено этажей: %d, золото: %d\n",
                       m->floors_passed, player->gold);
                running = 0;
            }
            else if (m->type == LOC_FOREST)
                printf("Домой можно только стоя на костре ^.\n");
            else
                printf("Из пещеры нет прямого пути домой. Найдите O.\n");
            continue;
        }

        int dx = 0, dy = 0;
        if      (cmd == 'W') dy = -1;
        else if (cmd == 'S') dy =  1;
        else if (cmd == 'A') dx = -1;
        else if (cmd == 'D') dx =  1;
        else { printf("Неизвестная команда.\n"); continue; }

        int result = move_hero(m, player, dx, dy);

        if (result == MOVE_BLOCKED)
        {
            printf("Туда нельзя.\n");
        }
        else if (result == MOVE_OK)
        {
            int cost = step_stamina_cost(&player->backpack);
            player->stamina -= cost;
            if (cost == 3)
                printf("Перегруз! Шаг стоил %d выносливости.\n", cost);
            if (player->stamina <= 0)
            {
                printf("Выносливость на нуле! Фермер падает без сил...\n");
                evacuate = 1; running = 0;
            }
        }
        else if (result == MOVE_CAMP)
        {
            printf("Вы у костра. Нажмите Q, чтобы вернуться домой.\n");
        }
        else if (result == MOVE_NEW_FOREST || result == MOVE_CAVE_ENTRANCE)
        {
            int was_forest = (m->type == LOC_FOREST);
            int old_x = player->x, old_y = player->y;

            free_map(m);
            m = create_map(MIN_FOREST_SIZE, MIN_FOREST_SIZE);
            if (m == NULL) { printf("Ошибка памяти!\n"); return; }

            if (was_forest)
            {
                generate_cave(m, player);
                printf("Вы вошли в пещеру...\n");
            }
            else
            {
                m->floors_passed++;
                generate_forest(m, player);

                *hour += FLOOR_TIME_COST;
                if (*hour >= HOURS_PER_DAY) { *hour %= HOURS_PER_DAY; (*day)++; }
                player->stamina -= FLOOR_STAMINA_COST;
                player->gold += GOLD_MULTIPLIER * m->floors_passed;

                int go_x = old_x, go_y = old_y;
                if (dy == -1) go_y = m->height - 1;
                if (dy ==  1) go_y = 0;
                if (dx == -1) go_x = m->width  - 1;
                if (dx ==  1) go_x = 0;
                if (go_x < 0) go_x = 0;
                if (go_x >= m->width)  go_x = m->width - 1;
                if (go_y < 0) go_y = 0;
                if (go_y >= m->height) go_y = m->height - 1;
                if (m->cells[go_y][go_x] != CELL_FLOOR)
                {
                    go_x = player->x;
                    go_y = player->y;
                }
                m->cells[player->y][player->x] = CELL_FLOOR;
                player->x = go_x;
                player->y = go_y;
                m->cells[player->y][player->x] = CELL_HERO;

                printf("Вы вышли из пещеры в новый лес. Этаж: %d, золото: %d\n",
                       m->floors_passed + 1, player->gold);
            }

            if (*hour >= NIGHT_HOUR && *hour < SLEEP_HOUR)
            {
                printf("Наступила ночь! Фермер падает без сил...\n");
                evacuate = 1; running = 0;
            }
            else if (player->stamina <= 0)
            {
                printf("Выносливость на нуле! Фермер падает без сил...\n");
                evacuate = 1; running = 0;
            }
        }
    }

    if (evacuate)
    {
        player->gold /= GOLD_EVAC_PENALTY;
        printf("Экстренная эвакуация! Спасатели забрали половину золота. Осталось: %d\n",
               player->gold);
    }

    free_map(m);
}
