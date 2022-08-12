/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tetris_tetromino.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: susami <susami@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/12 16:55:45 by susami            #+#    #+#             */
/*   Updated: 2022/08/13 01:57:19 by susami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <ncurses.h>
#include "tetris.h"

#define TETROMINO_S 1
#define TETROMINO_Z 2
#define TETROMINO_T 3
#define TETROMINO_L 4
#define TETROMINO_J 5
#define TETROMINO_O 6
#define TETROMINO_I 7

#define NUM_TETRIMINOS 7

t_tetromino generate_random_tetromino(void);

t_tetromino duplicate_tetromino(const t_tetromino piece);

void destroy_tetromino(t_tetromino piece);

bool is_valid_position(const t_tetromino piece, const t_board board);

void merge_tetromino_to_board(const t_tetromino piece, t_board board);

void init_curses_tetromino_colors(void);

// Non-zero value in array represents the cell of the tetromino.
// The value varies to express colors of the shape.
//
// SEE ALSO: init_curses_tetromino_colors(void)
static const t_tetromino tetrominoes[NUM_TETRIMINOS] = {
        // 1. shape S
        //   # #
        // # #
        {
                .array = (char *[]) {
                        (char[]) {0, 1, 1},
                        (char[]) {1, 1, 0},
                        (char[]) {0, 0, 0}},
                .length = 3,
                .position = {0}
        },
        // 2. shape Z
        // # #
        //   # #
        {
                .array = (char *[]) {
                        (char[]) {2, 2, 0},
                        (char[]) {0, 2, 2},
                        (char[]) {0, 0, 0}},
                .length = 3,
                .position = {0}
        },
        // 3. shape T
        // # # #
        //   #
        {
                .array = (char *[]) {
                        (char[]) {0, 0, 0},
                        (char[]) {3, 3, 3},
                        (char[]) {0, 3, 0}},
                .length = 3,
                .position = {0}
        },
        // 4. shape L
        // #
        // #
        // # #
        {
                .array = (char *[]) {
                        (char[]) {4, 0, 0},
                        (char[]) {4, 0, 0},
                        (char[]) {4, 4, 0}},
                .length = 3,
                .position = {0}
        },
        // 5. shape J
        //     #
        //     #
        //   # #
        {
                .array = (char *[]) {
                        (char[]) {0, 0, 5},
                        (char[]) {0, 0, 5},
                        (char[]) {0, 5, 5}},
                .length = 3,
                .position = {0}
        },
        // 6. shape O
        // # #
        // # #
        {
                .array = (char *[]) {
                        (char[]) {6, 6},
                        (char[]) {6, 6}},
                .length = 2,
                .position = {0}
        },
        // 7. shape I
        //     #
        //     #
        //     #
        //     #
        {
                .array = (char *[]) {
                        (char[]) {0, 0, 7, 0},
                        (char[]) {0, 0, 7, 0},
                        (char[]) {0, 0, 7, 0},
                        (char[]) {0, 0, 7, 0}},
                .length = 4,
                .position = {0}
        }
};

t_tetromino generate_random_tetromino(void) {
	const int random_tetromino_id = rand() % NUM_TETRIMINOS;
    t_tetromino new_piece = duplicate_tetromino(tetrominoes[random_tetromino_id]);

	const int random_position_col = rand() % (COL_SIZE - new_piece.length + 1);
    new_piece.position.col = random_position_col;
    new_piece.position.row = 0;
    return new_piece;
}

t_tetromino duplicate_tetromino(const t_tetromino piece) {
    t_tetromino new_piece = {
            .array = malloc(sizeof(char *) * (unsigned int) piece.length),
            .length = piece.length,
            .position = piece.position};

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

// Returns false when piece position is out of range or the piece overwraps the board.
// Otherwise, returns true.
bool is_valid_position(const t_tetromino piece, const t_board board) {
    for (int i = 0; i < piece.length; i++) {
        for (int j = 0; j < piece.length; j++) {
			const bool is_empty_cell = !piece.array[i][j];
            if (is_empty_cell) { continue; }
            const int r = piece.position.row + i;
			const int c = piece.position.col + j;

			const bool is_position_out_of_range = c < 0 || c >= COL_SIZE || r >= ROW_SIZE;
            if (is_position_out_of_range) { return false; }

			const bool is_board_already_filled = board[r][c];
            if (is_board_already_filled) { return false; }
        }
    }
    return true;
}

void merge_tetromino_to_board(const t_tetromino piece, t_board board) {
    for (int i = 0; i < piece.length; i++) {
        for (int j = 0; j < piece.length; j++) {
			const bool is_filled_cell = piece.array[i][j];
            if (is_filled_cell) {
				int row = piece.position.row + i;
				int col = piece.position.col + j;
                board[row][col] = piece.array[i][j];
            }
        }
    }
}

// When using colors in curses window, this function should be called.
void init_curses_tetromino_colors(void) {
	if (has_colors()) {
		start_color();
		init_pair(TETROMINO_S, COLOR_BLACK, COLOR_GREEN);
		init_pair(TETROMINO_Z, COLOR_WHITE, COLOR_RED);
		init_pair(TETROMINO_T, COLOR_WHITE, COLOR_MAGENTA);
		init_pair(TETROMINO_L, COLOR_BLACK, COLOR_WHITE); // shape L is orange but ncurses doesn't have orange color
		init_pair(TETROMINO_J, COLOR_WHITE, COLOR_BLUE);
		init_pair(TETROMINO_O, COLOR_BLACK, COLOR_YELLOW);
		init_pair(TETROMINO_I, COLOR_BLACK, COLOR_CYAN);
	}
}
