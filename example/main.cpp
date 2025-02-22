#include "CartesianTopology.h"

#include <iostream>

template <size_t Dim, class State>
WFC::CartesianTopology<Dim, State> tryCollapse(const WFC::CartesianTopology<Dim, State>& topology)
{
    for (size_t i = 0; i < 100; i++)
    {
        try
        {
            WFC::CartesianTopology<Dim, State> topologyCopy = topology;
            topologyCopy.collapse();
            return topologyCopy;
        }
        catch (const std::runtime_error&)
        {}
    }

    throw std::runtime_error("Unable to collapse");
}

template <class State>
void printTopology(const WFC::CartesianTopology<2, State>& topology)
{
    for (size_t y = 0; y < topology.size[1]; y++)
    {
        for (size_t x = 0; x < topology.size[0]; x++)
        {
            const WFC::Node<State>& node = topology.getNode({ x, y });
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

void examplePipes()
{
    // Create topology
    const std::unordered_map<char, std::array<std::vector<bool>, 4>> tokens
    {
        //                                l,     r,     u,     d
        { char(' '), { std::vector<bool>{ 0 }, { 0 }, { 0 }, { 0 } } }, // ' '
        { char(179), { std::vector<bool>{ 0 }, { 0 }, { 1 }, { 1 } } }, // │
        { char(191), { std::vector<bool>{ 1 }, { 0 }, { 0 }, { 1 } } }, // ┐
        { char(192), { std::vector<bool>{ 0 }, { 1 }, { 1 }, { 0 } } }, // └
        { char(196), { std::vector<bool>{ 1 }, { 1 }, { 0 }, { 0 } } }, // ─
        { char(197), { std::vector<bool>{ 1 }, { 1 }, { 1 }, { 1 } } }, // ┼
        { char(217), { std::vector<bool>{ 1 }, { 0 }, { 1 }, { 0 } } }, // ┘
        { char(218), { std::vector<bool>{ 0 }, { 1 }, { 0 }, { 1 } } }, // ┌
    };

    WFC::CartesianTopology<2, char> topology = WFC::CartesianTopology<2, char>({ 150, 10 }, tokens);
    topology.weights[' '] = 20;

    // Collapse
    WFC::CartesianTopology<2, char> topologyCollapsed = tryCollapse(topology);

    // Print
    printTopology(topologyCollapsed);
}

void exampleSudoku()
{
    // Create topology
    WFC::CartesianTopology<2, int> topology({ 9, 9 }, { 1, 2, 3, 4, 5, 6, 7, 8, 9 });

    for (size_t i = 0; i < topology.nodes.size(); i++)
    {
        topology.nodes[i].adjacent.clear();

        auto [x, y] = topology.getCoord(i);

        // Horizontal line
        for (size_t xx = 0; xx < 9; xx++)
        {
            if (xx == x) continue;
            topology.nodes[i].adjacent.push_back(&topology.getNode({ xx, y }));
        }

        // Vertical line
        for (size_t yy = 0; yy < 9; yy++)
        {
            if (yy == y) continue;
            topology.nodes[i].adjacent.push_back(&topology.getNode({ x, yy }));
        }

        // Block
        for (size_t xx = x / 3 * 3; xx < x / 3 * 3 + 3; xx++)
        {
            for (size_t yy = y / 3 * 3; yy < y / 3 * 3 + 3; yy++)
            {
                if (xx == x || yy == y) continue;
                topology.nodes[i].adjacent.push_back(&topology.getNode({ xx, yy }));
            }
        }
    }

    topology.compatible = [](const WFC::Node<int>&, const int& aState, const WFC::Node<int>&, const int& bState) { return aState != bState; };

    // Collapse known cells
    const std::array<int, 9 * 9> field =
    {
        0,0,0,  0,0,0,  0,0,0,
        0,0,0,  0,0,0,  0,0,0,
        0,0,0,  0,0,0,  0,0,0,

        0,0,0,  1,2,3,  0,0,0,
        0,0,0,  4,5,6,  0,0,0,
        0,0,0,  7,8,9,  0,0,0,

        0,0,0,  0,0,0,  0,0,0,
        0,0,0,  0,0,0,  0,0,0,
        0,0,0,  0,0,0,  0,0,0,
    };

    for (size_t i = 0; i < field.size(); i++)
    {
        if (field[i] != 0)
        {
            topology.collapseNode(topology.nodes[i], field[i]);
        }
    }

    // Collapse
    WFC::CartesianTopology<2, int> topologyCollapsed = tryCollapse(topology);

    // Print
    for (size_t y = 0; y < 9; y++)
    {
        if (y % 3 == 0)
        {
            std::cout << "+---+---+---+" << std::endl;
        }

        for (size_t x = 0; x < 9; x++)
        {
            if (x % 3 == 0) std::cout << '|';

            const WFC::Node<int>& node = topologyCollapsed.getNode({ x, y });
            if (node.states.size() == 1)
            {
                std::cout << node.states[0];
            }
            else
            {
                std::cout << '.';
            }
        }

        std::cout << '|' << std::endl;
    }

    std::cout << "+---+---+---+" << std::endl;
}

void exampleRules()
{
    // Create topology
    WFC::CartesianTopology<2, int> topology(
        { 10, 10 },
        { 0, 1, 2, 3 },
        std::array<std::function<bool(const int&, const int&)>, 2>
        {
            [](const int& left, const int& right) { return left <= right; },
            [](const int& up, const int& down) { return up <= down; },
        }
    );
    topology.collapseNode(topology.getNode({ 0, 0 }), 0);
    topology.collapseNode(topology.getNode({ 9, 9 }), 3);

    // Collapse
    WFC::CartesianTopology<2, int> topologyCollapsed = tryCollapse(topology);

    // Print
    printTopology(topologyCollapsed);
}

struct CustomState
{
    int value;

    bool operator==(const CustomState& cs) const
    {
        return this->value == cs.value;
    }
};

template <>
struct std::hash<CustomState>
{
    std::size_t operator()(const CustomState& cs) const
    {
        return std::hash<int>()(cs.value);
    }
};

void exampleCustonState()
{
    WFC::CartesianTopology<2, CustomState> topology({ 10, 10 }, std::vector<CustomState>{ { 0 }, { 1 }, { 2 } });
    topology.collapseNode(topology.getNode({ 5, 5 }), { 1 });
    topology.collapse();
}

int main()
{
    examplePipes();
    exampleSudoku();
    exampleRules();
    exampleCustonState();
    return 0;
}
