#pragma once

#include "Topology.h"

#include <array>

namespace Sudoku
{

WFC::Topology<int> create();

void print(const WFC::Topology<int>& topology);

size_t getIndex(size_t x, size_t y);

std::array<size_t, 2> getCoord(size_t index);

}
