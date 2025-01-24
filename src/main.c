#include <stdio.h>
#include <stdlib.h>
#include <gol.h>

/////////////////////////////////
// main

int main(void){
    system("cls");

    cw_grid *my_grid = cw_grid__init(50, 50);

    cw_grid__draw_pattern(my_grid, 10, 10, 
        L" O " "\n"
        L"  O" "\n"
        L"OOO" "\n"
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

        fprintf_s(stdout, "Generation: %d \n", generation_c);
        fprintf_s(stdout, "Population: %d \n", population_c);

        fprintf_s(stdout, "\n[SPACE]  | pause/unpause\n");
        fprintf_s(stdout, "[ESCAPE] | quit\n");

        cw_game__update(my_grid);

        Sleep(CW_TICK_SPEED);

        generation_c++;
    }
    return 0;
}