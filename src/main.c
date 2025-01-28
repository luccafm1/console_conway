#include <stdio.h>
#include <stdlib.h>
#define CW_CELL__CHECKER
#include <gol.h>

/////////////////////////////////
// main

int main(void){
    system("cls");

    cw_grid__ptr my_grid = cw_grid__init(100, 50);

    cw_grid__draw_pattern(my_grid, 10, 10, 
        L" O " "\n"
        L"  O" "\n"
        L"OOO" "\n"
    );

    while (running) {
        ///////////////////////////////////////////////////////////////
        cw_input_handle__update(my_grid);
        cw_grid__render(*my_grid);
        ///////////////////////////////////////////////////////////////

        size_t generation_c = my_grid->gen;
        size_t population_c = cw_grid__fetch_alive(*my_grid);

        fprintf_s(stdout, "Generation: %d \n", generation_c);
        fprintf_s(stdout, "Population: %d \n", population_c);

        fprintf_s(stdout, "\n[BACKSPACE] | clear\n");
        fprintf_s(stdout, "[SPACE]     | pause/unpause\n");
        fprintf_s(stdout, "[ESCAPE]    | quit\n");

        cw_game__update(my_grid);
    }

    cw_grid__free(my_grid);

    return 0;
}