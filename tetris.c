#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>

#include "tetromino.h"
#include "gravity.h"
#include "move.h"
#include "print.h"
#include "tetris.h"
#include "config.h"

// move.c

// tetromino.c

// gravity.c

// print.c

// main.c
void init_seed(void);
void init_curses(void);
void init_context(t_context *ctx);
void destroy_context(t_context *ctx);
void destroy_curses(void);
void run_tetris(t_context *ctx);

// Before calling this function, Randomness seed should be set by srand().
void init_context(t_context *ctx) {
    *ctx = (t_context){
            .score = 0,
            .game_on = true,
            .board = {0},
			.gravity = DEFAULT_GRAVITY
	};
	ctx->current = generate_random_tetromino();
    gettimeofday(&ctx->updated_at, NULL);
}

// Use current timestamp as randomness seed
void init_seed(void) {
	unsigned int seed;

	seed = (unsigned int)time(NULL);
    srand(seed);
}

// init screen and configure blocking read interval
void init_curses(void) {
    initscr();
    timeout(CURSES_READ_INTERVAL_MILLISEC);
}

void run_tetris(t_context *ctx) {
    int key_input;
	t_move move;

	// print initial screen to window
    printw_current_screen(ctx->board, ctx->current, ctx->score);

	// loop while game is not over
    while (ctx->game_on) {
		key_input = getch();
        if (key_input != ERR) {
			move = to_move(key_input);
            try_move(move, &ctx->current, ctx->board);
            printw_current_screen(ctx->board, ctx->current, ctx->score);
        }
        if (is_time_to_fall(ctx->updated_at, ctx->gravity)) {
			apply_gravity(ctx);
            printw_current_screen(ctx->board, ctx->current, ctx->score);
			gettimeofday(&ctx->updated_at, NULL);
        }
    }

	// print the final result to terminal
    print_result(ctx->score, ctx->board);
}

void destroy_context(t_context *ctx) {
    destroy_tetromino(ctx->current);
}

void destroy_curses(void) {
    endwin();
}

int main(void) {
    t_context ctx;

	init_seed();
	init_curses();
    init_context(&ctx);
    run_tetris(&ctx);
    destroy_context(&ctx);
	destroy_curses();
    return 0;
}
