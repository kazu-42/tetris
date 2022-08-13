#include "../include/tetris.h"
#include <ncurses.h>
#include <stdlib.h>

void init_srand_seed(void);

void init_curses(void);

void init_context(t_context *ctx);

void destroy_context(t_context *ctx);

void destroy_curses(void);

void run_tetris(t_context *ctx);

// Use current timestamp as randomness seed
void init_srand_seed(void) {
    const unsigned int seed = (unsigned int) time(NULL);
    srand(seed);
}

// Before calling this function, Randomness seed should be set by srand().
void init_context(t_context *ctx) {
    *ctx = (t_context){
            .score = 0,
            .game_on = true,
            .board = {{0}},
            .gravity = DEFAULT_GRAVITY};
    ctx->current = generate_random_tetromino();
    gettimeofday(&ctx->last_fell_at, NULL);
}

// init screen and configure blocking read interval
void init_curses(void) {
    initscr();
    keypad(stdscr, true);
    if (COLOR_ENABLED) {
        init_curses_tetromino_colors();
    }
    timeout(CURSES_READ_INTERVAL_MILLISEC);
}

// Run the tetris game loop until the game is over
void run_tetris(t_context *ctx) {
    // initialize curses window
    init_curses();

    // print initial screen to window
    printw_tetris_screen(ctx->board, ctx->current, ctx->score);

    // loop while game is not over
    while (ctx->game_on) {
        const int key_input = getch();
        if (key_input != ERR) {
            const t_move move = to_move(key_input);
            const bool moved = try_move_tetromino(move, &ctx->current, ctx->board);
            if (move == MOVE_DOWN && moved) {
                gettimeofday(&ctx->last_fell_at, NULL);
            }
            printw_tetris_screen(ctx->board, ctx->current, ctx->score);
        }
        if (is_time_to_fall(ctx->last_fell_at, ctx->gravity)) {
            apply_gravity(ctx);
            printw_tetris_screen(ctx->board, ctx->current, ctx->score);
            gettimeofday(&ctx->last_fell_at, NULL);
        }
    }

    // destroy curses window
    destroy_curses();

    // print the final score and board
    print_tetris_result_screen(ctx->score, ctx->board);
}

void destroy_context(t_context *ctx) {
    destroy_tetromino(ctx->current);
}

void destroy_curses(void) {
    endwin();
}

int main(void) {
    t_context ctx;

    init_srand_seed();
    init_context(&ctx);
    run_tetris(&ctx);
    destroy_context(&ctx);
    return 0;
}
