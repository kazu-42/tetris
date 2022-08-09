#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <ncurses.h>
#include <stdbool.h>

#define ROW 20
#define COLUMN 15

char Table[ROW][COLUMN] = {0};
int final = 0;
bool GameOn = true;
suseconds_t timer = 400000;
int decrease = 1000;

typedef struct {
	char **array;
	int width, row, col;
} Shape;
Shape current;

const Shape StructsArray[7] = {
		{(char *[]) {(char[]) {0, 1, 1}, (char[]) {1, 1, 0}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {1, 1, 0}, (char[]) {0, 1, 1}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {0, 1, 0}, (char[]) {1, 1, 1}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {0, 0, 1}, (char[]) {1, 1, 1}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {1, 0, 0}, (char[]) {1, 1, 1}, (char[]) {0, 0, 0}},                                 3},
		{(char *[]) {(char[]) {1, 1}, (char[]) {1, 1}},                                                           2},
		{(char *[]) {(char[]) {0, 0, 0, 0}, (char[]) {1, 1, 1, 1}, (char[]) {0, 0, 0, 0}, (char[]) {0, 0, 0, 0}}, 4}
};

Shape FunctionCreateShape(Shape shape) {
	Shape new_shape = shape;
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

void FunctionDestroyShape(Shape shape) {
	int i;
	for (i = 0; i < shape.width; i++) {
		free(shape.array[i]);
	}
	free(shape.array);
}

bool FunctionCheckPosition(Shape shape) {
	char **array = shape.array;
	int i, j;
	for (i = 0; i < shape.width; i++) {
		for (j = 0; j < shape.width; j++) {
			if ((shape.col + j < 0 || shape.col + j >= COLUMN || shape.row + i >= ROW)) {
				if (array[i][j])
					return false;

			} else if (Table[shape.row + i][shape.col + j] && array[i][j])
				return false;
		}
	}
	return true;
}

void FunctionRotateShape(Shape shape) {
	Shape temp = FunctionCreateShape(shape);
	int i, j, k, width;
	width = shape.width;
	for (i = 0; i < width; i++) {
		for (j = 0, k = width - 1; j < width; j++, k--) {
			shape.array[i][j] = temp.array[k][i];
		}
	}
	FunctionDestroyShape(temp);
}

void FunctionPrint() {
	char Buffer[ROW][COLUMN] = {0};
	int i, j;
	for (i = 0; i < current.width; i++) {
		for (j = 0; j < current.width; j++) {
			if (current.array[i][j])
				Buffer[current.row + i][current.col + j] = current.array[i][j];
		}
	}
	clear();
	for (i = 0; i < COLUMN - 9; i++)
		printw(" ");
	printw("42 Tetris\n");
	for (i = 0; i < ROW; i++) {
		for (j = 0; j < COLUMN; j++) {
			printw("%c ", (Table[i][j] + Buffer[i][j]) ? '#' : '.');
		}
		printw("\n");
	}
	printw("\nScore: %d\n", final);
}

struct timeval before_now, now;

int hasToUpdate() {
	return ((suseconds_t)(now.tv_sec * 1000000 + now.tv_usec) -
			((suseconds_t) before_now.tv_sec * 1000000 + before_now.tv_usec)) > timer;
}

void set_timeout(int time) {
	time = 1;
	timeout(1);
}

void update_terminal(int c) {
	Shape temp = FunctionCreateShape(current);
	switch (c) {
		case 's':
			temp.row++;  //move down
			if (FunctionCheckPosition(temp))
				current.row++;
			else {
				int i, j;
				for (i = 0; i < current.width; i++) {
					for (j = 0; j < current.width; j++) {
						if (current.array[i][j])
							Table[current.row + i][current.col + j] = current.array[i][j];
					}
				}
				int n, m, sum, count = 0;
				for (n = 0; n < ROW; n++) {
					sum = 0;
					for (m = 0; m < COLUMN; m++) {
						sum += Table[n][m];
					}
					if (sum == COLUMN) {
						count++;
						int l, k;
						for (k = n; k >= 1; k--)
							for (l = 0; l < COLUMN; l++)
								Table[k][l] = Table[k - 1][l];
						for (l = 0; l < COLUMN; l++)
							Table[k][l] = 0;
						timer -= decrease--;
					}
				}
				final += 100 * count;
				Shape new_shape = FunctionCreateShape(StructsArray[rand() % 7]);
				new_shape.col = rand() % (COLUMN - new_shape.width + 1);
				new_shape.row = 0;
				FunctionDestroyShape(current);
				current = new_shape;
				if (!FunctionCheckPosition(current)) {
					GameOn = false;
				}
			}
			break;
		case 'd':
			temp.col++;
			if (FunctionCheckPosition(temp))
				current.col++;
			break;
		case 'a':
			temp.col--;
			if (FunctionCheckPosition(temp))
				current.col--;
			break;
		case 'w':
			FunctionRotateShape(temp);
			if (FunctionCheckPosition(temp))
				FunctionRotateShape(current);
			break;
	}
	FunctionDestroyShape(temp);
	FunctionPrint();
}


int main() {
	srand(time(0));
	final = 0;
	int c;
	initscr();
	gettimeofday(&before_now, NULL);
	set_timeout(1);
	Shape new_shape = FunctionCreateShape(StructsArray[rand() % 7]);
	new_shape.col = rand() % (COLUMN - new_shape.width + 1);
	new_shape.row = 0;
	FunctionDestroyShape(current);
	current = new_shape;
	if (!FunctionCheckPosition(current)) {
		GameOn = false;
	}
	FunctionPrint();
	while (GameOn) {
		if ((c = getch()) != ERR) {
			update_terminal(c);
		}
		gettimeofday(&now, NULL);
		if (hasToUpdate()) {
			update_terminal('s');
			gettimeofday(&before_now, NULL);
		}
	}
	FunctionDestroyShape(current);
	endwin();
	int i, j;
	for (i = 0; i < ROW; i++) {
		for (j = 0; j < COLUMN; j++) {
			printf("%c ", Table[i][j] ? '#' : '.');
		}
		printf("\n");
	}
	printf("\nGame over!\n");
	printf("\nScore: %d\n", final);
	return 0;
}
