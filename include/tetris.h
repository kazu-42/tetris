#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>
#include <sys/time.h>
#include "config.h"

# define TETROMINO_NULL 0
# define TETROMINO_S 1
# define TETROMINO_Z 2
# define TETROMINO_T 3
# define TETROMINO_L 4
# define TETROMINO_J 5
# define TETROMINO_O 6
# define TETROMINO_I 7

typedef enum {
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_ROTATE_CLOCKWISE,
    MOVE_ROTATE_COUNTER_CLOCKWISE,
    MOVE_UNKNOWN,
} t_move;

typedef struct {
    int row;
    int col;
} t_position;

typedef struct {
    char **array;
    int length;// length is max(width, height) of the tetromino
    t_position position;
} t_tetromino;

typedef char t_board[ROW_SIZE][COL_SIZE];
typedef struct timeval t_timeval;

typedef struct {
    int score;
    bool game_on;
    t_board board;
    double gravity;
    t_tetromino current;
    t_timeval last_fell_at;
} t_context;

// tetris_tetromino.c
t_tetromino generate_random_tetromino(void);
t_tetromino duplicate_tetromino(const t_tetromino piece);
void destroy_tetromino(t_tetromino piece);
bool is_valid_position(const t_tetromino piece, const t_board board);
void merge_tetromino_to_board(const t_tetromino piece, t_board board);
void init_curses_tetromino_colors(void);

// tetris_move.c
t_move to_move(int ch);
void move_tetromino(t_move move, t_tetromino *piece);
bool try_move_tetromino(t_move move, t_tetromino *piece, const t_board board);
bool has_room_to_move(const t_move move, const t_tetromino piece, const t_board board);


// tetris_gravity.c
void apply_gravity(t_context *ctx);
bool is_time_to_fall(const t_timeval last_fell_at, const double gravity);

// tetris_print.c
void printw_tetris_screen(const t_board board, const t_tetromino piece, const int score);
void print_tetris_result_screen(const int score, const t_board board);

#endif //TETRIS_H
