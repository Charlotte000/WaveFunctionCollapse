#pragma once

#include "Node.h"

#include <map>
#include <queue>
#include <time.h>
#include <random>
#include <vector>
#include <stdexcept>
#include <functional>

/**
 * @brief Topology class for the Wave Function Collapse algorithm.
 * 
 * The topology is a container for nodes and weights.
 * 
 * @tparam T The type of the states.
 */
template <class T>
class Topology
{
public:
    std::vector<Node<T>> nodes;
    std::map<T, float> weights;
    std::function<bool(const Node<T>&, const T&, const Node<T>&, const T&)> compatible;

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
    void collapseNode(Node<T>& node, const T& state);

    /**
     * @brief Check if the topology is correct.
     * 
     * A topology is correct if all nodes have only one state and all adjacent nodes are compatible.
     * 
     * @return True if the topology is correct, false otherwise.
     */
    bool isCorrect();
private:
    bool isCollapsed();
    Node<T>* getMinEntropy(std::mt19937& randGen);
    void propagate(Node<T>& node);
    bool reduceStates(Node<T>& a);
    T getState(const Node<T>& node, std::mt19937& randGen);
    bool isPlaceable(const Node<T>& node, const T& state);
};

template <class T>
void Topology<T>::collapse(unsigned int seed)
{
    std::mt19937 randGen(seed);
    while (!this->isCollapsed())
    {
        Node<T>* node = this->getMinEntropy(randGen);
        T state = this->getState(*node, randGen);
        this->collapseNode(*node, state);
    }
}

template <class T>
void Topology<T>::collapseNode(Node<T>& node, const T& state)
{
    auto it = std::find_if(node.states.begin(), node.states.end(), [&state](T& n) { return n == state; });
    if (it == node.states.end())
    {
        throw std::logic_error("Invalid state to collapse");
    }

    node.states = { *it };
    this->propagate(node);
}

template <class T>
bool Topology<T>::isCorrect()
{
    return std::all_of(
        this->nodes.begin(),
        this->nodes.end(),
        [this](const Node<T>& a)
        {
            return a.states.size() == 1 &&
                std::all_of(
                    a.adjacent.begin(),
                    a.adjacent.end(),
                    [this, &a](const Node<T>* b)
                    {
                        return b == nullptr || b->states.size() == 1 && this->compatible(a, a.states.at(0), *b, b->states.at(0));
                    });
        });
}

template <class T>
bool Topology<T>::isCollapsed()
{
    return std::all_of(this->nodes.begin(), this->nodes.end(), [](const Node<T>& node) { return node.states.size() == 1; });
}

template <class T>
Node<T>* Topology<T>::getMinEntropy(std::mt19937& randGen)
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

    std::vector<Node<T>*> minNodes;
    for (Node<T>& node : this->nodes)
    {
        if (node.states.size() == minEntropy)
        {
            minNodes.push_back(&node);
        }
    }

    return minNodes[randGen() % minNodes.size()];
}

template <class T>
void Topology<T>::propagate(Node<T>& node)
{
    std::queue<Node<T>*> queue;
    std::vector<Node<T>*> visited;

    queue.push(&node);
    visited.push_back(&node);
    while (!queue.empty())
    {
        Node<T>* current = queue.front();
        queue.pop();

        for (Node<T>* neighbour : current->adjacent)
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

template <class T>
bool Topology<T>::reduceStates(Node<T>& a)
{
    std::vector<T> newStates;
    std::copy_if(
        a.states.begin(),
        a.states.end(),
        std::back_inserter(newStates),
        [this, &a](const T& aState) { return this->isPlaceable(a, aState); });

    bool changed = newStates.size() != a.states.size();
    a.states = newStates;
    if (newStates.empty())
    {
        throw std::runtime_error("No valid states");
    }

    return changed;
}

template <class T>
T Topology<T>::getState(const Node<T>& a, std::mt19937& randGen)
{
    std::vector<T> aStates;
    std::vector<double> aWeights;
    for (const T& aState : a.states)
    {
        float aWeight = this->weights.find(aState) != this->weights.end() ? this->weights[aState] : 1;
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

template <class T>
bool Topology<T>::isPlaceable(const Node<T>& node, const T& state)
{
    return std::all_of(
        node.adjacent.begin(),
        node.adjacent.end(),
        [this, &node, &state](const Node<T>* adjacent)
        {
            return adjacent == nullptr ||
                std::any_of(
                    adjacent->states.begin(),
                    adjacent->states.end(),
                    [this, &node, adjacent, &state](const T& adjacentState)
                    {
                        return this->compatible(node, state, *adjacent, adjacentState);
                    });
        });
}
