//
// Created by kazuhiro takami on 2022/08/12.
//

#ifndef TETRIS_TETRIS_H
#define TETRIS_TETRIS_H

#include "config.h"
#include "tetromino.h"
#include <sys/time.h>

typedef char t_board[ROW][COL];
typedef struct timeval t_timeval;

typedef struct {
    int score;
    bool game_on;
    t_board board;
    double gravity;
    t_tetromino current;
    t_timeval updated_at;
} t_context;

#endif//TETRIS_TETRIS_H
