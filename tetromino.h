#ifndef TETRIS_TETROMINO_H
#define TETRIS_TETROMINO_H

#include <stdbool.h>
#include "tetris.h"

typedef struct {
    char **array;
    int length, row, col;
} t_tetromino;

t_tetromino generate_random_tetromino(void);
t_tetromino duplicate_tetromino(const t_tetromino piece);
void destroy_tetromino(t_tetromino piece);
bool is_valid_position(const t_tetromino piece, const t_board board);

#endif//TETRIS_TETROMINO_H
