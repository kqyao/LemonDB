NAME		= lib.a
FILES		= $(shell ls | grep -E ".+\.cpp")
OBJ			= $(FILES:%.cpp=%.o)

# Optimization and Compiler flags and commands
CC = g++
CFLAGS = -Werror -std=c++14 -Iincludes -c

all: $(NAME)

# This won't run if the .o files don't exist or are not modified
$(NAME): $(OBJ)
	@ar rcs $(NAME) $(OBJ)

# This won't run if the source files don't exist or are not modified
$(OBJ): $(FILES)
	@g++ $(CFLAGS) $(FILES)

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f $(NAME)

re: fclean all


.PHONY: clean fclean all re
