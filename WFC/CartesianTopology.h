/**
 * @file CartesianTopology.h
 * @brief Creating topologies with a grid structure.
 */

#pragma once

#include "Topology.h"

#include <map>
#include <array>
#include <vector>
#include <numeric>
#include <functional>

/**
 * @brief CartesianTopology for creating topologies with a grid structure.
 * 
 * The CartesianTopology namespace is a helper for creating topologies with a grid structure.
 * Each node in the grid has adjacent nodes in each direction. Directions are determined by the number of dimensions (first negative, then positive).
 * For example, in 2D, the directions are [left, right, up, down]. In 3D, the directions are [left, right, up, down, back, front].
 */
namespace WFC::CartesianTopology
{

template <class State>
using Rule = std::function<bool(const Node<State>& target, const Node<State>& node, const State& nodeState)>;

template <size_t Dim>
using Vec = std::array<size_t, Dim>;

/**
 * @brief Get the index of a node with a specific coordinate.
 * @tparam Dim The number of dimensions.
 * @param coord The coordinate of the node.
 * @param size The size of the grid.
 * @return The index of the node.
 */
template <size_t Dim>
size_t getIndex(const Vec<Dim>& coord, const Vec<Dim>& size)
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

/**
 * @brief Get the coordinate of a node with a specific index.
 * @tparam Dim The number of dimensions.
 * @param index The index of the node.
 * @param size The size of the grid.
 * @return The coordinate of the node.
 */
template <size_t Dim>
Vec<Dim> getCoord(size_t index, const Vec<Dim>& size)
{
    Vec<Dim> coords;
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

/**
 * @brief Create a cartesian topology with a specific size, states and weights.
 * 
 * If the weights are not specified, the default weight is 1.
 * 
 * @tparam Dim The number of dimensions.
 * @tparam State The type of the states.
 * @param size The size of the grid.
 * @param states The states of the nodes.
 * @param periods Whether the grid is periodic (true) or not (false) in each dimension.
 * @param weights The weights of the states.
 * @return The grid topology.
 */
template <size_t Dim, class State>
Topology<State> createCart(const Vec<Dim>& size, const std::vector<State>& states, const std::array<bool, Dim>& periods = {}, const std::map<State, float>& weights = {})
{
    Topology<State> grid;
    grid.nodes = std::vector<Node<State>>(std::reduce(size.begin(), size.end(), 1, std::multiplies<size_t>()));
    grid.weights = weights;

    for (size_t i = 0; i < grid.nodes.size(); i++)
    {
        Vec<Dim> coords = CartesianTopology::getCoord(i, size);

        grid.nodes[i].states = states;
        grid.nodes[i].adjacent = std::vector<Node<State>*>(Dim * 2);
        for (size_t a = 0; a < Dim; a++)
        {
            Vec<Dim> coordsNegative = coords, coordsPositive = coords;
            coordsNegative[a] = coords[a] != 0 ? coords[a] - 1 : size[a] - 1;
            coordsPositive[a] = coords[a] != size[a] - 1 ? coords[a] + 1 : 0;
            grid.nodes[i].adjacent[2 * a] = coords[a] != 0 || periods[a] ? &grid.nodes[CartesianTopology::getIndex(coordsNegative, size)] : nullptr;
            grid.nodes[i].adjacent[2 * a + 1] = coords[a] != size[a] - 1 || periods[a] ? &grid.nodes[CartesianTopology::getIndex(coordsPositive, size)] : nullptr;
        }
    }

    grid.compatible = [](const Node<State>&, const State&, const Node<State>&, const State&) { return true; };

    return grid;
}

/**
 * @brief Create a cartesian topology with a specific size, rules and weights.
 * 
 * If the weights are not specified, the default weight is 1.
 * Rules are used to determine the compatibility of adjacent nodes.
 * 
 * @tparam Dim The number of dimensions.
 * @tparam State The type of the states.
 * @param size The size of the grid.
 * @param rules The rules of the states.
 * @param periods Whether the grid is periodic (true) or not (false) in each dimension.
 * @param weights The weights of the states.
 * @return The grid topology.
 */
template <size_t Dim, class State>
Topology<State> createCartRules(const Vec<Dim>& size, const std::map<State, std::array<Rule<State>, Dim * 2>>& rules, const std::array<bool, Dim>& periods = {}, const std::map<State, float>& weights = {})
{
    std::vector<State> states;
    states.reserve(rules.size());
    std::transform(rules.begin(), rules.end(), std::back_inserter(states), [](const auto& kv) { return kv.first; });

    Topology<State> grid = CartesianTopology::createCart(size, states, periods, weights);
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

/**
 * @brief Create a cartesian topology with a specific size, adjacent states and weights.
 * 
 * If the weights are not specified, the default weight is 1.
 * The array of adjacent states contains the states that are compatible with the node in the corresponding direction.
 * 
 * @tparam Dim The number of dimensions.
 * @tparam State The type of the states.
 * @param size The size of the grid.
 * @param adjacent The adjacent states of the nodes.
 * @param periods Whether the grid is periodic (true) or not (false) in each dimension.
 * @param weights The weights of the states.
 * @return The grid topology.
 */
template <size_t Dim, class State>
Topology<State> createCartAdjacent(const Vec<Dim>& size, const std::map<State, std::array<std::vector<State>, Dim * 2>>& adjacent, const std::array<bool, Dim>& periods = {}, const std::map<State, float>& weights = {})
{
    std::vector<State> states;
    states.reserve(adjacent.size());
    std::transform(adjacent.begin(), adjacent.end(), std::back_inserter(states), [](const auto& kv) { return kv.first; });

    Topology<State> grid = CartesianTopology::createCart(size, states, periods, weights);
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

/**
 * @brief Create a cartesian topology with a specific size, tokens and weights.
 * 
 * If the weights are not specified, the default weight is 1.
 * Tokens are used to determine the compatibility of adjacent nodes. If two adjacent nodes have the same token, they are compatible.
 * 
 * @tparam Dim The number of dimensions.
 * @tparam State The type of the states.
 * @tparam Token The type of the tokens.
 * @param size The size of the grid.
 * @param tokens The tokens of the nodes.
 * @param periods Whether the grid is periodic (true) or not (false) in each dimension.
 * @param weights The weights of the states.
 * @return The grid topology.
 */
template <size_t Dim, class State, class Token>
Topology<State> createCartTokens(const Vec<Dim>& size, const std::map<State, std::array<Token, Dim * 2>>& tokens, const std::array<bool, Dim>& periods = {}, const std::map<State, float>& weights = {})
{
    std::vector<State> states;
    states.reserve(tokens.size());
    std::transform(tokens.begin(), tokens.end(), std::back_inserter(states), [](const auto& kv) { return kv.first; });

    Topology<State> grid = CartesianTopology::createCart(size, states, periods, weights);
    grid.compatible = [tokens](const Node<State>& a, const State& aState, const Node<State>& b, const State& bState)
    {
        for (size_t i = 0; i < Dim * 2; i++)
        {
            size_t j = i ^ 1;
            if (a.adjacent[i] == &b && b.adjacent[j] == &a)
            {
                return tokens.at(aState)[i] == tokens.at(bState)[j];
            }
        }

        return false;
    };

    return grid;
}

}
