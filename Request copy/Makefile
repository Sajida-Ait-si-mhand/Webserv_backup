NAME = ParcingRequest
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++11 -fsanitize=address -g3

SRCS = http_parser.cpp
OBJS = $(SRCS:.cpp=.o)

PINK = \033[35m
BLUE = \033[34m
NC = \033[0m

all: $(NAME)

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(PINK)✓ Built $(NAME)$(NC)"

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "$(BLUE)Compiled: $<$(NC)"

clean:
	@rm -f $(OBJS)
	@echo "$(BLUE)Cleaned objects$(NC)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(PINK)Removed $(NAME)$(NC)"

re: fclean all

.PHONY: all clean fclean re