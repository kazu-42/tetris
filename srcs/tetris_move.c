/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tetris_move.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: susami <susami@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/12 16:55:59 by susami            #+#    #+#             */
/*   Updated: 2022/08/12 18:23:12 by susami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/tetris.h"

t_move to_move(int ch);
void move_tetromino(t_move move, t_tetromino *piece);
bool try_move_tetromino(t_move move, t_tetromino *piece, const t_board board);
static bool has_room_to_move(t_move move, const t_tetromino piece, const t_board board);
static void move_tetromino_down(t_tetromino *piece);
static void move_tetromino_right(t_tetromino *piece);
static void move_tetromino_left(t_tetromino *piece);
static void rotate_tetromino_clockwise(t_tetromino *piece);

t_move to_move(int ch) {
    switch (ch) {
        case 's':
            return MOVE_DOWN;
        case 'a':
            return MOVE_LEFT;
        case 'd':
            return MOVE_RIGHT;
        case 'w':
            return MOVE_ROTATE;
        default:
            return MOVE_UNKNOWN;
    }
}

// Returns true if the board has room and move is applied
bool try_move_tetromino(t_move move, t_tetromino *piece, const t_board board) {
    if (has_room_to_move(move, *piece, board)) {
        move_tetromino(move, piece);
		return true;
    } else {
		return false;
	}
}

void move_tetromino(t_move move, t_tetromino *piece) {
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
        case MOVE_ROTATE:
            rotate_tetromino_clockwise(piece);
            break;
		case MOVE_UNKNOWN:
            return;
    }
}

static bool has_room_to_move(t_move move, const t_tetromino piece, const t_board board) {
	bool	has_room_to_move;
    t_tetromino temp;

	temp = duplicate_tetromino(piece);
	move_tetromino(move, &temp);
    has_room_to_move = is_valid_position(temp, board);
    destroy_tetromino(temp);
    return has_room_to_move;
}

static void move_tetromino_down(t_tetromino *piece) {
    piece->row++;
}

static void move_tetromino_right(t_tetromino *piece) {
    piece->col++;
}

static void move_tetromino_left(t_tetromino *piece) {
    piece->col--;
}

static void rotate_tetromino_clockwise(t_tetromino *piece) {
    t_tetromino temp = duplicate_tetromino(*piece);
    for (int i = 0; i < piece->length; i++) {
        for (int j = 0; j < piece->length; j++) {
            int k = piece->length - 1 - j;
            piece->array[i][j] = temp.array[k][i];
        }
    }
    destroy_tetromino(temp);
}
