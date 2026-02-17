CXX      := g++
CXXFLAGS := -std=c++17 -O3 -Wall
TARGET   := main
SRC      := main.cpp
ifeq ($(OS),Windows_NT)
    SFML_LOCAL := lib/SFML
    CXXFLAGS += -I$(SFML_LOCAL)/include
    LDFLAGS  := -L$(SFML_LOCAL)/lib -lsfml-graphics -lsfml-window -lsfml-system
    EXE := $(TARGET).exe
    CLEAN_CMD := del /q $(TARGET).exe *.o
    COPY_DLL := xcopy /y /i "$(SFML_LOCAL)\bin\*.dll" .
else
    UNAME_S := $(shell uname -s)
    EXE := $(TARGET)
    CLEAN_CMD := rm -f $(EXE) *.o
    COPY_DLL := @echo "Unix system detected. Using system libraries."
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
	@echo "Detected OS: $(if $(OS),Windows,Unix/Mac)"
	$(CXX) $(CXXFLAGS) $(SRC) -o $(EXE) $(LDFLAGS)
	@echo "Build Success!"
	$(COPY_DLL)
run: all
	./$(EXE)
clean:
	$(CLEAN_CMD)
.PHONY: all run clean
