//
// Created by kazuhiro takami on 2022/08/12.
//

#ifndef TETRIS_GRAVITY_H
#define TETRIS_GRAVITY_H
#include <stdbool.h>
#include "tetromino.h"
#include "tetris.h"

void apply_gravity(t_context *ctx);
int is_time_to_fall(t_timeval updated_at, double gravity);
static void lock_tetromino_to_board(const t_tetromino piece, t_board board);
static bool is_line_filled(int row, const t_board board);
static void clear_line(int row, t_board board);
static int	clear_filled_lines(t_board board);
static double next_gravity(double gravity, int lines_cleared);
#endif//TETRIS_GRAVITY_H
