NAME	=	tetris
CFLAGS	=	-Wall -Wextra -Werror

$(NAME): tetris.c
	gcc tetris.c -lncurses -o tetris $(CFLAGS)

.PHONY: all
all: $(NAME)

.PHONY: clean
clean:
	$(RM) *.o

.PHONY: fclean
fclean: clean
	$(RM) $(NAME)

.PHONY: re
re: fclean all
