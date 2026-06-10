# Makefile para Tower Defense Mexicano
# Basado en estructura de EjemplosJuego

SRC_DIR := src
BIN_DIR := bin
INCLUDE_DIR := include

# Librerías de SFML 3.x
SFML := -lsfml-graphics -lsfml-window -lsfml-system

# Compiler
CXX := g++
CXXFLAGS := -std=c++17 -I$(INCLUDE_DIR) -Wall -Wextra

# Archivos fuente específicos
SOURCES := $(SRC_DIR)/main.cpp $(SRC_DIR)/Entity.cpp $(SRC_DIR)/Game.cpp $(SRC_DIR)/TextureManager.cpp $(SRC_DIR)/Map.cpp
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,obj/%.o,$(SOURCES))
EXECUTABLE := $(BIN_DIR)/TowerDefenseMexicano.exe

# Targets
all: $(EXECUTABLE)

# Crear directorios si no existen
$(shell mkdir -p $(BIN_DIR) obj)

# Compilar archivos objeto
obj/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Linkear ejecutable
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML)
	@echo "✓ Ejecutable creado: $@"

# Ejecutar el juego
run: $(EXECUTABLE)
	@echo "Iniciando Tower Defense Mexicano..."
	@./$(EXECUTABLE)

# Limpiar archivos compilados
clean:
	rm -rf obj $(BIN_DIR)
	@echo "✓ Limpieza completada"

# Recompilar todo
rebuild: clean all

# Ayuda
help:
	@echo "Comandos disponibles:"
	@echo "  make all     - Compilar el proyecto"
	@echo "  make run     - Compilar y ejecutar"
	@echo "  make clean   - Limpiar archivos compilados"
	@echo "  make rebuild - Limpiar y recompilar"

.PHONY: all run clean rebuild help



