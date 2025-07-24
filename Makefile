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

NAME		:= ircserv
CXX			:= c++
CXXFLAGS	:= -Wall -Wextra -Werror -std=c++98 -MMD -MP
OBJ_DIR		:= obj/
SRC_DIR		:= srcs/
INC_DIR		:= inc

INCLUDES	:= -I$(INC_DIR)
CMDS		:= cmds/
CMD_DIR		:= $(SRC_DIR)$(CMDS)
SRC_NAMES	:=	main.cpp \
				Server.cpp \
				Client.cpp \
				Channel.cpp \
				Handshake.cpp \
				Commands.cpp
CMDS_NAMES	:=	passCmd.cpp \
				nickCmd.cpp \
				userCmd.cpp \
				joinCmd.cpp \
				privmsgCmd.cpp \
				quitCmd.cpp \
				helpCmd.cpp \
				operCmd.cpp \
				dieCmd.cpp \
				partCmd.cpp \
				kickCmd.cpp \
				noticeCmd.cpp \
				inviteCmd.cpp \
				killCmd.cpp \
				modeCmd.cpp	\
				topicCmd.cpp

ALL_SRC_NAMES	:= $(SRC_NAMES) $(addprefix $(CMDS), $(CMDS_NAMES))

SRC			:= $(addprefix $(SRC_DIR), $(ALL_SRC_NAMES))
OBJ			:=$(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(SRC))

DEP			:= $(OBJ:.o=.d)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJ) -o $@
	@echo "Executable created: $(NAME)"

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp Makefile
	@echo "Compiling $< into $@"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEP)

.PHONY: all clean fclean re
