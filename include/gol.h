// conway's game of life, in C
// made by: lucca
// date: 2025-01-22 (started) - 2025-01-23 (ended)
// license: MIT




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>    // wchar_t
#include <stdbool.h>




/////////////////////////////////
// definitions

#ifndef _CW_H
#define _CW_H

#define CW_PXCHAR (wchar_t) L'█'
#define CW_WSTRING (wchar_t*) L"\x1b[38;2;%d;%d;%dm"
#define CW_WHOME (wchar_t*) L"\x1b[H"
#define CW_WRESET (wchar_t*) L"\x1b[0m"
#define CW_STATIC__DEF static
#define CW_CELL_COLOR__DEAD (COLORREF)RGB(255.0f, 255.0f, 255.0f)
#define CW_CELL_COLOR__ALIVE (COLORREF)RGB(0.0f, 0.0f, 0.0f)
#define CW_CELL_COLOR__MOUSE (COLORREF)RGB(190.0f, 190.0f, 190.0f)
#define CW_TICK_SPEED 50//ms

#define CW_WNEWL L'\n'

#define DEAD (const int)  0
#define ALIVE (const int) 1

#define OOB (const int)  -1

#endif 

/////////////////////////////////
// globals

CW_STATIC__DEF HANDLE cw_handler__console;
CW_STATIC__DEF int cw_mouse__position_x = -1, cw_mouse__position_y = -1;
CW_STATIC__DEF bool paused = false;
CW_STATIC__DEF bool running = true;

/////////////////////////////////
// grid

typedef struct cw_grid_t {
    int width;
    int height;
    int **cells; 
} cw_grid;

cw_grid *cw_grid__init(int width, int height) {
    cw_grid *g = malloc(sizeof(cw_grid));
    
    g->width = width;
    g->height = height;

    g->cells = malloc(sizeof(int*) * height);
    for (int i = 0; i < g->height; i++){
        g->cells[i] = malloc(sizeof(int) * g->width);
        for (int j = 0; j < g->width; j++){
            g->cells[i][j] = DEAD;
        }
    }

    ////////////////////////////////////////////////////////
    cw_handler__console = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE handler_input = GetStdHandle(STD_INPUT_HANDLE);
    DWORD fdw = 0;
    fdw |= ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS;
    SetConsoleMode(handler_input, fdw);
    ////////////////////////////////////////////////////////

    return g;
}

/////////////////////////////////
// fetches, checks and rules

int *cw_cell__fetch_adjacent(cw_grid g, int g_x, int g_y){
    if (g_x < 0 || g_x >= g.width || g_y < 0 || g_y >= g.height) return NULL;
    int cell = g.cells[g_y][g_x];
    // To fetch all surrounding cells to the corresponding (g_x, g_y)
    // cell, we must first consider the options that this cell is
    // located at one of the 4 corners of the grid, or at one of its
    // 4 sides.
    int *adjacent = malloc(8 * sizeof(int));
    int idx=0;
    for(int y = -1; y <= 1; y++) { 
        for(int x = -1; x <= 1; x++) {
            if(x == 0 && y == 0) continue;
            
            if (g_x+x >= 0 && g_x+x < g.width &&
                g_y+y >= 0 && g_y+y < g.height){
                adjacent[idx++] = g.cells[g_y + y][g_x + x];
            } else {
                adjacent[idx++] = OOB;
            }
            
            
        }
    }

    return adjacent;
}

int cw_cell__fetch_adjacent__alive(cw_grid g, int *adj){
    int live_count = 0;
    for (int i = 0; i < 8; i++){
        if (adj[i] == ALIVE) live_count++;
    }
    return live_count;
}

int cw_grid__fetch_alive(cw_grid g){
    int live_count = 0;
    for (int y = 0; y < g.height; y++){
        for (int x = 0; x < g.width; x++){
            if (g.cells[y][x] == ALIVE) live_count++;
        }
    }
    return live_count;
}

CW_STATIC__DEF bool cw_game__rule_underpopulation(cw_grid g, int *adj, int alive_c, int g_x, int g_y){
    // 1. Any live cell with fewer than two live neighbours dies, 
    // as if by underpopulation.
    if (g.cells[g_y][g_x] == DEAD) return false;
    return (alive_c < 2);
}
CW_STATIC__DEF bool cw_game__rule_survival(cw_grid g, int *adj, int alive_c, int g_x, int g_y){
    // 2. Any live cell with two or three live neighbours lives on 
    // to the next generation.
    if (g.cells[g_y][g_x] == DEAD) return false;
    return (alive_c == 2 || alive_c == 3);
}
CW_STATIC__DEF bool cw_game__rule_overpopulation(cw_grid g, int *adj, int alive_c, int g_x, int g_y){
    // 3. Any live cell with more than three live neighbours dies, 
    // as if by overpopulation.
    if (g.cells[g_y][g_x] == DEAD) return false;
    return (alive_c > 3);
}
CW_STATIC__DEF bool cw_game__rule_reproduction(cw_grid g, int *adj, int alive_c, int g_x, int g_y){
    // 4. Any dead cell with exactly three live neighbours becomes a
    // live cell, as if by reproduction.
    if (g.cells[g_y][g_x] == ALIVE) return false;
    return (alive_c == 3);
}

/////////////////////////////////
// event handling

CW_STATIC__DEF void cw_mouse__press_left(cw_grid *g){
    if (cw_mouse__position_x < 0 || cw_mouse__position_x >= g->width)  return;
    if (cw_mouse__position_y < 0 || cw_mouse__position_y >= g->height) return;

    g->cells[cw_mouse__position_y][cw_mouse__position_x] = ALIVE;
}

CW_STATIC__DEF void cw_mouse__press_right(cw_grid *g){
    if (cw_mouse__position_x < 0 || cw_mouse__position_x >= g->width)  return;
    if (cw_mouse__position_y < 0 || cw_mouse__position_y >= g->height) return;

    g->cells[cw_mouse__position_y][cw_mouse__position_x] = DEAD;
}

CW_STATIC__DEF void cw_key__press(KEY_EVENT_RECORD ker){
    switch (ker.wVirtualKeyCode) {
        case VK_SPACE: paused = !paused; break;
        case VK_ESCAPE: running = false; break;
    }
}

void cw_input_handle__update(cw_grid *g){
    DWORD numEvents = 0;
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    GetNumberOfConsoleInputEvents(hInput, &numEvents);
    // If there aren't any input events in the console buffer, we can return early
    // to avoid unnecessary processing. This helps optimize CPU usage when no input
    // is occurring.
    if (numEvents > 0) {
        INPUT_RECORD *inBuffer = malloc(sizeof(INPUT_RECORD) * numEvents);
        DWORD numRead = 0;
        ReadConsoleInput(hInput, inBuffer, numEvents, &numRead);
        // For each event in the console input buffer, we'll need to read and process it.
        // Most events will be mouse movements, but we should be prepared to handle other
        // types of input events as well.
        for (DWORD i = 0; i < numRead; i++) {
            if (inBuffer[i].EventType == MOUSE_EVENT) {
                MOUSE_EVENT_RECORD mer = inBuffer[i].Event.MouseEvent;
                
                SHORT cx = mer.dwMousePosition.X;
                SHORT cy = mer.dwMousePosition.Y;
                cw_mouse__position_x = cx / 2;
                cw_mouse__position_y = cy;

                if (mer.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) cw_mouse__press_left(g);
                if (mer.dwButtonState & RIGHTMOST_BUTTON_PRESSED) cw_mouse__press_right(g);
            }

            if (inBuffer[i].EventType == KEY_EVENT) {
                KEY_EVENT_RECORD ker = inBuffer[i].Event.KeyEvent;

                if (ker.bKeyDown) {
                    cw_key__press(ker);
                }
            }
        }
        free(inBuffer);
    }
}

/////////////////////////////////
// renders

void *cw_grid__render(cw_grid g){
    // We pre-allocate a wide character buffer large enough to hold the entire rendered frame,
    // which is more efficient than printing character by character. The buffer size calculation
    // accounts for color codes, grid cells, and newlines.
    size_t wbuffer_s = g.height * (g.width * 30 + 10) * sizeof(wchar_t);
    wchar_t *wbuffer = (wchar_t *)malloc(wbuffer_s * 2);
    size_t wbuffer_c= 0;
    // start by moving the cursor to the home position (top-left) using ANSI escape sequence.
    // This ensures each frame overwrites the previous one, creating a "tick" effect.
    wbuffer_c += swprintf(&wbuffer[wbuffer_c], wbuffer_s - wbuffer_c, CW_WHOME);
    for (int y = 0; y < g.height; y++){
        for (int x = 0; x < g.width; x++){
            COLORREF color;
            if (g.cells[y][x] == DEAD) color = CW_CELL_COLOR__DEAD;
            if (g.cells[y][x] == ALIVE) color = CW_CELL_COLOR__ALIVE;
            ///////////////////////////////////////////////////////////////////////////////////////////////
            if ((x == cw_mouse__position_x) && (y == cw_mouse__position_y)) color = CW_CELL_COLOR__MOUSE;
            ///////////////////////////////////////////////////////////////////////////////////////////////
            int red = GetRValue(color); int green = GetGValue(color); int blue = GetBValue(color);
            wbuffer_c += swprintf(&wbuffer[wbuffer_c], wbuffer_s - wbuffer_c, CW_WSTRING, red, green, blue);
            wbuffer[wbuffer_c++] = CW_PXCHAR;
            wbuffer_c += swprintf(&wbuffer[wbuffer_c], wbuffer_s - wbuffer_c, CW_WSTRING, red, green, blue);
            wbuffer[wbuffer_c++] = CW_PXCHAR;
        }
        wbuffer[wbuffer_c++] = CW_WNEWL;
    }
    // Reset the console colors to their defaults using another escape sequence, then 
    // write the entire buffer to the console in a single operation, which is much more 
    // efficient than multiple individual writes.
    wbuffer_c += swprintf(&wbuffer[wbuffer_c], wbuffer_s - wbuffer_c, CW_WRESET);
    DWORD written;
    WriteConsoleW(cw_handler__console, wbuffer, (DWORD)wbuffer_c, &written, NULL);

    free(wbuffer);
}

/////////////////////////////////
// update loops & stuff

int cw_game__update(cw_grid *g){
    // Conway's game of life must not be run sequentially --- the whole point is to make
    // all rules be applied simultaneously to all cells. In other words, we need a buffer
    // for the cell updates.
    int **new = malloc(sizeof(int*)*g->height);
    for (int i = 0; i < g->height; i++) {
        new[i] = malloc(sizeof(int) * g->width);
        memcpy(new[i], g->cells[i], sizeof(int) * g->width);
    }
    // The next step must include the actual rules check, followed by updating the
    // buffer. 
    for (int y = 0; y < g->height; y++){
        for (int x = 0; x < g->width; x++){
            int *adj = cw_cell__fetch_adjacent(*g, x, y);
            if (adj == NULL) continue;
            int alive_c = cw_cell__fetch_adjacent__alive(*g, adj);

            if (cw_game__rule_underpopulation(*g, adj, alive_c, x, y)) new[y][x] = DEAD;
            if (cw_game__rule_overpopulation(*g, adj, alive_c,x, y)) new[y][x]   = DEAD;
            if (cw_game__rule_reproduction(*g, adj,alive_c, x, y)) new[y][x]     = ALIVE;

            free(adj);
        }
    }
    // In the final step, we update the actual grid cells with the allocated buffer, 
    // which includes all updates, and apply them as-is (thus making it simultaneous).
    for (int i = 0; i < g->height; i++) {
        memcpy(g->cells[i], new[i], sizeof(int) * g->width);
        free(new[i]);
    }
    free(new);
}

/////////////////////////////////
// helpers for drawing patterns

void cw_grid__draw_alive(cw_grid *g, int x, int y){
    if (x < 0 || x >= g->width || y < 0 || y >= g->height) return;
    g->cells[y][x] = ALIVE;
}

void cw_grid__draw_pattern(cw_grid *g, int x, int y, const wchar_t *pattern) {
    int curr_x = 0;
    int curr_y = 0;
    // Run through all wide-chars of the wide-string and construct a pattern,
    // which we will position on the grid based on the provided x-y positions. 
    for (int i = 0; pattern[i] != L'\0'; i++) {
        if (pattern[i] == CW_WNEWL) {
            curr_y++;
            curr_x = 0;
            continue;
        }
        if (pattern[i] != L' ') cw_grid__draw_alive(g, x + curr_x, y + curr_y);
        curr_x++;
    }
}