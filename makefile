# Compiler and flags
CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g -I./include -I/usr/include/SDL2 -D_REENTRANT
LDFLAGS  = -L/usr/lib/x86_64-linux-gnu -lSDL2 -lSDL2_image -lSDL2_ttf

# Directories
SRC_DIRS  = src client
OBJ_DIR   = obj
BIN_DIR   = bin
TARGET    = $(BIN_DIR)/minesweeper
IMGUI_DIR = external/imgui
IMGUI_BACKEND = $(IMGUI_DIR)/backends
IMGUI_SOURCES = \
	$(IMGUI_DIR)/imgui.cpp \
	$(IMGUI_DIR)/imgui_draw.cpp \
	$(IMGUI_DIR)/imgui_tables.cpp \
	$(IMGUI_DIR)/imgui_widgets.cpp \
	$(IMGUI_BACKEND)/imgui_impl_sdl2.cpp \
	$(IMGUI_BACKEND)/imgui_impl_sdlrenderer2.cpp
CXXFLAGS += -I$(IMGUI_DIR) -I$(IMGUI_BACKEND)


SOURCES = $(foreach d, $(SRC_DIRS), $(wildcard $(d)/*.cpp)) $(IMGUI_SOURCES)

OBJECTS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)


$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)          
	$(CXX) $(CXXFLAGS) -c $< -o $@


$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)


gdb: $(TARGET)
	gdb $(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)


WIN_CXX = x86_64-w64-mingw32-g++

WIN_CXXFLAGS = -Wall -Wextra -std=c++17 -I./include \
	-Iwinlibs/boost_1_87 \
	-Iwinlibs/SDL2-2.30.0/x86_64-w64-mingw32/include \
	-Iwinlibs/SDL2-2.30.0/x86_64-w64-mingw32/include/SDL2 \
	-Iwinlibs/SDL2_image-2.8.2/x86_64-w64-mingw32/include \
	-Iwinlibs/SDL2_image-2.8.2/x86_64-w64-mingw32/include/SDL2 \
	-Iwinlibs/SDL2_ttf-2.22.0/x86_64-w64-mingw32/include
WIN_CXXFLAGS += -Iexternal/imgui -Iexternal/imgui/backends


WIN_LDFLAGS = -Lwinlibs/SDL2-2.30.0/x86_64-w64-mingw32/lib \
	-Lwinlibs/SDL2_image-2.8.2/x86_64-w64-mingw32/lib \
	-Lwinlibs/SDL2_ttf-2.22.0/x86_64-w64-mingw32/lib \
	-lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf \
	-lws2_32 -mwindows



WIN_TARGET = $(BIN_DIR)/minesweeper.exe
WIN_DLL_DIRS = winlibs/SDL2-2.30.0/x86_64-w64-mingw32/bin \
	winlibs/SDL2_image-2.8.2/x86_64-w64-mingw32/bin \
	winlibs/SDL2_ttf-2.22.0/x86_64-w64-mingw32/bin

windows: $(WIN_TARGET)

$(WIN_TARGET): $(SOURCES) | $(BIN_DIR)
	$(WIN_CXX) $(WIN_CXXFLAGS) $(SOURCES) -o $@ $(WIN_LDFLAGS)
	@echo "Compiled for Windows"
	@for dir in $(WIN_DLL_DIRS); do \
		cp $$dir/*.dll $(BIN_DIR)/; \
	done
	@echo "Done"
