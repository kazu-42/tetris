#ifndef CONFIG_H
#define CONFIG_H

# define TETRIS_TITLE "42 tetris"
# define ROW_SIZE 20
# define COL_SIZE 15
# define COLOR_ENABLED false
# define EMPTY_CELL '.'
# define FILLED_CELL '#'
# define UNKNOWN_CELL '?'

# define CURSES_READ_INTERVAL_MILLISEC 1
# define SCORE_PER_LINE 100
// 1/64 = 0.0156G
# define GRAVITY_INCREASE_PER_LINE 0.0156
# define DEFAULT_GRAVITY 0.0156
# define MAX_GRAVITY 1.0
# define FRAME_PER_SECOND 60
// 0.5 second
# define SOFT_DROP_HOLD_THRESHOLD_USEC 500000

#endif //CONFIG_H
