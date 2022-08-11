#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define ROW 20
#define COL 15
#define FRAME_INTERVAL_USEC 400000
#define NUM_TETRIMINOS 7
#define SCORE_PER_LINE 100

typedef enum {
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_ROTATE,
    MOVE_UNKNOWN,
} t_move;



typedef struct {
    char **array;
    int length, row, col;
} t_tetromino;

typedef char t_board[ROW][COL];
typedef struct timeval t_timeval;

typedef struct {
    t_tetromino current;
    t_board board;
    int score;
    bool game_on;
    t_timeval updated_at;
} t_context;

const t_tetromino tetriminos[NUM_TETRIMINOS] = {
        {(char *[]){
                 (char[]){0, 1, 1},
                 (char[]){1, 1, 0},
                 (char[]){0, 0, 0}},
         3},
        {(char *[]){
                 (char[]){1, 1, 0},
                 (char[]){0, 1, 1},
                 (char[]){0, 0, 0}},
         3},
        {(char *[]){
                 (char[]){0, 1, 0},
                 (char[]){1, 1, 1},
                 (char[]){0, 0, 0}},
         3},
        {(char *[]){
                 (char[]){0, 0, 1},
                 (char[]){1, 1, 1},
                 (char[]){0, 0, 0}},
         3},
        {(char *[]){
                 (char[]){1, 0, 0},
                 (char[]){1, 1, 1},
                 (char[]){0, 0, 0}},
         3},
        {(char *[]){
                 (char[]){1, 1},
                 (char[]){1, 1}},
         2},
        {(char *[]){
                 (char[]){0, 0, 0, 0},
                 (char[]){1, 1, 1, 1},
                 (char[]){0, 0, 0, 0},
                 (char[]){0, 0, 0, 0}},
         4}};

t_move to_move(int ch) {
    printw("%d\n", ch);
    switch (ch) {
        case 's':
            return MOVE_DOWN;
        case 'a':
            return MOVE_LEFT;
        case 'd':
            return MOVE_RIGHT;
        case 'w':
            return MOVE_ROTATE;
        default:
            return MOVE_UNKNOWN;
    }
}

t_tetromino duplicate_piece(const t_tetromino piece) {
    t_tetromino new_piece = {
            .array = malloc(sizeof(char *) * piece.length),
            .length = piece.length,
            .row = piece.row,
            .col = piece.col};

    for (int r = 0; r < new_piece.length; r++) {
        new_piece.array[r] = (char *) malloc(sizeof(char) * new_piece.length);
        for (int c = 0; c < new_piece.length; c++) {
            new_piece.array[r][c] = piece.array[r][c];
        }
    }
    return new_piece;
}

void destroy_piece(t_tetromino piece) {
    for (int r = 0; r < piece.length; r++) {
        free(piece.array[r]);
    }
    free(piece.array);
}

bool is_valid_position(const t_tetromino piece, const t_board board) {
    for (int i = 0; i < piece.length; i++) {
        for (int j = 0; j < piece.length; j++) {
            if (!piece.array[i][j])
                continue;
            int r = piece.row + i;
			int c = piece.col + j;
            if (c < 0 || c >= COL || r >= ROW)
                return false;
            if (board[r][c])
                return false;
        }
    }
    return true;
}

void rotate_piece(t_tetromino *piece) {//rotate_clockwise
    t_tetromino temp = duplicate_piece(*piece);
    for (int i = 0; i < piece->length; i++) {
        for (int j = 0; j < piece->length; j++) {
            int k = piece->length - 1 - j;
            piece->array[i][j] = temp.array[k][i];
        }
    }
    destroy_piece(temp);
}

void print_tetris(const t_board board, const t_tetromino current, int score) {
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

int has_to_update(t_timeval updated_at, suseconds_t frame_interval_usec) {
    t_timeval now;

    gettimeofday(&now, NULL);
    suseconds_t elapsed_usec = (now.tv_sec - updated_at.tv_sec) * 1000000 +
                               (now.tv_usec - updated_at.tv_usec);
    return elapsed_usec > frame_interval_usec;
}

void spawn_random_tetromino(t_tetromino *current) {
    t_tetromino new_piece = duplicate_piece(tetriminos[rand() % NUM_TETRIMINOS]);
    new_piece.col = rand() % (COL - new_piece.length + 1);
    new_piece.row = 0;
    destroy_piece(*current);
    *current = new_piece;
}

void copy_current_to_board(const t_tetromino current, t_board board) {
    for (int i = 0; i < current.length; i++) {
        for (int j = 0; j < current.length; j++) {
            if (current.array[i][j])
                board[current.row + i][current.col + j] = current.array[i][j];
        }
    }
}

bool is_completely_filled(int row, const t_board board) {
    for (int c = 0; c < COL; c++) {
        if (!board[row][c])
            return false;
    }
    return true;
}

void clear_line(int row, t_board board) {
    //move all rows above down one row
    //move backwards to achieve a non-destructive manner.
    for (int r = row; r >= 1; r--)
        for (int c = 0; c < COL; c++)
            board[r][c] = board[r - 1][c];
    //clear the first row
    for (int c = 0; c < COL; c++)
        board[0][c] = 0;
}

// Returns how many lines are cleared
int	clear_lines(t_board board) {
	int num_cleared = 0;

    for (int r = 0; r < ROW; r++) {
        if (is_completely_filled(r, board)) {
            clear_line(r, board);
			num_cleared++;
        }
    }
	return num_cleared;
}

void move_down(t_tetromino *piece) {
    piece->row++;
}

void move_right(t_tetromino *piece) {
    piece->col++;
}

void move_left(t_tetromino *piece) {
    piece->col--;
}

static void move_piece(t_move move, t_tetromino *piece) {
    switch (move) {
        case MOVE_DOWN:
            move_down(piece);
            break;
        case MOVE_RIGHT:
            move_right(piece);
            break;
        case MOVE_LEFT:
            move_left(piece);
            break;
        case MOVE_ROTATE:
            rotate_piece(piece);
            break;
        default:
            return;
    }
}

static bool has_room_to_move(t_move move, const t_tetromino piece, const t_board board) {
    t_tetromino temp = duplicate_piece(piece);
	move_piece(move, &temp);
    bool is_executable = is_valid_position(temp, board);
    destroy_piece(temp);
    return is_executable;
}

void execute(t_move move, t_context *ctx) {
    if (has_room_to_move(move, ctx->current, ctx->board)) {
        move_piece(move, &ctx->current);
    } else if (move == MOVE_DOWN) {
        copy_current_to_board(ctx->current, ctx->board);
        ctx->score += SCORE_PER_LINE * clear_lines(ctx->board);
        spawn_random_tetromino(&ctx->current);
        if (!is_valid_position(ctx->current, ctx->board)) {
            ctx->game_on = false;
        }
    }
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

void init_context(t_context *ctx) {
    *ctx = (t_context){
            .score = 0,
            .game_on = true,
            .current = {0},
            .board = {0}};
    gettimeofday(&ctx->updated_at, NULL);
}

void init_game(t_context *ctx) {
    init_context(ctx);
    srand(time(NULL));
    initscr();
    timeout(1);
    spawn_random_tetromino(&ctx->current);
    if (!is_valid_position(ctx->current, ctx->board)) {
        ctx->game_on = false;
    }
    print_tetris(ctx->board, ctx->current, ctx->score);
}

void loop_game(t_context *ctx) {
    int key_input;

    while (ctx->game_on) {
		key_input = getch();
        if (key_input != ERR) {
            execute(to_move(key_input), ctx);
            print_tetris(ctx->board, ctx->current, ctx->score);
        }
        if (has_to_update(ctx->updated_at, FRAME_INTERVAL_USEC)) {
            execute(MOVE_DOWN, ctx);
            print_tetris(ctx->board, ctx->current, ctx->score);
            gettimeofday(&ctx->updated_at, NULL);
        }
    }
}

void destroy_game(t_context *ctx) {
    destroy_piece(ctx->current);
    endwin();
    print_result(ctx->score, ctx->board);
}

int main() {
    t_context ctx;

    init_game(&ctx);
    loop_game(&ctx);
    destroy_game(&ctx);
    return 0;
}
