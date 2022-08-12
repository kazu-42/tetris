/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tetris_tetromino.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: susami <susami@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/12 16:55:45 by susami            #+#    #+#             */
/*   Updated: 2022/08/12 17:01:57 by susami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/tetris.h"
#include <stdlib.h>

#define NUM_TETRIMINOS 7
t_tetromino generate_random_tetromino(void);
t_tetromino duplicate_tetromino(const t_tetromino piece);
void destroy_tetromino(t_tetromino piece);
bool is_valid_position(const t_tetromino piece, const t_board board);

static const t_tetromino tetrominoes[NUM_TETRIMINOS] = {
        // shape S
        {
                .array = (char *[]){
                        (char[]){0, 1, 1},
                        (char[]){1, 1, 0},
                        (char[]){0, 0, 0}},
                .length = 3,
                .row = 0,
                .col = 0},
        // shape Z
        {.array = (char *[]){(char[]){1, 1, 0}, (char[]){0, 1, 1}, (char[]){0, 0, 0}}, .length = 3, .row = 0, .col = 0},
        // shape T
        {.array = (char *[]){(char[]){0, 0, 0}, (char[]){1, 1, 1}, (char[]){0, 1, 0}}, .length = 3, .row = 0, .col = 0},
        // shape L
        {.array = (char *[]){(char[]){1, 0, 0}, (char[]){1, 0, 0}, (char[]){1, 1, 0}}, .length = 3, .row = 0, .col = 0},
        // shape J
        {.array = (char *[]){(char[]){0, 0, 1}, (char[]){0, 0, 1}, (char[]){0, 1, 1}}, .length = 3, .row = 0, .col = 0},
        // shape O
        {.array = (char *[]){(char[]){1, 1}, (char[]){1, 1}}, .length = 2, .row = 0, .col = 0},
        // shape I
        {.array = (char *[]){(char[]){0, 0, 1, 0}, (char[]){0, 0, 1, 0}, (char[]){0, 0, 1, 0}, (char[]){0, 0, 1, 0}}, .length = 4, .row = 0, .col = 0}};

t_tetromino generate_random_tetromino(void) {
    t_tetromino new_piece = duplicate_tetromino(tetrominoes[rand() % NUM_TETRIMINOS]);
    new_piece.col = rand() % (COL_SIZE - new_piece.length + 1);
    new_piece.row = 0;
    return new_piece;
}

t_tetromino duplicate_tetromino(const t_tetromino piece) {
    t_tetromino new_piece = {
            .array = malloc(sizeof(char *) * (unsigned int) piece.length),
            .length = piece.length,
            .row = piece.row,
            .col = piece.col};

    for (int r = 0; r < new_piece.length; r++) {
        new_piece.array[r] = (char *) malloc(sizeof(char) * (unsigned int) new_piece.length);
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
            if (c < 0 || c >= COL_SIZE || r >= ROW_SIZE)
                return false;
            if (board[r][c])
                return false;
        }
    }
    return true;
}
