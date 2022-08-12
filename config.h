#ifndef TETRIS_CONFIG_H
#define TETRIS_CONFIG_H

#include "gravity.h"
#include "tetromino.h"

#define ROW 20
#define COL 15
#define ONE_SECOND_IN_USEC 1000000
#define FRAME_PER_SECOND 10
#define NUM_TETRIMINOS 7
#define SCORE_PER_LINE 100
#define GRAVITY_INCREASE_PER_LINE 0.01
#define DEFAULT_GRAVITY 0.1
#define CURSES_READ_INTERVAL_MILLISEC 1

static const t_tetromino tetrominoes[NUM_TETRIMINOS] = {
        // shape S
        {
                .array = (char *[]){
                        (char[]){0, 1, 1},
                        (char[]){1, 1, 0},
                        (char[]){0, 0, 0}},
                .length = 3,
                .row = 0,
                .col = 0
        },
        // shape Z
        {
                .array = (char *[]){
                        (char[]){1, 1, 0},
                        (char[]){0, 1, 1},
                        (char[]){0, 0, 0}},
                .length = 3,
                .row = 0,
                .col = 0
        },
        // shape T
        {
                .array = (char *[]){
                        (char[]){0, 0, 0},
                        (char[]){1, 1, 1},
                        (char[]){0, 1, 0}},
                .length = 3,
                .row = 0,
                .col = 0
        },
        // shape L
        {
                .array = (char *[]){
                        (char[]){1, 0, 0},
                        (char[]){1, 0, 0},
                        (char[]){1, 1, 0}},
                .length = 3,
                .row = 0,
                .col = 0
        },
        // shape J
        {
                .array = (char *[]){
                        (char[]){0, 0, 1},
                        (char[]){0, 0, 1},
                        (char[]){0, 1, 1}},
                .length = 3,
                .row = 0,
                .col = 0
        },
        // shape O
        {
                .array = (char *[]){
                        (char[]){1, 1},
                        (char[]){1, 1}},
                .length = 2,
                .row = 0,
                .col = 0
        },
        // shape I
        {
                .array = (char *[]){
                        (char[]){0, 0, 1, 0},
                        (char[]){0, 0, 1, 0},
                        (char[]){0, 0, 1, 0},
                        (char[]){0, 0, 1, 0}},
                .length = 4,
                .row = 0,
                .col = 0
        }};

#endif//TETRIS_CONFIG_H
