/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tetris_print.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: susami <susami@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/12 16:55:40 by susami            #+#    #+#             */
/*   Updated: 2022/08/12 17:10:39 by susami           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/tetris.h"
#include <ncurses.h>
#include <stdio.h>

// print.c
void printw_current_screen(const t_board board, const t_tetromino current, int score);
void print_result(int score, const t_board board);

void printw_current_screen(const t_board board, const t_tetromino current, int score) {
    char buffer[ROW_SIZE][COL_SIZE] = {0};
    for (int i = 0; i < current.length; i++) {
        for (int j = 0; j < current.length; j++) {
            if (current.array[i][j])
                buffer[current.row + i][current.col + j] = current.array[i][j];
        }
    }
    clear();
    for (int c = 0; c < COL_SIZE - 9; c++)
        printw(" ");
    printw("42 Tetris\n");
    for (int r = 0; r < ROW_SIZE; r++) {
        for (int c = 0; c < COL_SIZE; c++) {
            printw("%c ", (board[r][c] + buffer[r][c]) ? '#' : '.');
        }
        printw("\n");
    }
    printw("\nScore: %d\n", score);
}

void print_result(int score, const t_board board) {
    for (int r = 0; r < ROW_SIZE; r++) {
        for (int c = 0; c < COL_SIZE; c++) {
            printf("%c ", board[r][c] ? '#' : '.');
        }
        putchar('\n');
    }
    printf("\nGame over!\n");
    printf("\nScore: %d\n", score);
}
