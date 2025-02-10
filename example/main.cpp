#include "CartesianTopology.h"

#include <iostream>

void examplePipes(size_t w = 150, size_t h = 10)
{
    const std::map<char, std::array<std::vector<bool>, 4>> tokens
    {
        //                               l,   r,   u,   d
        { char(' '), { std::vector<bool>{0}, {0}, {0}, {0} } }, // ' '
        { char(179), { std::vector<bool>{0}, {0}, {1}, {1} } }, // │
        { char(180), { std::vector<bool>{1}, {0}, {1}, {1} } }, // ┤
        { char(191), { std::vector<bool>{1}, {0}, {0}, {1} } }, // ┐
        { char(192), { std::vector<bool>{0}, {1}, {1}, {0} } }, // └
        { char(193), { std::vector<bool>{1}, {1}, {1}, {0} } }, // ┴
        { char(194), { std::vector<bool>{1}, {1}, {0}, {1} } }, // ┬
        { char(195), { std::vector<bool>{0}, {1}, {1}, {1} } }, // ├
        { char(196), { std::vector<bool>{1}, {1}, {0}, {0} } }, // ─
        { char(197), { std::vector<bool>{1}, {1}, {1}, {1} } }, // ┼
        { char(217), { std::vector<bool>{1}, {0}, {1}, {0} } }, // ┘
        { char(218), { std::vector<bool>{0}, {1}, {0}, {1} } }, // ┌
    };

    WFC::CartesianTopology<2, char> topology = WFC::CartesianTopology<2, char>({w, h}, tokens);
    topology.weights[' '] = 10;
    topology.weights[char(180)] = 0;
    topology.weights[char(193)] = 0;
    topology.weights[char(194)] = 0;
    topology.weights[char(195)] = 0;

    topology.collapse();

    for (size_t y = 0; y < h; y++)
    {
        for (size_t x = 0; x < w; x++)
        {
            const WFC::Node<char>& node = topology.getNode({x, y});
            if (node.states.size() == 1)
            {
                std::cout << node.states[0];
            }
            else
            {
                std::cout << '.';
            }
        }

        std::cout << std::endl;
    }
}

void exampleSudoku(const std::array<int, 9 * 9>& field)
{
    WFC::CartesianTopology<2, int> topology({9, 9}, { 1, 2, 3, 4, 5, 6, 7, 8, 9 });

    for (size_t i = 0; i < topology.nodes.size(); i++)
    {
        topology.nodes[i].adjacent.clear();

        auto [x, y] = topology.getCoord(i);

        // Horizontal line
        for (size_t xx = 0; xx < 9; xx++)
        {
            if (xx == x) continue;
            topology.nodes[i].adjacent.push_back(&topology.getNode({xx, y}));
        }

        // Vertical line
        for (size_t yy = 0; yy < 9; yy++)
        {
            if (yy == y) continue;
            topology.nodes[i].adjacent.push_back(&topology.getNode({x, yy}));
        }

        // Block
        for (size_t xx = x / 3 * 3; xx < x / 3 * 3 + 3; xx++)
        {
            for (size_t yy = y / 3 * 3; yy < y / 3 * 3 + 3; yy++)
            {
                if (xx == x || yy == y) continue;
                topology.nodes[i].adjacent.push_back(&topology.getNode({xx, yy}));
            }
        }
    }

    topology.compatible = [](const WFC::Node<int>&, const int& aState, const WFC::Node<int>&, const int& bState) { return aState != bState; };

    for (size_t i = 0; i < field.size(); i++)
    {
        if (field[i] != 0)
        {
            topology.collapseNode(topology.nodes[i], field[i]);
        }
    }

    topology.collapse();

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

            const WFC::Node<int>& node = topology.getNode({x, y});
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

int main()
{
    // examplePipes();

    // const std::array<int, 9 * 9> field =
    // {
    //     0,0,0,  0,0,0,  0,0,0,
    //     0,0,0,  0,0,0,  0,0,0,
    //     0,0,0,  0,0,0,  0,0,0,

    //     0,0,0,  1,2,3,  0,0,0,
    //     0,0,0,  4,5,6,  0,0,0,
    //     0,0,0,  7,8,9,  0,0,0,

    //     0,0,0,  0,0,0,  0,0,0,
    //     0,0,0,  0,0,0,  0,0,0,
    //     0,0,0,  0,0,0,  0,0,0,
    // };
    // while (true)
    // {
    //     try
    //     {
    //         exampleSudoku(field);
    //         break;
    //     }
    //     catch (const std::runtime_error&)
    //     {
    //     }
    // }

    WFC::CartesianTopology<2, int> t(
        {10, 10},
        {0, 1, 2, 3},
        std::array<std::function<bool(const int&, const int&)>, 2>
        {
            [](const int& left, const int& right) { return left <= right; },
            [](const int& up, const int& down) { return up <= down; },
        }
    );
    t.collapseNode(t.getNode({0, 0}), 0);
    t.collapseNode(t.getNode({9, 9}), 0);
    t.collapse();

    for (size_t y = 0; y < t.size[1]; y++)
    {
        for (size_t x = 0; x < t.size[0]; x++)
        {
            std::cout << t.getNode({x, y}).states[0];
        }
        std::cout << '\n';
    }

    return 0;
}
