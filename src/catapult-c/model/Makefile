CXX := g++
CC := gcc

CXXFLAGS := -Iobj_dir -I/usr/share/verilator/include

SRC := sim_main.cpp $(wildcard obj_dir/*.cpp) verilated.cpp
OBJ := $(SRC:.cpp=.o)

all: sim_main

verilated.cpp: /usr/share/verilator/include/verilated.cpp
	cp $< $@

sim_main: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(OBJ)
	rm -f sim_main
	rm -f verilated.cpp

.PHONY: all clean
