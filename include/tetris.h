/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tetris.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: susami <susami@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/12 16:22:45 by susami            #+#    #+#             */
/*   Updated: 2022/08/12 20:55:33 by susami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TETRIS_H
# define TETRIS_H

#include <stdbool.h>
#include <sys/time.h>
#define ROW_SIZE 20
#define COL_SIZE 15

typedef enum {
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_ROTATE_CLOCKWISE,
    MOVE_ROTATE_COUNTER_CLOCKWISE,
    MOVE_UNKNOWN,
} t_move;

typedef struct {
    char **array;
    int length, row, col;
} t_tetromino;

typedef char t_board[ROW_SIZE][COL_SIZE];
typedef struct timeval t_timeval;

typedef struct {
    int score;
    bool game_on;
    t_board board;
	double gravity;
    t_tetromino current;
    t_timeval updated_at;
} t_context;

// tetris_tetromino.c
t_tetromino generate_random_tetromino(void);
t_tetromino duplicate_tetromino(const t_tetromino piece);
void destroy_tetromino(t_tetromino piece);
bool is_valid_position(const t_tetromino piece, const t_board board);

// tetris_move.c
t_move to_move(int ch);
void move_tetromino(t_move move, t_tetromino *piece);
bool try_move_tetromino(t_move move, t_tetromino *piece, const t_board board);

// tetris_gravity.c
void apply_gravity(t_context *ctx);
int is_time_to_fall(t_timeval updated_at, double gravity);

// tetris_print.c
void printw_current_screen(const t_board board, const t_tetromino current, int score);
void print_result(int score, const t_board board);
#endif
