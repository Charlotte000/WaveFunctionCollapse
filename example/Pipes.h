#pragma once

#include "Topology.h"

#include <map>
#include <array>

namespace Pipes
{

WFC::Topology<char> create(size_t w, size_t h);

void print(const WFC::Topology<char>& topology, size_t w, size_t h);

}
