//
// Created by kazuhiro takami on 2022/08/12.
//

#include "print.h"
#include <ncurses.h>


void printw_current_screen(const t_board board, const t_tetromino current, int score) {
    char buffer[ROW][COL] = {0};
    for (int i = 0; i < current.length; i++) {
        for (int j = 0; j < current.length; j++) {
            if (current.array[i][j])
                buffer[current.row + i][current.col + j] = current.array[i][j];
        }
    }
    clear();
    for (int c = 0; c < COL - 9; c++)
        printw(" ");
    printw("42 Tetris\n");
    for (int r = 0; r < ROW; r++) {
        for (int c = 0; c < COL; c++) {
            printw("%c ", (board[r][c] + buffer[r][c]) ? '#' : '.');
        }
        printw("\n");
    }
    printw("\nScore: %d\n", score);
}

void print_result(int score, const t_board board) {
    for (int r = 0; r < ROW; r++) {
        for (int c = 0; c < COL; c++) {
            printf("%c ", board[r][c] ? '#' : '.');
        }
        putchar('\n');
    }
    printf("\nGame over!\n");
    printf("\nScore: %d\n", score);
}
