#include "Pipes.h"
#include "GridTopology.h"

#include <vector>
#include <iostream>

std::map<char, std::array<bool, 4>> Pipes::tokens =
{
    //              l, r, u, d
    { char(' '),  { 0, 0, 0, 0 } }, // ' '
    { char(179), { 0, 0, 1, 1 } }, // │
    { char(180), { 1, 0, 1, 1 } }, // ┤
    { char(191), { 1, 0, 0, 1 } }, // ┐
    { char(192), { 0, 1, 1, 0 } }, // └
    { char(193), { 1, 1, 1, 0 } }, // ┴
    { char(194), { 1, 1, 0, 1 } }, // ┬
    { char(195), { 0, 1, 1, 1 } }, // ├
    { char(196), { 1, 1, 0, 0 } }, // ─
    { char(197), { 1, 1, 1, 1 } }, // ┼
    { char(217), { 1, 0, 1, 0 } }, // ┘
    { char(218), { 0, 1, 0, 1 } }, // ┌
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
