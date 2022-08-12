#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define ROW 20
#define COL 15
#define ONE_SECOND_IN_USEC 1000000
#define FRAME_PER_SECOND 10.0
#define NUM_TETRIMINOS 7
#define SCORE_PER_LINE 100
#define DEFAULT_GRAVITY 0.1
#define GRAVITY_INCREASE_PER_LINE 0.01
#define CURSES_READ_INTERVAL_MILLISEC 1

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
    int score;
    bool game_on;
    t_board board;
	double gravity;
    t_tetromino current;
    t_timeval updated_at;
} t_context;

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

// move.c
t_move to_move(int ch);
void apply_move(t_move move, t_tetromino *piece);
bool try_move(t_move move, t_tetromino *piece, const t_board board);
static void move_down(t_tetromino *piece);
static void move_right(t_tetromino *piece);
static void move_left(t_tetromino *piece);
static void rotate_tetromino(t_tetromino *piece);
static bool has_room_to_move(t_move move, const t_tetromino piece, const t_board board);

// tetromino.c
t_tetromino generate_random_tetromino(void);
t_tetromino duplicate_tetromino(const t_tetromino piece);
void destroy_tetromino(t_tetromino piece);
bool is_valid_position(const t_tetromino piece, const t_board board);

// gravity.c
void apply_gravity(t_context *ctx);
int is_time_to_fall(t_timeval updated_at, double gravity);
static void lock_tetromino_to_board(const t_tetromino piece, t_board board);
static bool is_line_filled(int row, const t_board board);
static void clear_line(int row, t_board board);
static int	clear_filled_lines(t_board board);
static double next_gravity(double gravity, int lines_cleared);

// print.c
static void printw_current_screen(const t_board board, const t_tetromino current, int score);
static void print_result(int score, const t_board board);

// main.c
void init_seed(void);
void init_curses(void);
void init_context(t_context *ctx);
void destroy_context(t_context *ctx);
void destroy_curses(void);
void run_tetris(t_context *ctx);

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

t_tetromino duplicate_tetromino(const t_tetromino piece) {
    t_tetromino new_piece = {
            .array = malloc(sizeof(char *) * (unsigned int)piece.length),
            .length = piece.length,
            .row = piece.row,
            .col = piece.col};

    for (int r = 0; r < new_piece.length; r++) {
        new_piece.array[r] = (char *) malloc(sizeof(char) * (unsigned int)new_piece.length);
        for (int c = 0; c < new_piece.length; c++) {
            new_piece.array[r][c] = piece.array[r][c];
        }
    }
    return new_piece;
}

void destroy_tetromino(t_tetromino piece) {
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

// rotate tetromino clockwise
void rotate_tetromino(t_tetromino *piece) {
    t_tetromino temp = duplicate_tetromino(*piece);
    for (int i = 0; i < piece->length; i++) {
        for (int j = 0; j < piece->length; j++) {
            int k = piece->length - 1 - j;
            piece->array[i][j] = temp.array[k][i];
        }
    }
    destroy_tetromino(temp);
}

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

// When certain period of time is passed since last updates, piece falls by gravity.
// The greater the gravity is, the faster the piece falls.
// Gravity is expressed in unit G, where 1G = 1 cell per frame, and 0.1G = 1 cell per 10 frames.
int is_time_to_fall(t_timeval updated_at, double gravity) {
    t_timeval now;
	time_t elapsed_usec;

    gettimeofday(&now, NULL);
    elapsed_usec = (now.tv_sec - updated_at.tv_sec) * ONE_SECOND_IN_USEC +
                               (now.tv_usec - updated_at.tv_usec);
	return ((double)elapsed_usec * gravity) > (ONE_SECOND_IN_USEC / FRAME_PER_SECOND);
}

t_tetromino generate_random_tetromino(void) {
    t_tetromino new_piece = duplicate_tetromino(tetrominoes[rand() % NUM_TETRIMINOS]);
    new_piece.col = rand() % (COL - new_piece.length + 1);
    new_piece.row = 0;
	return new_piece;
}

void lock_tetromino_to_board(const t_tetromino piece, t_board board) {
    for (int i = 0; i < piece.length; i++) {
        for (int j = 0; j < piece.length; j++) {
            if (piece.array[i][j])
                board[piece.row + i][piece.col + j] = piece.array[i][j];
        }
    }
}

bool is_line_filled(int row, const t_board board) {
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
int	clear_filled_lines(t_board board) {
	int num_cleared = 0;

    for (int r = 0; r < ROW; r++) {
        if (is_line_filled(r, board)) {
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

void apply_move(t_move move, t_tetromino *piece) {
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
            rotate_tetromino(piece);
            break;
		case MOVE_UNKNOWN:
            return;
    }
}

static bool has_room_to_move(t_move move, const t_tetromino piece, const t_board board) {
	bool	has_room_to_move;
    t_tetromino temp;

	temp = duplicate_tetromino(piece);
	apply_move(move, &temp);
    has_room_to_move = is_valid_position(temp, board);
    destroy_tetromino(temp);
    return has_room_to_move;
}

// Returns true if the board has room and move is applied
bool try_move(t_move move, t_tetromino *piece, const t_board board) {
    if (has_room_to_move(move, *piece, board)) {
        apply_move(move, piece);
		return true;
    } else {
		return false;
	}
}

static double next_gravity(double gravity, int lines_cleared) {
	return gravity + GRAVITY_INCREASE_PER_LINE * lines_cleared;
}

void apply_gravity(t_context *ctx) {
	bool is_tetoromino_landed;
	int	lines_cleared;

	is_tetoromino_landed = !try_move(MOVE_DOWN, &ctx->current, ctx->board);
    if (is_tetoromino_landed) {
        lock_tetromino_to_board(ctx->current, ctx->board);
		lines_cleared = clear_filled_lines(ctx->board);
        ctx->score += SCORE_PER_LINE * lines_cleared;
		ctx->gravity = next_gravity(ctx->gravity, lines_cleared);
		destroy_tetromino(ctx->current);
        ctx->current = generate_random_tetromino();
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
