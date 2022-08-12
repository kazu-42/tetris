//
// Created by kazuhiro takami on 2022/08/12.
//

#include "tetromino.h"
#include <stdlib.h>
#include "tetris.h"

t_tetromino generate_random_tetromino(void) {
    t_tetromino new_piece = duplicate_tetromino(tetrominoes[rand() % NUM_TETRIMINOS]);
    new_piece.col = rand() % (COL - new_piece.length + 1);
    new_piece.row = 0;
    return new_piece;
}

t_tetromino duplicate_tetromino(const t_tetromino piece) {
    t_tetromino new_piece = {
            .array = malloc(sizeof(char *) * (unsigned int)piece.length),
            .length = piece.length,
            .row = piece.row,
            .col = piece.col};

    for (int r = 0; r < new_piece.length; r++) {
        new_piece.array[r] = (char *) malloc(sizeof(char) * (unsigned int)new_piece.length);
        for (int c = 0; c < new_piece.length; c++) {
            new_piece.array[r][c] = piece.array[r][c];
        }
    }
    return new_piece;
}

void destroy_tetromino(t_tetromino piece) {
    for (int r = 0; r < piece.length; r++) {
        free(piece.array[r]);
    }
    free(piece.array);
}

bool is_valid_position(const t_tetromino piece, const t_board board) {
    for (int i = 0; i < piece.length; i++) {
        for (int j = 0; j < piece.length; j++) {
            if (!piece.array[i][j])
                continue;
            int r = piece.row + i;
            int c = piece.col + j;
            if (c < 0 || c >= COL || r >= ROW)
                return false;
            if (board[r][c])
                return false;
        }
    }
    return true;
}
