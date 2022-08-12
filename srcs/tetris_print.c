/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tetris_print.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: susami <susami@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/12 16:55:40 by susami            #+#    #+#             */
/*   Updated: 2022/08/12 23:07:41 by susami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include "tetris.h"

// WIN_SIZE is about 2x of COL_SIZE because space is added between each cell
#define WIN_SIZE (2 * COL_SIZE - 1)
#define TETRIS_TITLE "42 tetris"

// print.c
void printw_current_screen(const t_board board, const t_tetromino current, int score);
void print_result(int score, const t_board board);
static void printw_title_in_center(void);
static void print_game_over_text(void);
static void printw_board(const char buffer[ROW_SIZE][COL_SIZE]);
static void print_board(const char buffer[ROW_SIZE][COL_SIZE]);
static void printw_score(int score);
static void print_score(int score);

void printw_current_screen(const t_board board, const t_tetromino current, int score) {
    t_board buffer;
	// copy board and piece to buffer
	memmove(buffer, board, sizeof(t_board));
	merge_tetromino_to_board(current, buffer);

    clear(); // clears curses window
	printw_title_in_center();
	printw_board(buffer);
	printw_score(score);
}

void print_result(int score, const t_board board) {
	print_board(board);
	print_game_over_text();
    print_score(score);
}

static void printw_title_in_center(void) {
	static const int title_len = strlen(TETRIS_TITLE);
    for (int c = 0; c < (WIN_SIZE - title_len) / 2; c++)
        printw(" ");
    printw(TETRIS_TITLE);
    printw("\n");
}

static void print_game_over_text(void) {
    printf("Game over!\n");
}

static void printw_score(int score) {
    printw("Score: %d\n", score);
}

static void print_score(int score) {
    printf("Score: %d\n", score);
}

static void printw_board(const char buffer[ROW_SIZE][COL_SIZE]) {
    for (int r = 0; r < ROW_SIZE; r++) {
        for (int c = 0; c < COL_SIZE; c++) {
            printw("%c ", buffer[r][c] ? '#' : '.');
        }
        printw("\n");
    }
	printw("\n");
}

static void print_board(const char buffer[ROW_SIZE][COL_SIZE]) {
    for (int r = 0; r < ROW_SIZE; r++) {
        for (int c = 0; c < COL_SIZE; c++) {
            printf("%c ", buffer[r][c] ? '#' : '.');
        }
        printf("\n");
    }
	printf("\n");
}
