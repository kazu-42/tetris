#include "move.h"
#include <ncurses.h>

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

// Returns true if the board has room and move is applied
bool try_move(t_move move, t_tetromino *piece, const t_board board) {
    if (has_room_to_move(move, *piece, board)) {
        apply_move(move, piece);
        return true;
    } else {
        return false;
    }
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

static bool has_room_to_move(t_move move, const t_tetromino piece, const t_board board) {
    bool	has_room_to_move;
    t_tetromino temp;

    temp = duplicate_tetromino(piece);
    apply_move(move, &temp);
    has_room_to_move = is_valid_position(temp, board);
    destroy_tetromino(temp);
    return has_room_to_move;
}








