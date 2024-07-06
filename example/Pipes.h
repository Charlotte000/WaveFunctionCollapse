#pragma once

#include "Topology.h"

#include <map>
#include <array>

class Pipes
{
public:
    static std::map<char, std::array<bool, 4>> tokens;
    static Topology<char> create(size_t w, size_t h);
    static void print(const Topology<char>& topology, size_t w, size_t h);
private:
    Pipes() {}
};
