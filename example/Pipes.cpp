#include "Pipes.h"
#include "GridTopology.h"

#include <vector>
#include <iostream>

std::map<char, std::array<bool, 4>> Pipes::tokens =
{
    //              l, r, u, d
    { char(0xC0), { 0, 1, 1, 0 } }, // └
    { char(0xD9), { 1, 0, 1, 0 } }, // ┘
    { char(0xDA), { 0, 1, 0, 1 } }, // ┌
    { char(0xBF), { 1, 0, 0, 1 } }, // ┐
    { char(0xC4), { 1, 1, 0, 0 } }, // ─
    { char(0xC1), { 1, 1, 1, 0 } }, // ┴
    { char(0xC2), { 1, 1, 0, 1 } }, // ┬
    { char(0xB3), { 0, 0, 1, 1 } }, // │
    { char(0xB4), { 1, 0, 1, 1 } }, // ┤
    { char(0xC3), { 0, 1, 1, 1 } }, // ├
    { char(0xC5), { 1, 1, 1, 1 } }, // ┼
    { char(' '),  { 0, 0, 0, 0 } }, // ' '
};

Topology<char> Pipes::create(size_t w, size_t h)
{
    return GridTopology<2>::createGridTokens<char, bool>({w, h}, Pipes::tokens);
}

void Pipes::print(const Topology<char>& topology, size_t w, size_t h)
{
    for (size_t y = 0; y < h; y++)
    {
        for (size_t x = 0; x < w; x++)
        {
            Node node = topology.nodes[GridTopology<2>::getIndex({x, y}, {w, h})];
            if (node.states.size() == 1)
            {
                std::cout << node.states[0];
            }
            else
            {
                std::cout << node.states.size();
            }
        }

        std::cout << std::endl;
    }
}
