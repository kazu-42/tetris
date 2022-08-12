NAME	=	tetris
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
	gcc $(OBJS) $(LIBRARY) -o $(NAME) $(CFLAGS) $(INCLUDE)

.PHONY: all
all: $(NAME)

.PHONY: clean
clean:
	$(RM) *.o $(OBJ_DIR)/*.o

.PHONY: fclean
fclean: clean
	$(RM) $(NAME)

.PHONY: re
re: fclean all

.PHONY: debug
debug: fclean
	gcc $(SRC_DIR)/*.c $(LIBRARY) -o $(NAME) $(CFLAGS) $(INCLUDE) -Weverything
