#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <ncurses.h>
#include <stdbool.h>

#define ROW 20
#define COLUMN 15

typedef enum e_key{
	TETROMINO_DOWN = 's',
	TETROMINO_LEFT = 'a',
	TETROMINO_RIGHT = 'd',
	TETROMINO_ROTATE = 'w',
} t_key;


char g_table[ROW][COLUMN] = {0};
int g_final = 0;
bool g_game_on = true;
suseconds_t timer = 400000;
int decrease = 1000;

typedef struct {
	char **array;
	int width, row, col;
} t_shape;
t_shape current;

const t_shape shapes[7] = {
		{(char *[]) {(char[]) {0, 1, 1}, (char[]) {1, 1, 0}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {1, 1, 0}, (char[]) {0, 1, 1}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {0, 1, 0}, (char[]) {1, 1, 1}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {0, 0, 1}, (char[]) {1, 1, 1}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {1, 0, 0}, (char[]) {1, 1, 1}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {1, 1}, (char[]) {1, 1}},                                                           2},
		{(char *[]) {(char[]) {0, 0, 0, 0}, (char[]) {1, 1, 1, 1}, (char[]) {0, 0, 0, 0}, (char[]) {0, 0, 0, 0}}, 4}
};

t_shape copy_shape(t_shape shape) {
	t_shape new_shape = shape;
	char **copyshape = shape.array;
	new_shape.array = (char **) malloc(new_shape.width * sizeof(char *));
	int i, j;
	for (i = 0; i < new_shape.width; i++) {
		new_shape.array[i] = (char *) malloc(new_shape.width * sizeof(char));
		for (j = 0; j < new_shape.width; j++) {
			new_shape.array[i][j] = copyshape[i][j];
		}
	}
	return new_shape;
}

void destroy_shape(t_shape shape) {
	int i;
	for (i = 0; i < shape.width; i++) {
		free(shape.array[i]);
	}
	free(shape.array);
}

bool is_valid_position(t_shape shape) {
	char **array = shape.array;
	int i, j;
	for (i = 0; i < shape.width; i++) {
		for (j = 0; j < shape.width; j++) {
			if ((shape.col + j < 0 || shape.col + j >= COLUMN || shape.row + i >= ROW)) {
				if (array[i][j])
					return false;

			} else if (g_table[shape.row + i][shape.col + j] && array[i][j])
				return false;
		}
	}
	return true;
}

void rotate_shape(t_shape shape) {
	t_shape temp = copy_shape(shape);
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
	char buffer[ROW][COLUMN] = {0};
	int i, j;
	for (i = 0; i < current.width; i++) {
		for (j = 0; j < current.width; j++) {
			if (current.array[i][j])
				buffer[current.row + i][current.col + j] = current.array[i][j];
		}
	}
	clear();
	for (i = 0; i < COLUMN - 9; i++)
		printw(" ");
	printw("42 Tetris\n");
	for (i = 0; i < ROW; i++) {
		for (j = 0; j < COLUMN; j++) {
			printw("%c ", (g_table[i][j] + buffer[i][j]) ? '#' : '.');
		}
		printw("\n");
	}
	printw("\nScore: %d\n", g_final);
}

struct timeval before_now, now;

int has_to_update() {
	return ((suseconds_t)(now.tv_sec * 1000000 + now.tv_usec) -
			((suseconds_t) before_now.tv_sec * 1000000 + before_now.tv_usec)) > timer;
}

void create_random_shape() {
	t_shape new_shape = copy_shape(shapes[rand()%7]);
	new_shape.col = rand()%(COLUMN-new_shape.width+1);
	new_shape.row = 0;
	destroy_shape(current);
	current = new_shape;
	if(!is_valid_position(current)) {
		g_game_on = false;
	}
}

void move_down(void) {
	t_shape temp = copy_shape(current);
	temp.row++;
	if (is_valid_position(temp)) {
		current.row++;
	} else {
		int i, j;
		for(i = 0; i < current.width ;i++){
			for(j = 0; j < current.width ; j++){
				if(current.array[i][j])
					g_table[current.row+i][current.col+j] = current.array[i][j];
			}
		}
		int n, m, sum, count=0;
		for(n=0;n<ROW;n++){
			sum = 0;
			for(m=0;m< COLUMN;m++) {
				sum+=g_table[n][m];
			}
			if(sum==COLUMN){
				count++;
				int l, k;
				for(k = n;k >=1;k--)
					for(l=0;l<COLUMN;l++)
						g_table[k][l]=g_table[k-1][l];
				for(l=0;l<COLUMN;l++)
					g_table[k][l]=0;
				timer-=decrease--;
			}
		}
		g_final += 100*count;
		create_random_shape();
	}
	destroy_shape(temp);
}

void move_right(void) {
	t_shape temp = copy_shape(current);
	temp.col++;
	if (is_valid_position(temp)) {
		current.col++;
	}
	destroy_shape(temp);
}

void move_left(void) {
	t_shape temp = copy_shape(current);
	temp.col--;
	if (is_valid_position(temp)) {
		current.col--;
	}
	destroy_shape(temp);
}

void rotate(void) {
	t_shape temp = copy_shape(current);
	rotate_shape(temp);
	if (is_valid_position(temp)) {
		rotate_shape(current);
	}
	destroy_shape(temp);
}

void update_terminal(int input) {
	switch (c) {
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
		for (j = 0; j < COLUMN; j++) {
			printf("%c ", g_table[i][j] ? '#' : '.');
		}
		putchar('\n');
	}
	printf("\nGame over!\n");
	printf("\nScore: %d\n", g_final);
}

int main() {
	srand(time(NULL));
	initscr();
	gettimeofday(&before_now, NULL);
	timeout(1);
	create_random_shape();
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
