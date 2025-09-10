Name = webserv
cc = c++
flags = -Wall -Wextra -Werror -std=c++98   -fsanitize=address -g3
srcs = main.cpp config.cpp Utils.cpp
objs = $(srcs:.cpp=.o)

all : $(Name)

$(Name) : $(srcs)
		$(cc) $(flags) $(srcs) -o $(Name)

clean:
	rm -rf $(objs)

fclean : clean
	rm -rf $(Name)

re : fclean all
