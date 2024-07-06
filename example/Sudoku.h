#pragma once

#include "Topology.h"

#include <array>

class Sudoku
{
public:
    static Topology<int> create();
    static void print(const Topology<int>& topology);
    static size_t getIndex(size_t x, size_t y);
    static std::array<size_t, 2> getCoord(size_t index);
private:
    Sudoku() {}
};
