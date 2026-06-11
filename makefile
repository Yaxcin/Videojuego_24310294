# Makefile para Tower Defense Mexicano

SRC_DIR := src
BIN_DIR := bin
INCLUDE_DIR := include

SFML := -lsfml-graphics -lsfml-window -lsfml-system

CXX := g++
CXXFLAGS := -std=c++17 -I$(INCLUDE_DIR) -Wall -Wextra

SOURCES := $(SRC_DIR)/main.cpp $(SRC_DIR)/Entity.cpp $(SRC_DIR)/Game.cpp $(SRC_DIR)/TextureManager.cpp $(SRC_DIR)/Map.cpp $(SRC_DIR)/Enemies/Pinata.cpp $(SRC_DIR)/Towers/Tower.cpp
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,obj/%.o,$(SOURCES))
EXECUTABLE := $(BIN_DIR)/TowerDefenseMexicano.exe

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML)
	@echo "Ejecutable creado: $@"

obj/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(EXECUTABLE)
	@./$(EXECUTABLE)

clean:
	rm -rf obj $(BIN_DIR)
	@echo "Limpieza completada"

rebuild: clean
	$(MAKE) all

.PHONY: all run clean rebuild help
