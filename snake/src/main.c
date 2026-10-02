#include <graphx.h>
#include <keypadc.h>
#include <sys/rtc.h>
#include <sys/timers.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define CELL 8
#define COLS 40
#define ROWS 28
#define TOP 16
#define MAXLEN (COLS * ROWS)

static int bx[MAXLEN];
static int by[MAXLEN];
static int head;
static int len;
static int dirx, diry;
static int ndx, ndy;
static int foodx, foody;
static int score;
static bool dead;
static bool won;
static bool quit;

static bool occupied(int x, int y, int count) {
    int i;
    for (i = 0; i < count; i++) {
        int p = (head - i + MAXLEN) % MAXLEN;
        if (bx[p] == x && by[p] == y) {
            return true;
        }
    }
    return false;
}

static void place_food(void) {
    int guard = 0;
    if (len >= MAXLEN) {
        won = true;
        return;
    }
    do {
        foodx = rand() % COLS;
        foody = rand() % ROWS;
        guard++;
    } while (occupied(foodx, foody, len) && guard < MAXLEN * 4);
}

static void reset_game(void) {
    int i;
    len = 4;
    head = 3;
    for (i = 0; i < len; i++) {
        bx[i] = 8 + i;
        by[i] = ROWS / 2;
    }
    dirx = 1;
    diry = 0;
    ndx = 1;
    ndy = 0;
    score = 0;
    dead = false;
    won = false;
    place_food();
}

static void consider(int x, int y) {
    if (x == -dirx && y == -diry) {
        return;
    }
    ndx = x;
    ndy = y;
}

static void poll_keys(void) {
    kb_Scan();
    if (kb_Data[6] & kb_Clear) {
        quit = true;
        return;
    }
    if (kb_Data[7] & kb_Up) {
        consider(0, -1);
    } else if (kb_Data[7] & kb_Down) {
        consider(0, 1);
    } else if (kb_Data[7] & kb_Left) {
        consider(-1, 0);
    } else if (kb_Data[7] & kb_Right) {
        consider(1, 0);
    }
}

static void step(void) {
    int nx, ny, check;

    dirx = ndx;
    diry = ndy;
    nx = bx[head] + dirx;
    ny = by[head] + diry;
    if (nx < 0 || ny < 0 || nx >= COLS || ny >= ROWS) {
        dead = true;
        return;
    }
    check = (nx == foodx && ny == foody) ? len : len - 1;
    if (occupied(nx, ny, check)) {
        dead = true;
        return;
    }
    head = (head + 1) % MAXLEN;
    bx[head] = nx;
    by[head] = ny;
    if (nx == foodx && ny == foody) {
        len++;
        score++;
        if (len >= MAXLEN) {
            won = true;
            return;
        }
        place_food();
    }
}

static void draw(void) {
    int i;
    gfx_FillScreen(0xFF);
    gfx_SetTextFGColor(0x00);
    gfx_SetTextBGColor(0xFF);
    gfx_PrintStringXY("Score ", 4, 4);
    gfx_SetTextXY(52, 4);
    gfx_PrintInt(score, 3);
    if (dead) {
        gfx_PrintStringXY("Dead  enter=again clear=quit", 90, 4);
    } else if (won) {
        gfx_PrintStringXY("Full  enter=again clear=quit", 90, 4);
    }
    gfx_SetColor(0x00);
    for (i = 0; i < len; i++) {
        int p = (head - i + MAXLEN) % MAXLEN;
        gfx_FillRectangle(bx[p] * CELL, TOP + by[p] * CELL, CELL - 1, CELL - 1);
    }
    gfx_SetColor(0x03);
    gfx_FillRectangle(foodx * CELL, TOP + foody * CELL, CELL - 1, CELL - 1);
    gfx_SwapDraw();
}

static void wait_ticks(int slices) {
    int i;
    for (i = 0; i < slices && !quit; i++) {
        poll_keys();
        delay(15);
    }
}

int main(void) {
    srand((unsigned)rtc_Time());
    gfx_Begin();
    gfx_SetDrawBuffer();
    reset_game();
    while (!quit) {
        draw();
        if (dead || won) {
            kb_Scan();
            while (!(kb_Data[6] & kb_Enter) && !(kb_Data[6] & kb_Clear)) {
                kb_Scan();
                delay(20);
            }
            if (kb_Data[6] & kb_Clear) {
                break;
            }
            reset_game();
            continue;
        }
        wait_ticks(6);
        if (quit) {
            break;
        }
        step();
    }
    gfx_End();
    return 0;
}
