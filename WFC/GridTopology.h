/**
 * @file GridTopology.h
 * @brief GridTopology class for creating topologies with a grid structure.
 */

#pragma once

#include "Topology.h"

#include <map>
#include <array>
#include <vector>
#include <numeric>
#include <functional>

template <class State>
using Rule = std::function<bool(const Node<State>& target, const Node<State>& node, const State& nodeState)>;

/**
 * @brief GridTopology class for creating topologies with a grid structure.
 * 
 * The GridTopology class is a helper class for creating topologies with a grid structure.
 * Each node in the grid has adjacent nodes in each direction. Directions are determined by the number of dimensions (first negative, then positive).
 * For example, in 2D, the directions are [left, right, up, down]. In 3D, the directions are [left, right, up, down, back, front].
 * 
 * @tparam Dim The number of dimensions.
 */
template <size_t Dim>
class GridTopology
{
public:
    /**
     * @brief Get the index of a node with a specific coordinate.
     * @param coord The coordinate of the node.
     * @param size The size of the grid.
     * @return The index of the node.
     */
    static size_t getIndex(const std::array<size_t, Dim>& coord, const std::array<size_t, Dim>& size);

    /**
     * @brief Get the coordinate of a node with a specific index.
     * @param index The index of the node.
     * @param size The size of the grid.
     * @return The coordinate of the node.
     */
    static std::array<size_t, Dim> getCoord(size_t index, const std::array<size_t, Dim>& size);

    /**
     * @brief Create a grid topology with a specific size, states and weights.
     * 
     * If the weights are not specified, the default weight is 1.
     * 
     * @tparam State The type of the states.
     * @param size The size of the grid.
     * @param states The states of the nodes.
     * @param weights The weights of the states.
     * @return The grid topology.
     */
    template <class State>
    static Topology<State> createGrid(const std::array<size_t, Dim>& size, const std::vector<State>& states, const std::map<State, float>& weights = {});

    /**
     * @brief Create a grid topology with a specific size, rules and weights.
     * 
     * If the weights are not specified, the default weight is 1.
     * Rules are used to determine the compatibility of adjacent nodes.
     * 
     * @tparam State The type of the states.
     * @param size The size of the grid.
     * @param rules The rules of the states.
     * @param weights The weights of the states.
     * @return The grid topology.
     */
    template <class State>
    static Topology<State> createGridRules(const std::array<size_t, Dim>& size, const std::map<State, std::array<Rule<State>, Dim * 2>>& rules, const std::map<State, float>& weights = {});

    /**
     * @brief Create a grid topology with a specific size, adjacent states and weights.
     * 
     * If the weights are not specified, the default weight is 1.
     * The array of adjacent states contains the states that are compatible with the node in the corresponding direction.
     * 
     * @tparam State The type of the states.
     * @param size The size of the grid.
     * @param adjacent The adjacent states of the nodes.
     * @param weights The weights of the states.
     * @return The grid topology.
     */
    template <class State>
    static Topology<State> createGridAdjacent(const std::array<size_t, Dim>& size, const std::map<State, std::array<std::vector<State>, Dim * 2>>& adjacent, const std::map<State, float>& weights = {});

    /**
     * @brief Create a grid topology with a specific size, tokens and weights.
     * 
     * If the weights are not specified, the default weight is 1.
     * Tokens are used to determine the compatibility of adjacent nodes. If two adjacent nodes have the same token, they are compatible.
     * 
     * @tparam State The type of the states.
     * @tparam Token The type of the tokens.
     * @param size The size of the grid.
     * @param tokens The tokens of the nodes.
     * @param weights The weights of the states.
     * @return The grid topology.
     */
    template <class State, class Token>
    static Topology<State> createGridTokens(const std::array<size_t, Dim>& size, const std::map<State, std::array<Token, Dim * 2>>& tokens, const std::map<State, float>& weights = {});
private:
    GridTopology() {}
};

template <size_t Dim>
size_t GridTopology<Dim>::getIndex(const std::array<size_t, Dim>& coord, const std::array<size_t, Dim>& size)
{
    size_t accum = 1;
    return std::inner_product(
        coord.begin(),
        coord.end(),
        size.begin(),
        0,
        std::plus<size_t>(),
        [&accum](size_t coord_val, size_t size_val)
        {
            size_t result = coord_val * accum;
            accum *= size_val;
            return result;
        });
}

template <size_t Dim>
std::array<size_t, Dim> GridTopology<Dim>::getCoord(size_t index, const std::array<size_t, Dim>& size)
{
    std::array<size_t, Dim> coords;
    std::transform(
        size.begin(),
        size.end(),
        coords.begin(),
        [&index](size_t d)
        {
            size_t coord = index % d;
            index /= d;
            return coord;
        });

    return coords;
}

template <size_t Dim>
template <class State>
Topology<State> GridTopology<Dim>::createGrid(const std::array<size_t, Dim>& size, const std::vector<State>& states, const std::map<State, float>& weights)
{
    Topology<State> grid;
    grid.nodes = std::vector<Node<State>>(std::reduce(size.begin(), size.end(), 1, std::multiplies<size_t>()));
    grid.weights = weights;

    for (size_t i = 0; i < grid.nodes.size(); i++)
    {
        std::array<size_t, Dim> coords = GridTopology::getCoord(i, size);

        grid.nodes[i].states = states;
        grid.nodes[i].adjacent = std::vector<Node<State>*>(Dim * 2);

        for (size_t a = 0; a < Dim; a++)
        {
            std::array<size_t, Dim> coordsNegative = coords;
            coordsNegative[a] -= 1;
            grid.nodes[i].adjacent[2 * a] = coords[a] != 0 ? &grid.nodes[GridTopology::getIndex(coordsNegative, size)] : nullptr;

            std::array<size_t, Dim> coordsPositive = coords;
            coordsPositive[a] += 1;
            grid.nodes[i].adjacent[2 * a + 1] = coords[a] != size[a] - 1 ? &grid.nodes[GridTopology::getIndex(coordsPositive, size)] : nullptr;
        }
    }

    grid.compatible = [](const Node<State>&, const State&, const Node<State>&, const State&) { return true; };

    return grid;
}

template <size_t Dim>
template <class State>
Topology<State> GridTopology<Dim>::createGridRules(const std::array<size_t, Dim>& size, const std::map<State, std::array<Rule<State>, Dim * 2>>& rules, const std::map<State, float>& weights)
{
    std::vector<State> states;
    for (auto const& [key, val] : rules)
    {
        states.push_back(key);
    }

    Topology<State> grid = GridTopology::createGrid(size, states, weights);
    grid.compatible = [rules](const Node<State>& a, const State& aState, const Node<State>& b, const State& bState)
    {
        for (size_t i = 0; i < Dim * 2; i++)
        {
            size_t j = i ^ 1;
            if (a.adjacent[i] == &b && b.adjacent[j] == &a)
            {
                return rules.at(aState)[i](a, b, bState) && rules.at(bState)[j](b, a, aState);
            }
        }

        return false;
    };

    return grid;
}

template <size_t Dim>
template <class State>
Topology<State> GridTopology<Dim>::createGridAdjacent(const std::array<size_t, Dim>& size, const std::map<State, std::array<std::vector<State>, Dim * 2>>& adjacent, const std::map<State, float>& weights)
{
    std::vector<State> states;
    for (auto const& [key, val] : adjacent)
    {
        states.push_back(key);
    }

    Topology<State> grid = GridTopology::createGrid(size, states, weights);
    grid.compatible = [adjacent](const Node<State>& a, const State& aState, const Node<State>& b, const State& bState)
    {
        for (size_t i = 0; i < Dim * 2; i++)
        {
            size_t j = i ^ 1;
            if (a.adjacent[i] == &b && b.adjacent[j] == &a)
            {
                std::vector<State> availableA = adjacent.at(aState)[i];
                std::vector<State> availableB = adjacent.at(bState)[j];
                return std::find(availableA.begin(), availableA.end(), bState) != availableA.end() &&
                    std::find(availableB.begin(), availableB.end(), aState) != availableB.end();
            }
        }

        return false;
    };

    return grid;
}

template <size_t Dim>
template <class State, class Token>
Topology<State> GridTopology<Dim>::createGridTokens(const std::array<size_t, Dim>& size, const std::map<State, std::array<Token, Dim * 2>>& tokens, const std::map<State, float>& weights)
{
    std::vector<State> states;
    for (auto const& [key, val] : tokens)
    {
        states.push_back(key);
    }

    Topology<State> grid = GridTopology::createGrid(size, states, weights);
    grid.compatible = [tokens](const Node<State>& a, const State& aState, const Node<State>& b, const State& bState)
    {
        for (size_t dir = 0; dir < Dim * 2; dir++)
        {
            size_t opDir = dir ^ 1;
            if (a.adjacent[dir] == &b && b.adjacent[opDir] == &a)
            {
                return tokens.at(aState)[dir] == tokens.at(bState)[opDir];
            }
        }

        return false;
    };

    return grid;
}
