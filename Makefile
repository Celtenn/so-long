NAME = solong
CC = cc
CFLAGS = -Wall -Wextra -Werror
INCLUDES = -Iminilibx-linux
LIBS = -Lminilibx-linux -lmlx -lXext -lX11 -lm
SRC = deneme.c
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) $(INCLUDES) $(LIBS) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re