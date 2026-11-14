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

// перевыделение cells под новый размер-------------------------------------

static void resize_map_cells(struct Map *m, int h, int w)
{
    if (m->cells != NULL)
    {
        for (int i = 0; i < m->height; i++) free(m->cells[i]);
        free(m->cells);
    }
    m->cells = (char **)malloc(h * sizeof(char *));
    if (m->cells == NULL) { m->height = 0; m->width = 0; return; }
    for (int i = 0; i < h; i++)
    {
        m->cells[i] = (char *)malloc(w * sizeof(char));
        if (m->cells[i] == NULL)
        {
            for (int j = 0; j < i; j++) free(m->cells[j]);
            free(m->cells);
            m->cells = NULL;
            m->height = 0;
            m->width = 0;
            return;
        }
    }
    m->height = h;
    m->width = w;
}

// BFS от (sx, sy): помечаем достижимые клетки (не стена/камень/дерево)---
// Возвращает число достижимых клеток.

static int bfs_reach(struct Map *m, int sx, int sy,
                     char reach[MAX_FOREST_SIZE][MAX_FOREST_SIZE])
{
    int h = m->height, w = m->width;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            reach[y][x] = 0;

    int qx[MAX_FOREST_SIZE * MAX_FOREST_SIZE];
    int qy[MAX_FOREST_SIZE * MAX_FOREST_SIZE];
    int head = 0, tail = 0;

    reach[sy][sx] = 1;
    qx[tail] = sx; qy[tail] = sy; tail++;

    const int dxs[4] = {0, 0, -1, 1};
    const int dys[4] = {-1, 1, 0, 0};

    while (head < tail)
    {
        int cx = qx[head], cy = qy[head]; head++;
        for (int d = 0; d < 4; d++)
        {
            int nx = cx + dxs[d], ny = cy + dys[d];
            if (nx < 0 || nx >= w || ny < 0 || ny >= h) continue;
            if (reach[ny][nx]) continue;
            char c = m->cells[ny][nx];
            if (c == CELL_WALL || c == CELL_ROCK || c == CELL_TREE) continue;
            reach[ny][nx] = 1;
            qx[tail] = nx; qy[tail] = ny; tail++;
        }
    }
    return tail;
}

// прорубание прямой линии от (x1,y1) до (x2,y2)---------------------------
// Превращает деревья/камни на пути в пол.

static void carve_line(struct Map *m, int x1, int y1, int x2, int y2)
{
    int cx = x1, cy = y1;
    while (cx != x2 || cy != y2)
    {
        if (m->cells[cy][cx] == CELL_TREE || m->cells[cy][cx] == CELL_ROCK)
            m->cells[cy][cx] = CELL_FLOOR;
        if (cx < x2) cx++;
        else if (cx > x2) cx--;
        if (cy < y2) cy++;
        else if (cy > y2) cy--;
    }
}

// генерация леса-----------------------------------------------------------
// Гарантии:
//   - стены из деревьев по краю;
//   - один выход — вырубленный проход;
//   - герой всегда может дойти до выхода (прорубаем путь при необходимости);
//   - костёр и вход в пещеру — на достижимых клетках.

void generate_forest(struct Map *m, struct Player *player)
{
    int h = MIN_FOREST_SIZE + rand() % (MAX_FOREST_SIZE - MIN_FOREST_SIZE + 1);
    int w = MIN_FOREST_SIZE + rand() % (MAX_FOREST_SIZE - MIN_FOREST_SIZE + 1);

    resize_map_cells(m, h, w);

    m->type = LOC_FOREST;
    m->campfire_x = -1;
    m->campfire_y = -1;

    // всё — трава
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            m->cells[y][x] = CELL_FLOOR;

    // стены из деревьев по краям
    for (int x = 0; x < w; x++)
    {
        m->cells[0][x]     = CELL_TREE;
        m->cells[h - 1][x] = CELL_TREE;
    }
    for (int y = 0; y < h; y++)
    {
        m->cells[y][0]     = CELL_TREE;
        m->cells[y][w - 1] = CELL_TREE;
    }

    // один выход — вырубаем дерево в случайной стене
    int side = rand() % 4;
    int exit_x = 0, exit_y = 0;
    if (side == 0) { exit_y = 0;     exit_x = 1 + rand() % (w - 2); }
    if (side == 1) { exit_y = h - 1; exit_x = 1 + rand() % (w - 2); }
    if (side == 2) { exit_x = 0;     exit_y = 1 + rand() % (h - 2); }
    if (side == 3) { exit_x = w - 1; exit_y = 1 + rand() % (h - 2); }
    m->cells[exit_y][exit_x] = CELL_FLOOR;

    // клетка перед выходом — всегда свободна
    int front_x = exit_x, front_y = exit_y;
    if (side == 0) front_y = 1;
    if (side == 1) front_y = h - 2;
    if (side == 2) front_x = 1;
    if (side == 3) front_x = w - 2;
    m->cells[front_y][front_x] = CELL_FLOOR;

    // случайные деревья/камни внутри
    for (int y = 1; y < h - 1; y++)
        for (int x = 1; x < w - 1; x++)
        {
            if (x == front_x && y == front_y) continue;
            if (rand() % 100 < 18)
                m->cells[y][x] = (rand() % 2 == 0) ? CELL_TREE : CELL_ROCK;
        }

    // герой — центр или ближайшая свободная клетка
    int hx = w / 2, hy = h / 2;
    if (m->cells[hy][hx] != CELL_FLOOR)
    {
        int found = 0;
        for (int y = 1; y < h - 1 && !found; y++)
            for (int x = 1; x < w - 1 && !found; x++)
                if (m->cells[y][x] == CELL_FLOOR) { hx = x; hy = y; found = 1; }
        if (!found) m->cells[hy][hx] = CELL_FLOOR;
    }
    m->cells[hy][hx] = CELL_HERO;
    player->x = hx;
    player->y = hy;

    // ГАРАНТИРУЕМ: путь от героя до выхода свободен
    char reach[MAX_FOREST_SIZE][MAX_FOREST_SIZE];
    bfs_reach(m, hx, hy, reach);
    if (!reach[front_y][front_x])
    {
        carve_line(m, hx, hy, front_x, front_y);
        m->cells[front_y][front_x] = CELL_FLOOR;
        m->cells[exit_y][exit_x]   = CELL_FLOOR;
    }

    // костёр — на достижимой клетке
    int placed = 0;
    for (int attempt = 0; attempt < 300 && !placed; attempt++)
    {
        int cx = 1 + rand() % (w - 2);
        int cy = 1 + rand() % (h - 2);
        if (m->cells[cy][cx] == CELL_FLOOR)
        {
            m->cells[cy][cx] = CELL_CAMP;
            m->campfire_x = cx;
            m->campfire_y = cy;
            placed = 1;
        }
    }
    if (!placed)
        for (int y = 1; y < h - 1 && !placed; y++)
            for (int x = 1; x < w - 1 && !placed; x++)
                if (m->cells[y][x] == CELL_FLOOR)
                {
                    m->cells[y][x] = CELL_CAMP;
                    m->campfire_x = x;
                    m->campfire_y = y;
                    placed = 1;
                }

    // вход в пещеру — на достижимой клетке
    placed = 0;
    for (int attempt = 0; attempt < 400 && !placed; attempt++)
    {
        int ox = 1 + rand() % (w - 2);
        int oy = 1 + rand() % (h - 2);
        if (ox == front_x && oy == front_y) continue;
        if (m->cells[oy][ox] == CELL_FLOOR)
        {
            m->cells[oy][ox] = CELL_CAVE;
            placed = 1;
        }
    }
    if (!placed)
        for (int y = 1; y < h - 1 && !placed; y++)
            for (int x = 1; x < w - 1 && !placed; x++)
            {
                if (x == front_x && y == front_y) continue;
                if (m->cells[y][x] == CELL_FLOOR)
                {
                    m->cells[y][x] = CELL_CAVE;
                    placed = 1;
                }
            }
}

// генерация пещеры---------------------------------------------------------
// Гарантии:
//   - герой на свободной клетке;
//   - достижимая область не меньше 1/3 карты (прорубаем камни при необходимости);
//   - O стоит на достижимой клетке и НЕ в тупике (>= 2 открытых соседа);
//   - абсолютный fallback: прорубаем соседа и ставим O рядом с героем.

void generate_cave(struct Map *m, struct Player *player)
{
    int h = MIN_CAVE_SIZE + rand() % (MAX_CAVE_SIZE - MIN_CAVE_SIZE + 1);
    int w = MIN_CAVE_SIZE + rand() % (MAX_CAVE_SIZE - MIN_CAVE_SIZE + 1);

    resize_map_cells(m, h, w);

    m->type = LOC_CAVE;

    // стены по краю, внутри — камни/пол
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

    // минимум 2 клетки пола
    int free_count = 0;
    for (int y = 1; y < h - 1; y++)
        for (int x = 1; x < w - 1; x++)
            if (m->cells[y][x] == CELL_FLOOR) free_count++;
    for (int y = 1; y < h - 1 && free_count < 2; y++)
        for (int x = 1; x < w - 1 && free_count < 2; x++)
            if (m->cells[y][x] != CELL_FLOOR) { m->cells[y][x] = CELL_FLOOR; free_count++; }

    // герой — на первой свободной клетке
    int hx = -1, hy = -1;
    for (int y = 1; y < h - 1 && hx == -1; y++)
        for (int x = 1; x < w - 1; x++)
            if (m->cells[y][x] == CELL_FLOOR) { hx = x; hy = y; break; }
    m->cells[hy][hx] = CELL_HERO;
    player->x = hx;
    player->y = hy;

    // цель: >= 1/3 всех внутренних клеток достижимы
    int inside_total = (h - 2) * (w - 2);
    int need = inside_total / 3;
    if (need < 3) need = 3;

    char reach[MAX_FOREST_SIZE][MAX_FOREST_SIZE];

    // прорубаем камни, граничащие с достижимой зоной, пока область < need
    for (int attempt = 0; attempt < 100; attempt++)
    {
        int reached = bfs_reach(m, hx, hy, reach);
        if (reached >= need) break;

        int progress = 0;
        for (int y = 1; y < h - 1 && !progress; y++)
            for (int x = 1; x < w - 1 && !progress; x++)
            {
                if (m->cells[y][x] != CELL_ROCK) continue;
                int touch = 0;
                if (y > 0     && reach[y-1][x]) touch = 1;
                if (y < h - 1 && reach[y+1][x]) touch = 1;
                if (x > 0     && reach[y][x-1]) touch = 1;
                if (x < w - 1 && reach[y][x+1]) touch = 1;
                if (touch) { m->cells[y][x] = CELL_FLOOR; progress = 1; }
            }
        if (!progress) break;
    }

    // финальный BFS
    bfs_reach(m, hx, hy, reach);

    // O — на достижимой клетке, не в тупике, максимально далеко от героя
    int best = -1, ox = -1, oy = -1;
    for (int y = 1; y < h - 1; y++)
        for (int x = 1; x < w - 1; x++)
        {
            if (!reach[y][x] || m->cells[y][x] != CELL_FLOOR) continue;
            if (x == hx && y == hy) continue;

            int open_neighbors = 0;
            const int dxs[4] = {0, 0, -1, 1};
            const int dys[4] = {-1, 1, 0, 0};
            for (int d = 0; d < 4; d++)
            {
                int nx = x + dxs[d], ny = y + dys[d];
                if (nx <= 0 || nx >= w - 1 || ny <= 0 || ny >= h - 1) continue;
                char c = m->cells[ny][nx];
                if (c == CELL_FLOOR || c == CELL_CAMP ||
                    c == CELL_HERO || c == CELL_CAVE)
                    open_neighbors++;
            }
            if (open_neighbors < 2) continue;

            int d = (x - hx) * (x - hx) + (y - hy) * (y - hy);
            if (d > best) { best = d; ox = x; oy = y; }
        }

    // если «не тупиковых» нет — берём любую достижимую
    if (ox == -1)
        for (int y = 1; y < h - 1 && ox == -1; y++)
            for (int x = 1; x < w - 1; x++)
            {
                if (!reach[y][x] || m->cells[y][x] != CELL_FLOOR) continue;
                if (x == hx && y == hy) continue;
                ox = x; oy = y; break;
            }

    // абсолютный fallback — прорубаем соседа героя
    if (ox == -1)
    {
        const int dxs[4] = {0, 0, -1, 1};
        const int dys[4] = {-1, 1, 0, 0};
        for (int d = 0; d < 4; d++)
        {
            int nx = hx + dxs[d], ny = hy + dys[d];
            if (nx <= 0 || nx >= w - 1 || ny <= 0 || ny >= h - 1) continue;
            if (m->cells[ny][nx] == CELL_ROCK) m->cells[ny][nx] = CELL_FLOOR;
            if (m->cells[ny][nx] == CELL_FLOOR)
            { ox = nx; oy = ny; break; }
        }
    }

    if (ox != -1)
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

                // спавн на противоположной стороне нового леса
                int go_x = old_x, go_y = old_y;
                if (dy == -1) go_y = m->height - 2;
                if (dy ==  1) go_y = 1;
                if (dx == -1) go_x = m->width  - 2;
                if (dx ==  1) go_x = 1;
                if (go_x < 1) go_x = 1;
                if (go_x >= m->width  - 1) go_x = m->width  - 2;
                if (go_y < 1) go_y = 1;
                if (go_y >= m->height - 1) go_y = m->height - 2;
                if (m->cells[go_y][go_x] != CELL_FLOOR)
                {
                    int found = 0;
                    for (int ry = 1; ry < m->height - 1 && !found; ry++)
                        for (int rx = 1; rx < m->width - 1 && !found; rx++)
                            if (m->cells[ry][rx] == CELL_FLOOR)
                            { go_x = rx; go_y = ry; found = 1; }
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
