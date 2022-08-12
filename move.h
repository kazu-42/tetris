#ifndef TETRIS_MOVE_H
#define TETRIS_MOVE_H

#include <stdbool.h>
#include "tetromino.h"
#include "tetris.h"

typedef enum {
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_ROTATE,
    MOVE_UNKNOWN,
} t_move;

t_move to_move(int ch);
void apply_move(t_move move, t_tetromino *piece);
bool try_move(t_move move, t_tetromino *piece, const t_board board);
static void move_down(t_tetromino *piece);
static void move_right(t_tetromino *piece);
static void move_left(t_tetromino *piece);
static void rotate_tetromino(t_tetromino *piece);
static bool has_room_to_move(t_move move, const t_tetromino piece, const t_board board);

#endif//TETRIS_MOVE_H
