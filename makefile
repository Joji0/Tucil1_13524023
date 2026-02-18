CXX      := g++
CXXFLAGS := -std=c++17 -O3 -Wall
TARGET   := main
SRC      := src/main.cpp
BINDIR   := bin

ifeq ($(OS),Windows_NT)
    SFML_DIR := lib/SFML
    CXXFLAGS += -I$(SFML_DIR)/include
    LDFLAGS  := -L$(SFML_DIR)/lib -lsfml-graphics -lsfml-window -lsfml-system
    EXE := $(BINDIR)/$(TARGET).exe
    CLEAN_CMD := del /q $(BINDIR)\$(TARGET).exe *.o 2>NUL
    COPY_DLL := copy /y "$(SFML_DIR)\bin\*.dll" $(BINDIR)\ >NUL 2>&1 || echo DLLs copied
    MKDIR_CMD := if not exist $(BINDIR) mkdir $(BINDIR)
else
    UNAME_S := $(shell uname -s)
    EXE := $(BINDIR)/$(TARGET)
    CLEAN_CMD := rm -f $(BINDIR)/$(TARGET) $(BINDIR)/$(TARGET).exe *.o
    COPY_DLL := @echo "Unix detected. Using system libraries."
    MKDIR_CMD := mkdir -p $(BINDIR)
    ifeq ($(UNAME_S), Darwin)
        ifneq ($(wildcard /opt/homebrew/include/SFML/Config.hpp),)
            CXXFLAGS += -I/opt/homebrew/include
            LDFLAGS  += -L/opt/homebrew/lib
        else
            CXXFLAGS += -I/usr/local/include
            LDFLAGS  += -L/usr/local/lib
        endif
    endif
    LDFLAGS += -lsfml-graphics -lsfml-window -lsfml-system
endif
all: $(EXE)
$(EXE): $(SRC)
	@echo "Compiling $(SRC) for $(if $(OS),Windows,Unix)..."
	$(MKDIR_CMD)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(EXE) $(LDFLAGS)
	@echo "Build Success! Output: $(EXE)"
	$(COPY_DLL)
run: all
	./$(EXE)
clean:
	$(CLEAN_CMD)
	@echo "Cleaned up binaries and objects."
.PHONY: all run clean
