#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <ncurses.h>
#include <stdbool.h>

#define ROW 20
#define COL 15
#define FRAME_INTERVAL_USEC 400000

typedef enum e_key{
	TETROMINO_DOWN = 's',
	TETROMINO_LEFT = 'a',
	TETROMINO_RIGHT = 'd',
	TETROMINO_ROTATE = 'w',
} t_key;

typedef struct {
	char **array;
	int width, row, col;
} t_tetromino;

typedef bool t_board[ROW][COL];
typedef struct timeval t_timeval;

typedef struct {
	t_tetromino	current;
	t_board		board;
	int			score;
	bool		game_on;
	t_timeval	updated_at;
} t_context;

const t_tetromino pieces[7] = {
		{(char *[]) {(char[]) {0, 1, 1}, (char[]) {1, 1, 0}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {1, 1, 0}, (char[]) {0, 1, 1}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {0, 1, 0}, (char[]) {1, 1, 1}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {0, 0, 1}, (char[]) {1, 1, 1}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {1, 0, 0}, (char[]) {1, 1, 1}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {1, 1}, (char[]) {1, 1}},                                                           2},
		{(char *[]) {(char[]) {0, 0, 0, 0}, (char[]) {1, 1, 1, 1}, (char[]) {0, 0, 0, 0}, (char[]) {0, 0, 0, 0}}, 4}
};

t_tetromino copy_piece(const t_tetromino piece) {
	t_tetromino new_tetromino = piece;
	char **copy_piece = piece.array;
	new_tetromino.array = (char **) malloc(new_tetromino.width * sizeof(char *));
	int i, j;
	for (i = 0; i < new_tetromino.width; i++) {
		new_tetromino.array[i] = (char *) malloc(new_tetromino.width * sizeof(char));
		for (j = 0; j < new_tetromino.width; j++) {
			new_tetromino.array[i][j] = copy_piece[i][j];
		}
	}
	return new_tetromino;
}

void destroy_piece(t_tetromino piece) {
	int i;
	for (i = 0; i < piece.width; i++) {
		free(piece.array[i]);
	}
	free(piece.array);
}

bool is_valid_position(const t_tetromino piece, const t_board board) {
	char **array = piece.array;
	int i, j;
	for (i = 0; i < piece.width; i++) {
		for (j = 0; j < piece.width; j++) {
			int r = piece.row + i, c = piece.col + j;
			if (!array[i][j])
				continue;
			if (c < 0 || c >= COL || r >= ROW )
				return false;
            if (board[r][c])
				return false;
		}
	}
	return true;
}

void rotate_piece(t_tetromino *piece) { //rotate_clockwise
	t_tetromino temp = copy_piece(*piece);
	int i, j, k, width;
	width = piece->width;
	for (i = 0; i < width; i++) {
		for (j = 0, k = width - 1; j < width; j++, k--) {
			piece->array[i][j] = temp.array[k][i];
		}
	}
	destroy_piece(temp);
}

void print_tetris(const t_board board, const t_tetromino current, int score) {
	char buffer[ROW][COL] = {0};
	int i, j;
	for (i = 0; i < current.width; i++) {
		for (j = 0; j < current.width; j++) {
			if (current.array[i][j])
				buffer[current.row + i][current.col + j] = current.array[i][j];
		}
	}
	clear();
	for (i = 0; i < COL - 9; i++)
		printw(" ");
	printw("42 Tetris\n");
	for (i = 0; i < ROW; i++) {
		for (j = 0; j < COL; j++) {
			printw("%c ", (board[i][j] + buffer[i][j]) ? '#' : '.');
		}
		printw("\n");
	}
	printw("\nScore: %d\n", score);
}


int has_to_update(t_timeval updated_at, suseconds_t frame_interval_usec) {
	t_timeval now;

	gettimeofday(&now, NULL);
	return ((suseconds_t)(now.tv_sec * 1000000 + now.tv_usec) -
			((suseconds_t) updated_at.tv_sec * 1000000 + updated_at.tv_usec)) > frame_interval_usec;
}

void spawn_random_tetromino(t_tetromino *current) {
	t_tetromino new_piece = copy_piece(pieces[rand()%7]);
	new_piece.col = rand()%(COL - new_piece.width + 1);
	new_piece.row = 0;
	destroy_piece(*current);
	*current = new_piece;
}

void copy_current_to_board(const t_tetromino current, t_board board) {
	int i, j;
	for(i = 0; i < current.width ;i++){
		for(j = 0; j < current.width ; j++){
			if(current.array[i][j])
				board[current.row+i][current.col+j] = current.array[i][j];
		}
	}
}

bool is_completely_filled(int row, const t_board board) {
	for(int i = 0; i < COL; i++){
		if(!board[row][i])
			return false;
	}
	return true;
}

void clear_line(int row, t_board board) { //TODO: var i, j -> なんかいい感じの変数名に
	int i, j;

	for(j = row;j >=1;j--)
		for(i=0;i<COL;i++)
			board[j][i]=board[j-1][i];
	for(i=0;i<COL;i++)
		board[j][i]=0;
}

void clear_lines(int *score, t_board board) {
	for(int n=0;n<ROW;n++){
		if(is_completely_filled(n, board)){
			clear_line(n, board);
			*score += 100;
		}
	}
}

void move_down(t_tetromino *piece) {
    piece->row++;
}

void move_right(t_tetromino *piece){
    piece->col++;
}

void move_left(t_tetromino *piece){
    piece->col--;
}

bool	is_executable(t_key op, const t_tetromino piece, const t_board board) {
	t_tetromino temp = copy_piece(piece);
	switch (op) {
		case TETROMINO_DOWN:
            move_down(&temp);
			break;
		case TETROMINO_RIGHT:
            move_right(&temp);
			break;
		case TETROMINO_LEFT:
			move_left(&temp);
			break;
		case TETROMINO_ROTATE:
			rotate_piece(&temp);
			break;
        default:
			return false;
	}
	bool res = is_valid_position(temp, board);
	destroy_piece(temp);
	return res;
}

void	execute(t_key op, t_tetromino *piece) {
    switch (op) {
        case TETROMINO_DOWN:
            move_down(piece);
            break;
        case TETROMINO_RIGHT:
            move_right(piece);
            break;
        case TETROMINO_LEFT:
            move_left(piece);
            break;
        case TETROMINO_ROTATE:
            rotate_piece(piece);
            break;
        default:
            return;
    }
}

void update_terminal(t_key op, t_context *ctx) {
    if (is_executable(op, ctx->current, ctx->board)) {
        execute(op, &ctx->current);
    } else if (op == TETROMINO_DOWN) {
        copy_current_to_board(ctx->current, ctx->board);
        clear_lines(&ctx->score, ctx->board);
        spawn_random_tetromino(&ctx->current);
		if(!is_valid_position(ctx->current, ctx->board)) {
			ctx->game_on = false;
		}
    }
}

void print_result(int score, const t_board board) {
	int i, j;
	for (i = 0; i < ROW; i++) {
		for (j = 0; j < COL; j++) {
			printf("%c ", board[i][j] ? '#' : '.');
		}
		putchar('\n');
	}
	printf("\nGame over!\n");
	printf("\nScore: %d\n", score);
}

void	init_context(t_context *ctx) {
	*ctx = (t_context) {
		.score = 0,
		.game_on = true,
		.current = {0},
		.board = {0}
	};
	gettimeofday(&ctx->updated_at, NULL);
}

void	init_game(t_context *ctx) {
	init_context(ctx);
	srand(time(NULL));
	initscr();
	timeout(1);
	spawn_random_tetromino(&ctx->current);
	if(!is_valid_position(ctx->current, ctx->board)) {
		ctx->game_on = false;
	}
	print_tetris(ctx->board, ctx->current, ctx->score);
}

void	loop_game(t_context *ctx) {
	int	key_input;

	while (ctx->game_on) {
		if ((key_input = getch()) != ERR) {
			update_terminal(key_input, ctx);
			print_tetris(ctx->board, ctx->current, ctx->score);
		}
		if (has_to_update(ctx->updated_at, FRAME_INTERVAL_USEC)) {
			update_terminal(TETROMINO_DOWN, ctx);
			print_tetris(ctx->board, ctx->current, ctx->score);
			gettimeofday(&ctx->updated_at, NULL);
		}
	}
}

void	destroy_game(t_context *ctx) {
	destroy_piece(ctx->current);
	endwin();
	print_result(ctx->score, ctx->board);
}

int main() {
	t_context	ctx;

	init_game(&ctx);
	loop_game(&ctx);
	destroy_game(&ctx);
	return 0;
}
