NAME	=	tetris
CC		=	gcc
CFLAGS	=	-Wall -Wextra -Werror
LIBRARY	=	-lncurses
INCLUDE	=	-Iinclude
SRC_DIR	=	srcs
SRCS	=	$(SRC_DIR)/tetris.c \
			$(SRC_DIR)/tetris_tetromino.c \
			$(SRC_DIR)/tetris_move.c \
			$(SRC_DIR)/tetris_gravity.c \
			$(SRC_DIR)/tetris_print.c \

OBJ_DIR	=	objs
OBJS	=	$(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $^ -o $@ -c $(CFLAGS) $(INCLUDE)

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(LIBRARY) -o $(NAME) $(CFLAGS) $(INCLUDE)

.PHONY: all
all: $(NAME)

.PHONY: clean
clean:
	$(RM) *.o $(OBJ_DIR)/*.o

.PHONY: fclean
fclean: clean
	$(RM) $(NAME) $(NAME)_debug

.PHONY: re
re: fclean all

.PHONY: debug
debug: fclean
	gcc $(SRCS) -o $(NAME)_debug $(LIBRARY) $(CFLAGS) $(INCLUDE) -Weverything -Wno-padded -Wno-poison-system-directories
