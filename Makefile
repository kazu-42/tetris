NAME	=	tetris
CFLAGS	=	-Wall -Wextra -Werror
LIBRARY	=	-lncurses

$(NAME): tetris.c
	gcc tetris.c $(LIBRARY) -o $(NAME) $(CFLAGS)

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

.PHONY: debug
debug: fclean
	gcc tetris.c $(LIBRARY) -o $(NAME) $(CFLAGS) -Weverything
