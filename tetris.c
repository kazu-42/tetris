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


char g_table[ROW][COL] = {0};
int g_score = 0;
bool g_game_on = true;


typedef struct {
	char **array;
	int width, row, col;
} t_tetromino;
t_tetromino current;

const t_tetromino shapes[7] = {
		{(char *[]) {(char[]) {0, 1, 1}, (char[]) {1, 1, 0}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {1, 1, 0}, (char[]) {0, 1, 1}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {0, 1, 0}, (char[]) {1, 1, 1}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {0, 0, 1}, (char[]) {1, 1, 1}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {1, 0, 0}, (char[]) {1, 1, 1}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {1, 1}, (char[]) {1, 1}},                                                           2},
		{(char *[]) {(char[]) {0, 0, 0, 0}, (char[]) {1, 1, 1, 1}, (char[]) {0, 0, 0, 0}, (char[]) {0, 0, 0, 0}}, 4}
};

t_tetromino copy_shape(t_tetromino shape) {
	t_tetromino new_tetromino = shape;
	char **copyshape = shape.array;
	new_tetromino.array = (char **) malloc(new_tetromino.width * sizeof(char *));
	int i, j;
	for (i = 0; i < new_tetromino.width; i++) {
		new_tetromino.array[i] = (char *) malloc(new_tetromino.width * sizeof(char));
		for (j = 0; j < new_tetromino.width; j++) {
			new_tetromino.array[i][j] = copyshape[i][j];
		}
	}
	return new_tetromino;
}

void destroy_shape(t_tetromino shape) {
	int i;
	for (i = 0; i < shape.width; i++) {
		free(shape.array[i]);
	}
	free(shape.array);
}

bool is_valid_position(t_tetromino shape) {
	char **array = shape.array;
	int i, j;
	for (i = 0; i < shape.width; i++) {
		for (j = 0; j < shape.width; j++) {
			if ((shape.col + j < 0 || shape.col + j >= COL || shape.row + i >= ROW)) {
				if (array[i][j])
					return false;

			} else if (g_table[shape.row + i][shape.col + j] && array[i][j])
				return false;
		}
	}
	return true;
}

void rotate_shape(t_tetromino shape) { //rotate_clockwise
	t_tetromino temp = copy_shape(shape);
	int i, j, k, width;
	width = shape.width;
	for (i = 0; i < width; i++) {
		for (j = 0, k = width - 1; j < width; j++, k--) {
			shape.array[i][j] = temp.array[k][i];
		}
	}
	destroy_shape(temp);
}

void print_tetris() {
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
			printw("%c ", (g_table[i][j] + buffer[i][j]) ? '#' : '.');
		}
		printw("\n");
	}
	printw("\nScore: %d\n", g_score);
}

struct timeval before_now, now;

int has_to_update() {
	return ((suseconds_t)(now.tv_sec * 1000000 + now.tv_usec) -
			((suseconds_t) before_now.tv_sec * 1000000 + before_now.tv_usec)) > FRAME_INTERVAL_USEC;
}

void spawn_random_tetromino() {
	t_tetromino new_shape = copy_shape(shapes[rand()%7]);
	new_shape.col = rand()%(COL-new_shape.width+1);
	new_shape.row = 0;
	destroy_shape(current);
	current = new_shape;
	if(!is_valid_position(current)) {
		g_game_on = false;
	}
}

void copy_current_to_table(void) {
	int i, j;
	for(i = 0; i < current.width ;i++){
		for(j = 0; j < current.width ; j++){
			if(current.array[i][j])
				g_table[current.row+i][current.col+j] = current.array[i][j];
		}
	}
}

bool is_completely_filled(int row) {
	for(int i = 0; i < COL; i++){
		if(!g_table[row][i])
			return false;
	}
	return true;
}

void clear_line(int row) { //TODO: var i, j -> なんかいい感じの変数名に
	int i, j;

	for(j = row;j >=1;j--)
		for(i=0;i<COL;i++)
			g_table[j][i]=g_table[j-1][i];
	for(i=0;i<COL;i++)
		g_table[j][i]=0;
}

void clear_lines(void) {
	for(int n=0;n<ROW;n++){
		if(is_completely_filled(n)){
			clear_line(n);
			g_score += 100;
		}
	}
}

void move_down(void) {
	t_tetromino temp = copy_shape(current);
	temp.row++;
	if (is_valid_position(temp)) {
		current.row++;
	} else {
		copy_current_to_table();
		clear_lines();
		spawn_random_tetromino();
	}
	destroy_shape(temp);
}

void move_right(void) {
	t_tetromino temp = copy_shape(current);
	temp.col++;
	if (is_valid_position(temp)) {
		current.col++;
	}
	destroy_shape(temp);
}

void move_left(void) {
	t_tetromino temp = copy_shape(current);
	temp.col--;
	if (is_valid_position(temp)) {
		current.col--;
	}
	destroy_shape(temp);
}

void rotate(void) { //TODO: rotate_clockwise
	t_tetromino temp = copy_shape(current);
	rotate_shape(temp);
	if (is_valid_position(temp)) {
		rotate_shape(current);
	}
	destroy_shape(temp);
}

void update_terminal(int input) {
	switch (input) {
		case TETROMINO_DOWN:
			move_down();
			break;
		case TETROMINO_RIGHT:
			move_right();
			break;
		case TETROMINO_LEFT:
			move_left();
			break;
		case TETROMINO_ROTATE:
			rotate();
			break;
	}
}

void print_result(void) {
	int i, j;
	for (i = 0; i < ROW; i++) {
		for (j = 0; j < COL; j++) {
			printf("%c ", g_table[i][j] ? '#' : '.');
		}
		putchar('\n');
	}
	printf("\nGame over!\n");
	printf("\nScore: %d\n", g_score);
}

int main() {
	srand(time(NULL));
	initscr();
	gettimeofday(&before_now, NULL);
	timeout(1);
	spawn_random_tetromino();
	print_tetris();
	int key_input;
	while (g_game_on) {
		if ((key_input = getch()) != ERR) {
			update_terminal(key_input);
			print_tetris();
		}
		gettimeofday(&now, NULL);
		if (has_to_update()) {
			update_terminal(TETROMINO_DOWN);
			print_tetris();
			gettimeofday(&before_now, NULL);
		}
	}
	destroy_shape(current);
	endwin();
	print_result();
	return 0;
}
