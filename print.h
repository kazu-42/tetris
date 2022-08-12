//
// Created by kazuhiro takami on 2022/08/12.
//

#ifndef TETRIS_PRINT_H
#define TETRIS_PRINT_H

#include "tetris.h"

static void printw_current_screen(const t_board board, const t_tetromino current, int score);
static void print_result(int score, const t_board board);

#endif//TETRIS_PRINT_H
