NAME = npuzzle

CXX = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -O2
RM = rm -rf

# Directories
INCLUDES_DIR = includes
SRCS_DIR = srcs
OBJ_DIR = obj

# Source files
SRCS =	$(SRCS_DIR)/main.cpp \
		$(SRCS_DIR)/Puzzle.cpp \
		$(SRCS_DIR)/Parser.cpp \
		$(SRCS_DIR)/Heuristics.cpp \
		$(SRCS_DIR)/Solver.cpp

# Object files
OBJS = $(patsubst $(SRCS_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Colors and Formatting
RESET_LINE = \033[2K\r
RED_FLASH = \033[5;31m
GREEN_LIGHT = \033[1;32m
YELLOW = \033[1;33m
BLUE = \033[1;34m
BLUE_LIGHT = \033[1;36m
CYAN = \033[1;36m
GREY = \033[1;30m
NOCOLOR = \033[0m

.PHONY: all clean fclean re help

all: $(NAME)

COUNT = 0
TOTAL := $(words $(SRCS))

$(NAME): $(OBJS)
	@echo "$(BLUE)🔨 Linking $(NAME)...$(NOCOLOR)"
	@$(CXX) $(CXXFLAGS) -I$(INCLUDES_DIR) $(OBJS) -o $@
	@printf "	$(RESET_LINE)$(BLUE_LIGHT)$(NAME)\t: $(GREEN_LIGHT)Updated\n$(NOCOLOR)"

$(OBJ_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(eval COUNT=$(shell expr $(COUNT) + 1))
	@printf "$(RESET_LINE)$(CYAN)📦 [%d/%d] Compiling $<...$(NOCOLOR)\n" $(COUNT) $(TOTAL)
	@$(CXX) $(CXXFLAGS) -I$(INCLUDES_DIR) -c $< -o $@

clean:
	@printf "$(RESET_LINE)$(RED_FLASH)Deleting $(NAME) obj/	$(GREY)"
	@sleep 0.1
	@printf "$(RESET_LINE)$(RED_FLASH)Deleting $(NAME) obj/.	$(GREY)"
	@sleep 0.1
	@printf "$(RESET_LINE)$(RED_FLASH)Deleting $(NAME) obj/..	$(GREY)"
	@sleep 0.1
	@printf "$(RESET_LINE)$(RED_FLASH)Deleting $(NAME) obj/...	$(GREY)"
	@sleep 0.1
	@printf "$(RESET_LINE)$(RED_FLASH)Deleting $(NAME) obj/	$(GREY)"
	@sleep 0.1
	@printf "$(RESET_LINE)$(RED_FLASH)Deleting $(NAME) obj/.	$(GREY)"
	@sleep 0.1
	@printf "$(RESET_LINE)$(RED_FLASH)Deleting $(NAME) obj/..	$(GREY)"
	@sleep 0.1
	@printf "$(RESET_LINE)$(RED_FLASH)Deleting $(NAME) obj/...	$(GREY)"
	@sleep 0.1
	@$(RM) $(OBJ_DIR)
	@printf "$(RESET_LINE)$(GREEN_LIGHT)Deleted successfully!\n$(NOCOLOR)"

fclean: clean
	@echo "$(YELLOW)🛑 Removing executable $(NAME)...$(NOCOLOR)"
	@$(RM) $(NAME)
	@echo "$(GREEN_LIGHT)✅ Full clean successful!$(NOCOLOR)"

re: fclean all

help:
	@echo "Usage: make [target]"
	@echo ""
	@echo "Available targets:"
	@echo "  all      - Build the $(NAME) executable"
	@echo "  clean    - Remove object files (deletes obj/ directory)"
	@echo "  fclean   - Remove object files and the executable"
	@echo "  re       - Rebuild the complete project (fclean + all)"
	@echo "  help     - Show this help message"
