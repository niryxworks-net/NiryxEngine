# Compilador y flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2 -Iinclude -IC:/raylib/include
LDFLAGS = -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm

# Directorios
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

# Archivos fuente
SOURCES = $(SRC_DIR)/main.cpp \
          $(SRC_DIR)/engine.cpp \
          $(SRC_DIR)/splash_screen.cpp \
          $(SRC_DIR)/dialogue_system.cpp \
          $(SRC_DIR)/resource_manager.cpp \
          $(SRC_DIR)/character.cpp \
          $(SRC_DIR)/scene_manager.cpp \
          $(SRC_DIR)/dialogue_parser.cpp

# Archivos objeto
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

# Ejecutable
TARGET = $(BUILD_DIR)/PotatoCake.exe

# Icono (opcional)
ICON_RES = $(BUILD_DIR)/icon.res
ICON_RC = resources/icon/icon.rc

# Regla principal
all: $(BUILD_DIR) $(TARGET)

# Crear directorio build si no existe
$(BUILD_DIR):
	@if not exist "$(BUILD_DIR)" mkdir $(BUILD_DIR)

# Compilar ejecutable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo Compilacion exitosa!

# Compilar archivos objeto
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpiar archivos compilados
clean:
	@if exist "$(BUILD_DIR)\*.o" del /Q $(BUILD_DIR)\*.o
	@if exist "$(TARGET)" del /Q $(TARGET)
	@echo Limpieza completa!

# Ejecutar el juego
run: $(TARGET)
	@echo Ejecutando PotatoCake...
	@cd $(BUILD_DIR) && PotatoCake.exe

# Compilar con icono (opcional)
with-icon: $(BUILD_DIR) $(ICON_RES) $(OBJECTS)
	$(CXX) $(OBJECTS) $(ICON_RES) -o $(TARGET) $(LDFLAGS)
	@echo Compilacion con icono exitosa!

$(ICON_RES): $(ICON_RC)
	windres $(ICON_RC) -o $(ICON_RES)

# Recompilar todo
rebuild: clean all

# Debug mode
debug: CXXFLAGS += -g -DDEBUG
debug: clean all

# Release mode (optimizado)
release: CXXFLAGS += -O3 -DNDEBUG
release: clean all

.PHONY: all clean run rebuild debug release with-icon