/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tetris_move.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: susami <susami@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/12 16:55:59 by susami            #+#    #+#             */
/*   Updated: 2022/08/13 00:37:15 by susami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tetris.h"

t_move to_move(int ch);
void move_tetromino(const t_move move, t_tetromino *piece);
bool try_move_tetromino(const t_move move, t_tetromino *piece, const t_board board);
static bool has_room_to_move(const t_move move, const t_tetromino piece, const t_board board);
static void move_tetromino_down(t_tetromino *piece);
static void move_tetromino_right(t_tetromino *piece);
static void move_tetromino_left(t_tetromino *piece);
static void rotate_tetromino_clockwise(t_tetromino *piece);
static void rotate_tetromino_counter_clockwise(t_tetromino *piece);

// functions to manipulate square matrix
static void transpose_matrix(char **matrix, const int size);
static void reverse_individual_rows_matrix(char **matrix, const int size);
static void reverse_individual_cols_matrix(char **matrix, const int size);
static void swap(char *a, char *b);

t_move to_move(const int ch) {
    switch (ch) {
        case 's':
            return MOVE_DOWN;
        case 'a':
            return MOVE_LEFT;
        case 'd':
            return MOVE_RIGHT;
        case 'w':
            return MOVE_ROTATE_CLOCKWISE;
		case 'q':
			return MOVE_ROTATE_COUNTER_CLOCKWISE;
        default:
            return MOVE_UNKNOWN;
    }
}

// Returns true if the board has room and move is applied
bool try_move_tetromino(const t_move move, t_tetromino *piece, const t_board board) {
    if (has_room_to_move(move, *piece, board)) {
        move_tetromino(move, piece);
		return true;
    } else {
		return false;
	}
}

void move_tetromino(const t_move move, t_tetromino *piece) {
    switch (move) {
        case MOVE_DOWN:
            move_tetromino_down(piece);
            break;
        case MOVE_RIGHT:
            move_tetromino_right(piece);
            break;
        case MOVE_LEFT:
            move_tetromino_left(piece);
            break;
        case MOVE_ROTATE_CLOCKWISE:
            rotate_tetromino_clockwise(piece);
            break;
        case MOVE_ROTATE_COUNTER_CLOCKWISE:
            rotate_tetromino_counter_clockwise(piece);
            break;
		case MOVE_UNKNOWN:
            return;
    }
}

static bool has_room_to_move(const t_move move, const t_tetromino piece, const t_board board) {
    t_tetromino temp = duplicate_tetromino(piece);
	move_tetromino(move, &temp);
    const bool has_room_to_move = is_valid_position(temp, board);
    destroy_tetromino(temp);
    return has_room_to_move;
}

static void move_tetromino_down(t_tetromino *piece) {
    piece->position.row++;
}

static void move_tetromino_right(t_tetromino *piece) {
    piece->position.col++;
}

static void move_tetromino_left(t_tetromino *piece) {
    piece->position.col--;
}

/*
Approach:
	We first transpose the given matrix, and then reverse the content of
	individual rows to get the resultant 90 degree clockwise rotated matrix.

1  2  3    [transpose]    1  4  7   [reverse rows]  7  4  1
4  5  6  -------------->  2  5  8  -------------->  8  5  2
7  8  9                   3  6  9                   9  6  3
*/
static void rotate_tetromino_clockwise(t_tetromino *piece) {
	transpose_matrix(piece->array, piece->length);
	reverse_individual_rows_matrix(piece->array, piece->length);
}

/*
Approach:
	We first transpose the given matrix, and then reverse the content of
	individual cols to get the resultant 90 degree counter-clockwise rotated matrix.

1  2  3    [transpose]    1  4  7   [reverse cols]  3  6  9
4  5  6  -------------->  2  5  8  -------------->  2  5  8
7  8  9                   3  6  9                   1  4  7
*/
static void rotate_tetromino_counter_clockwise(t_tetromino *piece) {
	transpose_matrix(piece->array, piece->length);
	reverse_individual_cols_matrix(piece->array, piece->length);
}

static void swap(char *a, char *b) {
	char tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

/*
1  2  3    [transpose]    1  4  7   [reverse rows]  7  4  1
4  5  6  -------------->  2  5  8  -------------->  8  5  2
7  8  9                   3  6  9                   9  6  3
*/
static void transpose_matrix(char **matrix, const int size) {
    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) {
            swap(&matrix[i][j], &matrix[j][i]);
        }
    }
}

/*
1  2  3   [reverse rows]  3  2  1
4  5  6  -------------->  6  5  4
7  8  9                   9  8  7
*/
static void reverse_individual_rows_matrix(char **matrix, const int size) {
    for (int row = 0; row < size; row++) {
		int low = 0;
		int high = size - 1;
        while (low < high) {
            swap(&matrix[row][low], &matrix[row][high]);
            low++;
            high--;
        }
    }
}

/*
1  2  3   [reverse cols]  7  8  9
4  5  6  -------------->  4  5  6
7  8  9                   1  2  3
*/
static void reverse_individual_cols_matrix(char **matrix, const int size) {
    for (int col = 0; col < size; col++) {
		int low = 0;
		int high = size - 1;
        while (low < high) {
            swap(&matrix[low][col], &matrix[high][col]);
            low++;
            high--;
        }
    }
}
