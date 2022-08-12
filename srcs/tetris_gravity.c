/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tetris_gravity.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: susami <susami@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/12 16:55:49 by susami            #+#    #+#             */
/*   Updated: 2022/08/12 18:20:45 by susami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/tetris.h"

#define ONE_SECOND_IN_USEC 1000000
#define FRAME_PER_SECOND 10
#define SCORE_PER_LINE 100
#define GRAVITY_INCREASE_PER_LINE 0.01

void apply_gravity(t_context *ctx);
int is_time_to_fall(t_timeval updated_at, double gravity);
static void lock_tetromino_to_board(const t_tetromino piece, t_board board);
static bool is_line_filled(int row, const t_board board);
static void clear_line(int row, t_board board);
static int	clear_filled_lines(t_board board);
static void increase_gravity(double *gravity, int lines_cleared);

// When certain period of time is passed since last updates, piece falls by gravity.
void apply_gravity(t_context *ctx) {
	bool is_tetoromino_landed;
	int	lines_cleared;

	is_tetoromino_landed = !try_move_tetromino(MOVE_DOWN, &ctx->current, ctx->board);
    if (is_tetoromino_landed) {
        lock_tetromino_to_board(ctx->current, ctx->board);
		lines_cleared = clear_filled_lines(ctx->board);
        ctx->score += SCORE_PER_LINE * lines_cleared;
		increase_gravity(&ctx->gravity, lines_cleared);
		destroy_tetromino(ctx->current);
        ctx->current = generate_random_tetromino();
        if (!is_valid_position(ctx->current, ctx->board)) {
            ctx->game_on = false;
        }
    }
}

// The greater the gravity is, the faster the piece falls.
// Gravity is expressed in unit G, where 1G = 1 cell per frame, and 0.1G = 1 cell per 10 frames.
int is_time_to_fall(t_timeval updated_at, double gravity) {
    t_timeval now;
	time_t elapsed_usec;

    gettimeofday(&now, NULL);
    elapsed_usec = (now.tv_sec - updated_at.tv_sec) * ONE_SECOND_IN_USEC +
                               (now.tv_usec - updated_at.tv_usec);
	return ((double)elapsed_usec * gravity) > (ONE_SECOND_IN_USEC / FRAME_PER_SECOND);
}

static void lock_tetromino_to_board(const t_tetromino piece, t_board board) {
    for (int i = 0; i < piece.length; i++) {
        for (int j = 0; j < piece.length; j++) {
            if (piece.array[i][j])
                board[piece.row + i][piece.col + j] = piece.array[i][j];
        }
    }
}

static bool is_line_filled(int row, const t_board board) {
    for (int c = 0; c < COL_SIZE; c++) {
        if (!board[row][c])
            return false;
    }
    return true;
}

static void clear_line(int row, t_board board) {
    //move all rows above down one row
    //move backwards to achieve a non-destructive manner.
    for (int r = row; r >= 1; r--)
        for (int c = 0; c < COL_SIZE; c++)
            board[r][c] = board[r - 1][c];
    //clear the first row
    for (int c = 0; c < COL_SIZE; c++)
        board[0][c] = 0;
}

// Returns how many lines are cleared
static int	clear_filled_lines(t_board board) {
	int num_cleared = 0;

    for (int r = 0; r < ROW_SIZE; r++) {
        if (is_line_filled(r, board)) {
            clear_line(r, board);
			num_cleared++;
        }
    }
	return num_cleared;
}

static void increase_gravity(double *gravity, int lines_cleared) {
	*gravity = *gravity + GRAVITY_INCREASE_PER_LINE * lines_cleared;
}
