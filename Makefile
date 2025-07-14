# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: atudor <marvin@42.fr>                      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/08 19:34:34 by atudor            #+#    #+#              #
#    Updated: 2025/07/08 19:39:19 by atudor           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME    := ircserv
CXX     := c++
CXXFLAGS:= -Wall -Wextra -Werror -std=c++98
SRC_DIR := srcs
INC_DIR := inc
SRC_NAMES := main.cpp \
             Server.cpp \
			 Client.cpp \
			 Channel.cpp

SRC     := $(addprefix $(SRC_DIR)/, $(SRC_NAMES))

OBJ     := $(SRC:.cpp=.o)
INCLUDES:= -I$(INC_DIR)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
