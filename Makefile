NAME = ircserv

CC = c++

FLAGS = -Wall -Werror -Wextra -std=c++98

INCLUDES = -Iincludes

SRC_DIR = src
OBJ_DIR = obj

SRC = main.cpp Client.cpp Executor.cpp Channel.cpp \
	  server/Server.cpp server/Connection.cpp server/Messaging.cpp \
	  server/ClientManager.cpp server/ChannelManager.cpp \
	  commands/Registration.cpp commands/ChannelCommands.cpp \
	  commands/Messaging.cpp commands/Mode.cpp commands/Connection.cpp

SRCS = $(addprefix $(SRC_DIR)/, $(SRC))

OBJS = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(FLAGS) $(INCLUDES) -o $(NAME) $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
