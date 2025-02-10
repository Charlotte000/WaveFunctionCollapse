/**
 * @file Topology.h
 * @brief Topology class for the Wave Function Collapse algorithm.
 * 
 * The topology is a container for nodes and weights.
 */
#pragma once

#include <map>
#include <queue>
#include <time.h>
#include <vector>
#include <random>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <functional>

namespace WFC
{

/**
 * @brief Node class for the topology.
 * 
 * A node is a container for states and the adjacent nodes.
 * 
 * @tparam State The type of the states.
 */
template <class State>
struct Node
{
    /**
     * @brief All possible states of the node. 
     */
    std::vector<State> states;

    /**
     * @brief The adjacent nodes are the nodes that are connected to the current node.
     */
    std::vector<Node<State>*> adjacent;
};

/**
 * @brief Topology class for the Wave Function Collapse algorithm.
 * 
 * The topology is a container for nodes and weights.
 * 
 * @tparam State The type of the states.
 */
template <class State>
class Topology
{
public:
    /**
     * @brief Nodes are elements of the topology.
     */
    std::vector<Node<State>> nodes;

    /**
     * @brief Weights are used to bias the selection of states.
     */
    std::map<State, float> weights;

    /**
     * @brief The compatible function is used to check if two states are compatible.
     * 
     * This function has to be defined by the user and should be symmetric ( compatible(a, b) == compatible(b, a) ).
     * @param a The first node.
     * @param aState The state of the first node.
     * @param b The second node.
     * @param bState The state of the second node.
     * @return True if the states are compatible, false otherwise.
     */
    std::function<bool(const Node<State>&, const State&, const Node<State>&, const State&)> compatible;

    Topology() = default;

    Topology(const Topology<State>& topology);

    /**
     * @brief Collapse the topology using the Wave Function Collapse algorithm.
     * @param seed The seed for the random number generator.
     * @throw std::runtime_error If no valid states are found.
     */
    void collapse(unsigned int seed = time(NULL));

    /**
     * @brief Collapse a node with a specific state.
     * @param node The node to collapse.
     * @param state The state to collapse the node with.
     * @throw std::logic_error If the state is not valid.
     */
    void collapseNode(Node<State>& node, const State& state);

    /**
     * @brief Check if the topology is correct.
     * 
     * A topology is correct if all nodes have only one state and all adjacent nodes are compatible.
     * 
     * @return True if the topology is correct, false otherwise.
     */
    bool isCorrect() const;
private:
    bool isCollapsed() const;
    Node<State>* getMinEntropy(std::mt19937& randGen);
    void propagate(Node<State>& node);
    bool reduceStates(Node<State>& a);
    State getState(const Node<State>& node, std::mt19937& randGen) const;
    bool isPlaceable(const Node<State>& node, const State& state) const;
};

template <class State>
Topology<State>::Topology(const Topology<State>& topology)
{
    this->nodes = topology.nodes;
    for (Node<State>& node : this->nodes)
    {
        for (Node<State>*& adj : node.adjacent)
        {
            adj = adj == nullptr ? nullptr : &this->nodes[adj - topology.nodes.data()];
        }
    }

    this->weights = topology.weights;
    this->compatible = topology.compatible;
}

template <class State>
void Topology<State>::collapse(unsigned int seed)
{
    std::mt19937 randGen(seed);
    while (!this->isCollapsed())
    {
        Node<State>* node = this->getMinEntropy(randGen);
        State state = this->getState(*node, randGen);
        this->collapseNode(*node, state);
    }
}

template <class State>
void Topology<State>::collapseNode(Node<State>& node, const State& state)
{
    auto it = std::find(node.states.begin(), node.states.end(), state);
    if (it == node.states.end())
    {
        throw std::logic_error("Invalid state to collapse");
    }

    node.states = { *it };
    this->propagate(node);
}

template <class State>
bool Topology<State>::isCorrect() const
{
    return std::all_of(
        this->nodes.begin(),
        this->nodes.end(),
        [this](const Node<State>& a)
        {
            return a.states.size() == 1 &&
                std::all_of(
                    a.adjacent.begin(),
                    a.adjacent.end(),
                    [this, &a](const Node<State>* b)
                    {
                        return b == nullptr || b->states.size() == 1 && this->compatible(a, a.states.at(0), *b, b->states.at(0));
                    });
        });
}

template <class State>
bool Topology<State>::isCollapsed() const
{
    return std::all_of(this->nodes.begin(), this->nodes.end(), [](const Node<State>& node) { return node.states.size() == 1; });
}

template <class State>
Node<State>* Topology<State>::getMinEntropy(std::mt19937& randGen)
{
    size_t minEntropy = -1;
    for (size_t i = 0; i < this->nodes.size(); i++)
    {
        size_t entropy = this->nodes[i].states.size();
        if (entropy < minEntropy && entropy != 1)
        {
            minEntropy = entropy;
        }
    }

    std::vector<Node<State>*> minNodes;
    for (Node<State>& node : this->nodes)
    {
        if (node.states.size() == minEntropy)
        {
            minNodes.push_back(&node);
        }
    }

    return minNodes[randGen() % minNodes.size()];
}

template <class State>
void Topology<State>::propagate(Node<State>& node)
{
    std::queue<Node<State>*> queue({ &node });
    std::vector<Node<State>*> visited({ &node });
    visited.reserve(this->nodes.size());
    while (!queue.empty())
    {
        Node<State>* current = queue.front();
        queue.pop();

        for (Node<State>* neighbour : current->adjacent)
        {
            if (neighbour != nullptr &&
                std::find(visited.begin(), visited.end(), neighbour) == visited.end() &&
                this->reduceStates(*neighbour))
            {
                queue.push(neighbour);
                visited.push_back(neighbour);
            }
        }
    }
}

template <class State>
bool Topology<State>::reduceStates(Node<State>& a)
{
    std::vector<State> newStates;
    std::copy_if(
        a.states.begin(),
        a.states.end(),
        std::back_inserter(newStates),
        [this, &a](const State& aState) { return this->isPlaceable(a, aState); });

    bool changed = newStates.size() != a.states.size();
    a.states = newStates;
    if (newStates.empty())
    {
        throw std::runtime_error("No valid states");
    }

    return changed;
}

template <class State>
State Topology<State>::getState(const Node<State>& a, std::mt19937& randGen) const
{
    std::vector<State> aStates;
    std::vector<double> aWeights;
    for (const State& aState : a.states)
    {
        float aWeight = this->weights.find(aState) != this->weights.end() ? this->weights.at(aState) : 1;
        if (aWeight > 0 && this->isPlaceable(a, aState))
        {
            aStates.push_back(aState);
            aWeights.push_back(aWeight);
        }
    }

    if (aStates.size() == 0)
    {
        throw std::runtime_error("No valid states");
    }

    std::discrete_distribution<size_t> randState(aWeights.begin(), aWeights.end());
    return aStates[randState(randGen)];
}

template <class State>
bool Topology<State>::isPlaceable(const Node<State>& node, const State& state) const
{
    return std::all_of(
        node.adjacent.begin(),
        node.adjacent.end(),
        [this, &node, &state](const Node<State>* adjacent)
        {
            return adjacent == nullptr ||
                std::any_of(
                    adjacent->states.begin(),
                    adjacent->states.end(),
                    [this, &node, adjacent, &state](const State& adjacentState)
                    {
                        return this->compatible(node, state, *adjacent, adjacentState);
                    });
        });
}

}
