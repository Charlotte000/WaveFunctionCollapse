#include "Sudoku.h"
#include "Topology.h"

#include <vector>
#include <iostream>

WFC::Topology<int> Sudoku::create()
{
    std::vector<int> states = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    WFC::Topology<int> topology;
    topology.nodes = std::vector<WFC::Node<int>>(81);

    for (size_t i = 0; i < topology.nodes.size(); i++)
    {
        auto [x, y] = Sudoku::getCoord(i);

        topology.nodes[i].states = states;

        // Horizontal line
        for (size_t xx = 0; xx < 9; xx++)
        {
            if (xx == x) continue;
            topology.nodes[i].adjacent.push_back(&topology.nodes[Sudoku::getIndex(xx, y)]);
        }

        // Vertical line
        for (size_t yy = 0; yy < 9; yy++)
        {
            if (yy == y) continue;
            topology.nodes[i].adjacent.push_back(&topology.nodes[Sudoku::getIndex(x, yy)]);
        }

        // Block
        for (size_t xx = x / 3 * 3; xx < x / 3 * 3 + 3; xx++)
        {
            for (size_t yy = y / 3 * 3; yy < y / 3 * 3 + 3; yy++)
            {
                if (xx == x || yy == y) continue;
                topology.nodes[i].adjacent.push_back(&topology.nodes[Sudoku::getIndex(xx, yy)]);
            }
        }
    }

    topology.compatible = [](const WFC::Node<int>&, const int& aState, const WFC::Node<int>&, const int& bState) { return aState != bState; };
    return topology;
}

void Sudoku::print(const WFC::Topology<int>& topology)
{
    for (size_t y = 0; y < 9; y++)
    {
        if (y == 0)
        {
            std::cout << char(0xDA);
            std::cout << char(0xC4) << char(0xC4) << char(0xC4);
            std::cout << char(0xC2);
            std::cout << char(0xC4) << char(0xC4) << char(0xC4);
            std::cout << char(0xC2);
            std::cout << char(0xC4) << char(0xC4) << char(0xC4);
            std::cout << char(0xBF);
            std::cout << std::endl;

        }
        else if (y % 3 == 0)
        {
            std::cout << char(0xC3);
            std::cout << char(0xC4) << char(0xC4) << char(0xC4);
            std::cout << char(0xC5);
            std::cout << char(0xC4) << char(0xC4) << char(0xC4);
            std::cout << char(0xC5);
            std::cout << char(0xC4) << char(0xC4) << char(0xC4);
            std::cout << char(0xB4);
            std::cout << std::endl;
        }

        for (size_t x = 0; x < 9; x++)
        {
            if (x % 3 == 0) std::cout << char(0xB3);

            WFC::Node node = topology.nodes[Sudoku::getIndex(x, y)];
            if (node.states.size() == 1)
            {
                std::cout << node.states[0];
            }
            else
            {
                std::cout << '.';
            }
        }

        std::cout << char(0xB3) << std::endl;
    }

    std::cout << char(0xC0);
    std::cout << char(0xC4) << char(0xC4) << char(0xC4);
    std::cout << char(0xC1);
    std::cout << char(0xC4) << char(0xC4) << char(0xC4);
    std::cout << char(0xC1);
    std::cout << char(0xC4) << char(0xC4) << char(0xC4);
    std::cout << char(0xD9);
    std::cout << std::endl;
}

size_t Sudoku::getIndex(size_t x, size_t y)
{
    return y * 9 + x;
}

std::array<size_t, 2> Sudoku::getCoord(size_t index)
{
    size_t y = index / 9;
    size_t x = index - y * 9;
    return { x, y };
}
