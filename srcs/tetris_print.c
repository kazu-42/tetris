/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tetris_print.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: susami <susami@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/12 16:55:40 by susami            #+#    #+#             */
/*   Updated: 2022/08/13 01:56:47 by susami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tetris.h"
#include <ncurses.h>
#include <stdio.h>
#include <string.h>

// WIN_SIZE is about 2x of COL_SIZE because space is added between each cell
#define WIN_SIZE (2 * COL_SIZE - 1)
#define TETRIS_TITLE "42 tetris"
#define EMPTY_CELL '.'
#define FILLED_CELL '#'
#define UNKNOWN_CELL '?'

static void printw_title_in_center(void);

static void print_game_over_text(void);

static void printw_board(const t_board board);

static void print_board(const t_board board);

static void printw_score(const int score);

static void print_score(const int score);

static void printw_cell(const char cell);

static void print_cell(const char cell);

// Print the game screen to the curses window.
// Information below are displayed.
// - Title of the game
// - Piece which is currently falling
// - Board
// - Score
void printw_tetris_screen(const t_board board, const t_tetromino piece, const int score) {
    t_board buffer;
    // copy board and piece to buffer
    memmove(buffer, board, sizeof(t_board));
    merge_tetromino_to_board(piece, buffer);

    clear();// clears curses window
    printw_title_in_center();
    printw_board(buffer);
    printw_score(score);
}

// Print the game result to the terminal standard output.
// Information below are displayed.
// - Board
// - Game over text
// - Score
void print_tetris_result_screen(const int score, const t_board board) {
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

/*
 printw_xxx and print_xxx functions below look very similar, 
 but it's much easier to maintain with these similar lines
 when it comes to pretty formatting like adding colors and so on.
*/
static void printw_score(const int score) {
    printw("Score: %d\n", score);
}

static void print_score(const int score) {
    printf("Score: %d\n", score);
}

static void printw_board(const t_board board) {
    for (int r = 0; r < ROW_SIZE; r++) {
        for (int c = 0; c < COL_SIZE; c++) {
            printw_cell(board[r][c]);
        }
        printw("\n");
    }
    printw("\n");
}

static void print_board(const t_board board) {
    for (int r = 0; r < ROW_SIZE; r++) {
        for (int c = 0; c < COL_SIZE; c++) {
            print_cell(board[r][c]);
        }
        printf("\n");
    }
    printf("\n");
}

static void printw_cell(const char cell) {
    switch (cell) {
        case 0:
            printw("%c ", EMPTY_CELL);
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            if (COLOR_ENABLED) {
                attron(COLOR_PAIR(cell));
                printw("%c ", FILLED_CELL);
                attroff(COLOR_PAIR(cell));
            } else {
                printw("%c ", FILLED_CELL);
            }
            break;
        default:
            printw("%c ", UNKNOWN_CELL);
            break;
    }
}

static void print_cell(const char cell) {
    switch (cell) {
        case 0:
            printf("%c ", EMPTY_CELL);
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            printf("%c ", FILLED_CELL);
            break;
        default:
            printf("%c ", UNKNOWN_CELL);
            break;
    }
}
