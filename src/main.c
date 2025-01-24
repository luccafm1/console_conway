#include <stdio.h>
#include <stdlib.h>
#include <gol.h>

/////////////////////////////////
// main

int main(void){
    system("cls");

    cw_grid *my_grid = cw_grid__init(50, 50);

    cw_grid__draw_pattern(my_grid, 10, 10, 
        L" █ " "\n"
        L"  █" "\n"
        L"███" "\n"
    );

    size_t generation_c = 0;
    size_t population_c = 0;

    while (running) {
        ///////////////////////////////////////////////////////////////
        cw_input_handle__update(my_grid);
        cw_grid__render(*my_grid);
        ///////////////////////////////////////////////////////////////

        if (paused) continue;

        population_c = cw_grid__fetch_alive(*my_grid);

        fwprintf_s(stdout, L"Generation: %d\n", generation_c);
        fwprintf_s(stdout, L"Population: %d\n", population_c);

        fwprintf_s(stdout, L"\n[SPACE]  | pause/unpause\n");
        fwprintf_s(stdout, L"[ESCAPE] | quit\n");

        cw_game__update(my_grid);

        Sleep(CW_TICK_SPEED);

        generation_c++;
    }
    return 0;
}