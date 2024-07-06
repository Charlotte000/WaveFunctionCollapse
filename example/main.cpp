#include "Pipes.h"
#include "Sudoku.h"
#include "Topology.h"

void examplePipes()
{
    Topology<char> topology = Pipes::create(150, 10);
    topology.weights[' '] = 10;
    topology.weights[char(0xC1)] = 0;
    topology.weights[char(0xC2)] = 0;
    topology.weights[char(0xC3)] = 0;
    topology.weights[char(0xB4)] = 0;

    topology.collapse();
    Pipes::print(topology, 150, 10);
}

void exampleSudoku()
{
    while (true)
    {
        try
        {
            Topology<int> topology = Sudoku::create();
            topology.collapse();
            Sudoku::print(topology);
            break;
        }
        catch(const std::runtime_error&)
        { }
    }
}

int main()
{
    examplePipes();
    exampleSudoku();
    return 0;
}
