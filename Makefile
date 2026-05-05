CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

# Pastas
SRC_DIR = src
INC_DIR = include
TEST_DIR = tests
OBJ_DIR = obj

# Arquivos
SRCS = $(SRC_DIR)/ast.c $(SRC_DIR)/lexer.c $(SRC_DIR)/parser.c $(SRC_DIR)/interpreter.c
MAIN = $(TEST_DIR)/main.c
OBJS = $(OBJ_DIR)/ast.o $(OBJ_DIR)/lexer.o $(OBJ_DIR)/parser.o $(OBJ_DIR)/interpreter.o $(OBJ_DIR)/main.o

# Nome do executável final
TARGET = axis_run

# Regra principal
all: $(OBJ_DIR) $(TARGET)

# Linkagem final
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Compilação dos objetos de src
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compilação do main que está em tests
$(OBJ_DIR)/main.o: $(MAIN)
	$(CC) $(CFLAGS) -c $< -o $@

# Cria a pasta de objetos se não existir
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Limpeza
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean