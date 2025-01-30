#include "Pipes.h"
#include "Sudoku.h"
#include "Topology.h"

void examplePipes()
{
    WFC::Topology<char> topology = Pipes::create(150, 10);
    topology.weights[' '] = 10;
    topology.weights[char(180)] = 0;
    topology.weights[char(193)] = 0;
    topology.weights[char(194)] = 0;
    topology.weights[char(195)] = 0;

    topology.collapse();
    Pipes::print(topology, 150, 10);
}

void exampleSudoku()
{
    while (true)
    {
        try
        {
            WFC::Topology<int> topology = Sudoku::create();
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
