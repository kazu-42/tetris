#include "../include/tetris.h"

#define ONE_SECOND_IN_USEC 1000000

static bool is_time_to_lock(const t_timeval last_fell_at);

static bool is_line_filled(const int row, const t_board board);

static void clear_line(const int row, t_board board);

static int clear_filled_lines(t_board board);

static void increase_gravity(double *gravity, const int lines_cleared);

// When certain period of time is passed since last updates, piece falls by gravity.
void apply_gravity(t_context *ctx) {
    const bool is_moved = try_move_tetromino(MOVE_DOWN, &ctx->current, ctx->board);
    if (is_moved) {
        gettimeofday(&ctx->last_fell_at, NULL);
    }
    const bool is_tetromino_landed = !has_room_to_move(MOVE_DOWN, ctx->current, ctx->board);
    if (is_tetromino_landed && is_time_to_lock(ctx->last_fell_at)) {
        merge_tetromino_to_board(ctx->current, ctx->board);
        const int lines_cleared = clear_filled_lines(ctx->board);
        ctx->score += SCORE_PER_LINE * lines_cleared;
        increase_gravity(&ctx->gravity, lines_cleared);
        destroy_tetromino(ctx->current);
        ctx->current = generate_random_tetromino();
        gettimeofday(&ctx->last_fell_at, NULL);
        if (!is_valid_position(ctx->current, ctx->board)) {
            ctx->game_on = false;
        }
    }
}

// The greater the gravity is, the faster the piece falls.
// Gravity is expressed in unit G [cell / frame],
// where 1G = 1 cell per frame, and 0.1G = 1 cell per 10 frames.
bool is_time_to_fall(const t_timeval last_fell_at, const double gravity) {
    t_timeval now;

    gettimeofday(&now, NULL);
    const time_t elapsed_usec = (now.tv_sec - last_fell_at.tv_sec) * ONE_SECOND_IN_USEC +
                                (now.tv_usec - last_fell_at.tv_usec);
    // elapsed_usec 		: [usec]
    // ONE_SECOND_IN_USEC 	: [usec / sec]
    // FRAME_PER_SECOND		: [frame / sec]
    // gravity				: [cell / frame]

    // droppage				: [cell] = [usec] / [usec / sec] * [frame / sec] * [cell / frame]
    const double droppage = (double) elapsed_usec / ONE_SECOND_IN_USEC * FRAME_PER_SECOND * gravity;
    return droppage >= 1;
}

static bool is_time_to_lock(const t_timeval last_fell_at) {
    t_timeval now;

    gettimeofday(&now, NULL);
    const time_t elapsed_usec = (now.tv_sec - last_fell_at.tv_sec) * ONE_SECOND_IN_USEC +
                                (now.tv_usec - last_fell_at.tv_usec);
    return elapsed_usec >= SOFT_DROP_HOLD_THRESHOLD_USEC;
}

static bool is_line_filled(const int row, const t_board board) {
    for (int c = 0; c < COL_SIZE; c++) {
        const bool is_cell_empty = !board[row][c];
        if (is_cell_empty) { return false; }
    }
    return true;
}

static void clear_line(const int row, t_board board) {
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
static int clear_filled_lines(t_board board) {
    int num_cleared = 0;

    for (int r = 0; r < ROW_SIZE; r++) {
        if (is_line_filled(r, board)) {
            clear_line(r, board);
            num_cleared++;
        }
    }
    return num_cleared;
}

// Increase gravity according to how many lines cleared.
// This function should be used to adjust the game level.
static void increase_gravity(double *gravity, const int lines_cleared) {
    const double increase = GRAVITY_INCREASE_PER_LINE * lines_cleared;

    if (*gravity + increase > MAX_GRAVITY) {
        *gravity = MAX_GRAVITY;
    } else {
        *gravity = *gravity + increase;
    }
}
