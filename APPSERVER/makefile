# Makefile para compilar main.cpp com as bibliotecas etsi_qkd_004 e load_ascii_20200819

# Compilador
CXX = g++

# Diretórios de cabeçalho
INCLUDES = -I/path/to/load_ascii_20200819/header

# Diretórios das bibliotecas
LDFLAGS = -L/path/to/load_ascii_20200819/library

# Bibliotecas a serem ligadas
LDLIBS =  -lload_ascii_20200819

# Nome do executável
TARGET = main

# Arquivo fonte
SRC = main_app.cpp

# Objeto gerado
OBJ = $(SRC:.cpp=.o)

# Regra padrão
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Regra para compilar os arquivos fonte
%.o: %.cpp
	$(CXX) $(INCLUDES) -c $< -o $@

# Limpa os arquivos gerados
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
