#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "map.h"
#include "inventory_weight.h"

// ---- память -------------------------------------------------------------

struct Map *create_map(int h, int w)
{
    struct Map *m = malloc(sizeof(*m));
    if (!m) return NULL;
    m->cells = malloc(h * sizeof(char *));
    if (!m->cells) { free(m); return NULL; }
    for (int i = 0; i < h; i++)
        if (!(m->cells[i] = malloc(w))) {
            while (i--) free(m->cells[i]);
            free(m->cells); free(m);
            return NULL;
        }
    m->height = h; m->width = w;
    m->floors_passed = 0;
    m->type = LOC_FOREST;
    m->campfire_x = m->campfire_y = -1;
    return m;
}

void free_map(struct Map *m)
{
    if (!m) return;
    for (int i = 0; i < m->height; i++) free(m->cells[i]);
    free(m->cells); free(m);
}

static void resize_map_cells(struct Map *m, int h, int w)
{
    for (int i = 0; i < m->height; i++) free(m->cells[i]);
    free(m->cells);
    m->cells = malloc(h * sizeof(char *));
    for (int i = 0; i < h; i++) m->cells[i] = malloc(w);
    m->height = h; m->width = w;
}

// ---- BFS: какие клетки достижимы ----------------------------------------

static int bfs_reach(struct Map *m, int sx, int sy,
                     char reach[MAX_FOREST_SIZE][MAX_FOREST_SIZE])
{
    int h = m->height, w = m->width;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) reach[y][x] = 0;

    int qx[MAX_FOREST_SIZE * MAX_FOREST_SIZE], qy[MAX_FOREST_SIZE * MAX_FOREST_SIZE];
    int head = 0, tail = 0;
    reach[sy][sx] = 1;
    qx[tail] = sx; qy[tail] = sy; tail++;

    const int dxs[4] = {0, 0, -1, 1}, dys[4] = {-1, 1, 0, 0};
    while (head < tail) {
        int cx = qx[head], cy = qy[head]; head++;
        for (int d = 0; d < 4; d++) {
            int nx = cx + dxs[d], ny = cy + dys[d];
            if (nx < 0 || nx >= w || ny < 0 || ny >= h || reach[ny][nx]) continue;
            char c = m->cells[ny][nx];
            if (c == CELL_WALL || c == CELL_ROCK || c == CELL_TREE) continue;
            reach[ny][nx] = 1;
            qx[tail] = nx; qy[tail] = ny; tail++;
        }
    }
    return tail;
}

// ---- лес ----------------------------------------------------------------

void generate_forest(struct Map *m, struct Player *player)
{
    int h = MIN_FOREST_SIZE + rand() % (MAX_FOREST_SIZE - MIN_FOREST_SIZE + 1);
    int w = MIN_FOREST_SIZE + rand() % (MAX_FOREST_SIZE - MIN_FOREST_SIZE + 1);
    resize_map_cells(m, h, w);

    m->type = LOC_FOREST;
    m->campfire_x = m->campfire_y = -1;

    // периметр — T, внутри — трава
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            m->cells[y][x] = (y == 0 || y == h-1 || x == 0 || x == w-1)
                             ? CELL_TREE : CELL_FLOOR;

    // один выход в случайной стене
    int side = rand() % 4, ex, ey, fx, fy;
    if (side == 0) { ey = 0;     ex = 1 + rand() % (w-2); fy = 1;     fx = ex; }
    if (side == 1) { ey = h - 1; ex = 1 + rand() % (w-2); fy = h - 2; fx = ex; }
    if (side == 2) { ex = 0;     ey = 1 + rand() % (h-2); fx = 1;     fy = ey; }
    if (side == 3) { ex = w - 1; ey = 1 + rand() % (h-2); fx = w - 2; fy = ey; }

    // внутри — случайные деревья/камни (кроме выхода и клетки перед ним)
    for (int y = 1; y < h - 1; y++)
        for (int x = 1; x < w - 1; x++) {
            if (x == fx && y == fy) continue;
            if (rand() % 100 < 18)
                m->cells[y][x] = (rand() % 2 == 0) ? CELL_TREE : CELL_ROCK;
        }

    m->cells[ey][ex] = CELL_FLOOR;
    m->cells[fy][fx] = CELL_FLOOR;

    // герой в центр
    int hx = w / 2, hy = h / 2;
    if (m->cells[hy][hx] != CELL_FLOOR) {
        for (int y = 1; y < h - 1; y++)
            for (int x = 1; x < w - 1; x++)
                if (m->cells[y][x] == CELL_FLOOR) { hx = x; hy = y; y = h; break; }
    }
    m->cells[hy][hx] = CELL_HERO;
    player->x = hx; player->y = hy;

    // гарантируем путь от героя до выхода
    char reach[MAX_FOREST_SIZE][MAX_FOREST_SIZE];
    bfs_reach(m, hx, hy, reach);
    if (!reach[fy][fx]) {
        int cx = hx, cy = hy;
        while (cx != fx || cy != fy) {
            if (m->cells[cy][cx] != CELL_HERO) m->cells[cy][cx] = CELL_FLOOR;
            if (cx < fx) cx++; else if (cx > fx) cx--;
            if (cy < fy) cy++; else if (cy > fy) cy--;
        }
        m->cells[fy][fx] = CELL_FLOOR;
        m->cells[ey][ex] = CELL_FLOOR;
    }

    // костёр
    for (int t = 0; t < 300 && m->campfire_x == -1; t++) {
        int cx = 1 + rand() % (w-2), cy = 1 + rand() % (h-2);
        if (m->cells[cy][cx] == CELL_FLOOR) {
            m->cells[cy][cx] = CELL_CAMP;
            m->campfire_x = cx; m->campfire_y = cy;
        }
    }

    // вход в пещеру
    for (int t = 0; t < 400; t++) {
        int ox = 1 + rand() % (w-2), oy = 1 + rand() % (h-2);
        if ((ox == fx && oy == fy) || m->cells[oy][ox] != CELL_FLOOR) continue;
        m->cells[oy][ox] = CELL_CAVE; break;
    }
}

// ---- пещера -------------------------------------------------------------

void generate_cave(struct Map *m, struct Player *player)
{
    int h = MIN_CAVE_SIZE + rand() % (MAX_CAVE_SIZE - MIN_CAVE_SIZE + 1);
    int w = MIN_CAVE_SIZE + rand() % (MAX_CAVE_SIZE - MIN_CAVE_SIZE + 1);
    resize_map_cells(m, h, w);

    m->type = LOC_CAVE;

    // стены по краю, внутри — камни/пол
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            int edge = (x == 0 || y == 0 || x == w-1 || y == h-1);
            m->cells[y][x] = edge ? CELL_WALL
                                  : (rand() % 100 < 25 ? CELL_ROCK : CELL_FLOOR);
        }

    // герой — первая свободная
    int hx = -1, hy = -1;
    for (int y = 1; y < h - 1 && hx == -1; y++)
        for (int x = 1; x < w - 1; x++)
            if (m->cells[y][x] == CELL_FLOOR) { hx = x; hy = y; break; }
    m->cells[hy][hx] = CELL_HERO;
    player->x = hx; player->y = hy;

    // прорубаем камни, пока достижимая область < 1/3
    int need = ((h-2) * (w-2)) / 3; if (need < 3) need = 3;
    char reach[MAX_FOREST_SIZE][MAX_FOREST_SIZE];
    for (int t = 0; t < 100; t++) {
        if (bfs_reach(m, hx, hy, reach) >= need) break;
        int done = 0;
        for (int y = 1; y < h - 1 && !done; y++)
            for (int x = 1; x < w - 1 && !done; x++) {
                if (m->cells[y][x] != CELL_ROCK) continue;
                if ((y > 0 && reach[y-1][x]) || (y < h-1 && reach[y+1][x]) ||
                    (x > 0 && reach[y][x-1]) || (x < w-1 && reach[y][x+1])) {
                    m->cells[y][x] = CELL_FLOOR; done = 1;
                }
            }
        if (!done) break;
    }
    bfs_reach(m, hx, hy, reach);

    // O — достижимая, не в тупике, максимально далеко
    int best = -1, ox = -1, oy = -1;
    for (int y = 1; y < h - 1; y++)
        for (int x = 1; x < w - 1; x++) {
            if (!reach[y][x] || m->cells[y][x] != CELL_FLOOR) continue;
            if (x == hx && y == hy) continue;
            int open = 0;
            if (m->cells[y-1][x] == CELL_FLOOR || m->cells[y-1][x] == CELL_HERO) open++;
            if (m->cells[y+1][x] == CELL_FLOOR || m->cells[y+1][x] == CELL_HERO) open++;
            if (m->cells[y][x-1] == CELL_FLOOR || m->cells[y][x-1] == CELL_HERO) open++;
            if (m->cells[y][x+1] == CELL_FLOOR || m->cells[y][x+1] == CELL_HERO) open++;
            if (open < 2) continue;
            int d = (x-hx)*(x-hx) + (y-hy)*(y-hy);
            if (d > best) { best = d; ox = x; oy = y; }
        }

    // fallback: прорубаем соседа
    if (ox == -1) {
        const int dxs[4] = {0,0,-1,1}, dys[4] = {-1,1,0,0};
        for (int d = 0; d < 4; d++) {
            int nx = hx+dxs[d], ny = hy+dys[d];
            if (nx <= 0 || nx >= w-1 || ny <= 0 || ny >= h-1) continue;
            if (m->cells[ny][nx] == CELL_ROCK) m->cells[ny][nx] = CELL_FLOOR;
            if (m->cells[ny][nx] == CELL_FLOOR) { ox = nx; oy = ny; break; }
        }
    }
    if (ox != -1) m->cells[oy][ox] = CELL_CAVE;
}

// ---- вывод карты --------------------------------------------------------

void print_map(struct Map *m)
{
    printf("\n");
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) putchar(m->cells[y][x]);
        putchar('\n');
    }
}

// ---- ход ----------------------------------------------------------------

int move_hero(struct Map *m, struct Player *player, int dx, int dy)
{
    int nx = player->x + dx, ny = player->y + dy;
    if (nx < 0 || nx >= m->width || ny < 0 || ny >= m->height)
        return (m->type == LOC_FOREST) ? MOVE_NEW_FOREST : MOVE_BLOCKED;

    char c = m->cells[ny][nx];
    if (c == CELL_TREE || c == CELL_ROCK || c == CELL_WALL) return MOVE_BLOCKED;

    int at_camp = (m->type == LOC_FOREST &&
                   player->x == m->campfire_x && player->y == m->campfire_y);
    m->cells[player->y][player->x] = at_camp ? CELL_CAMP : CELL_FLOOR;

    player->x = nx; player->y = ny;
    if (c == CELL_CAVE) return MOVE_CAVE_ENTRANCE;
    m->cells[ny][nx] = CELL_HERO;
    if (c == CELL_CAMP) return MOVE_CAMP;
    return MOVE_OK;
}

// ---- приключение --------------------------------------------------------

void run_adventure(struct Player *player, int *day, int *hour)
{
    if (!player) { printf("Сначала создайте персонажа!\n"); return; }
    if (player->stamina <= 0) { printf("Нет выносливости! Отдохните.\n"); return; }
    if (*hour >= NIGHT_HOUR && *hour < SLEEP_HOUR)
    { printf("Сейчас ночь — фермер должен спать!\n"); return; }

    struct Map *m = create_map(MIN_FOREST_SIZE, MIN_FOREST_SIZE);
    if (!m) { printf("Ошибка памяти!\n"); return; }

    generate_forest(m, player);
    int running = 1, evacuate = 0;

    while (running) {
        print_map(m);
        printf("HP:%d/%d | Вын:%d/%d | Золото:%d | Этаж:%d | Время:%d:00\n",
               player->hp, player->max_hp, player->stamina, player->max_stamina,
               player->gold, m->floors_passed + 1, *hour);
        printf("Вес:%d/%d [%s]\n", player->backpack.current_weight,
               player->backpack.max_capacity,
               load_state_name(player->backpack.state));
        printf("Ход (WASD / Q) > ");

        char cmd; int ch;
        if (scanf(" %c", &cmd) != 1) break;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
        cmd = toupper((unsigned char)cmd);

        if (cmd == 'Q') {
            if (m->type == LOC_FOREST &&
                player->x == m->campfire_x && player->y == m->campfire_y) {
                printf("Домой! Этажей: %d, золото: %d\n", m->floors_passed, player->gold);
                running = 0;
            } else if (m->type == LOC_FOREST)
                printf("Домой только с костра ^.\n");
            else
                printf("Из пещеры — ищите O.\n");
            continue;
        }

        int dx = 0, dy = 0;
        if      (cmd == 'W') dy = -1;
        else if (cmd == 'S') dy =  1;
        else if (cmd == 'A') dx = -1;
        else if (cmd == 'D') dx =  1;
        else { printf("?\n"); continue; }

        int r = move_hero(m, player, dx, dy);

        if (r == MOVE_BLOCKED) printf("Туда нельзя.\n");
        else if (r == MOVE_CAMP) printf("У костра. Q — домой.\n");
        else if (r == MOVE_OK) {
            int cost = step_stamina_cost(&player->backpack);
            player->stamina -= cost;
            if (cost == 3) printf("Перегруз! -%d вын.\n", cost);
            if (player->stamina <= 0) { printf("Без сил...\n"); evacuate = 1; running = 0; }
        }
        else if (r == MOVE_NEW_FOREST || r == MOVE_CAVE_ENTRANCE) {
            int was_forest = (m->type == LOC_FOREST);
            int old_x = player->x, old_y = player->y;

            free_map(m);
            m = create_map(MIN_FOREST_SIZE, MIN_FOREST_SIZE);
            if (!m) { printf("Ошибка памяти!\n"); return; }

            if (was_forest) {
                generate_cave(m, player);
                printf("Вы вошли в пещеру...\n");
            } else {
                m->floors_passed++;
                generate_forest(m, player);

                *hour += FLOOR_TIME_COST;
                if (*hour >= HOURS_PER_DAY) { *hour %= HOURS_PER_DAY; (*day)++; }
                player->stamina -= FLOOR_STAMINA_COST;
                player->gold += GOLD_MULTIPLIER * m->floors_passed;

                int gx = old_x, gy = old_y;
                if (dy == -1) gy = m->height - 2;
                if (dy ==  1) gy = 1;
                if (dx == -1) gx = m->width - 2;
                if (dx ==  1) gx = 1;
                if (gx < 1) gx = 1;
                if (gx >= m->width  - 1) gx = m->width  - 2;
                if (gy < 1) gy = 1;
                if (gy >= m->height - 1) gy = m->height - 2;
                if (m->cells[gy][gx] != CELL_FLOOR) {
                    for (int ry = 1; ry < m->height - 1; ry++) {
                        int done = 0;
                        for (int rx = 1; rx < m->width - 1; rx++)
                            if (m->cells[ry][rx] == CELL_FLOOR) { gx = rx; gy = ry; done = 1; break; }
                        if (done) break;
                    }
                }
                m->cells[player->y][player->x] = CELL_FLOOR;
                player->x = gx; player->y = gy;
                m->cells[gy][gx] = CELL_HERO;

                printf("Новый лес. Этаж: %d, золото: %d\n", m->floors_passed + 1, player->gold);
            }

            if (*hour >= NIGHT_HOUR && *hour < SLEEP_HOUR) { printf("Ночь...\n"); evacuate = 1; running = 0; }
            else if (player->stamina <= 0) { printf("Без сил...\n"); evacuate = 1; running = 0; }
        }
    }

    if (evacuate) {
        player->gold /= GOLD_EVAC_PENALTY;
        printf("Эвакуация! Золото: %d\n", player->gold);
    }
    free_map(m);
}
