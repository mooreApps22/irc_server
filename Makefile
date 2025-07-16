RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
PURPLE = \033[0;35m
CYAN = \033[0;36m
NORMAL = \033[0m

NAME = ircserv
CPP = c++
FLAGS = -Wall -Wextra -Werror -std=c++98 -Wshadow -D_GLIBCXX_USE_CXX11_ABI=0 -g3

INC_DIR = ./inc
HEADERS = $(wildcard $(INC_DIR)/*.hpp)
			
SRC_DIR = ./src
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
			
OBJ_DIR = ./obj
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

$(info SRCS = $(SRCS))
$(info OBJS = $(OBJS))
all: $(OBJ_DIR) $(NAME)


$(OBJ_DIR):
	@echo "${CYAN}Making the directory for object files...${NORMAL}"
	@mkdir -p $(OBJ_DIR)
	@echo "${GREEN}    $(OBJ_DIR) was successfully created.${NORMAL}"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@echo "${YELLOW}Compiling $< into $@...${NORMAL}"
	@mkdir -p $(@D)
	@$(CPP) $(FLAGS) -I$(INC_DIR) -c $< -o $@
	@echo "${GREEN}    $@ successfully compiled.${NORMAL}"

$(NAME): $(OBJS)
	@echo "${YELLOW}Building $(NAME) and linking .o files...${NORMAL}"
	$(CPP) $(FLAGS) $(OBJS) -o $(NAME)
	@echo "${GREEN}    $(NAME) successfully built and .o files linked.${NORMAL}"

clean:
	@echo "${PURPLE}Deleting $(OBJ_DIR) and .o files...${NORMAL}"
	@rm -rf $(OBJ_DIR)
	@echo "${GREEN}    $(OBJ_DIR) and .o files successfully deleted.${NORMAL}"

fclean: clean
	@echo "${RED}Deleting $(NAME)...${NORMAL}"
	@rm $(NAME)
	@echo "${GREEN}    $(NAME) successfully deleted.${NORMAL}"

re: fclean all
	@echo "${BLUE}Relinking $(NAME)...${NORMAL}"
	@echo "${GREEN}    $(NAME) successfully relinked.${NORMAL}"

colors:
	@echo "${RED}xxxx...${NORMAL}"
	@echo "${GREEN}xxxx...${NORMAL}"
	@echo "${YELLOW}xxxx...${NORMAL}"
	@echo "${BLUE}xxxx...${NORMAL}"
	@echo "${PURPLE}xxxx...${NORMAL}"
	@echo "${CYAN}xxxx...${NORMAL}"

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./$(NAME)

gdb:
	@gdb --args ./$(NAME) 6667 password

al:
	irssi --config=irssi_config/al.conf

bob:
	irssi --config=irssi_config/bob.conf

carl:
	irssi --config=irssi_config/carl.conf

.PHONY: all clean fclean re
