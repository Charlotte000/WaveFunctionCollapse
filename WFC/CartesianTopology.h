/**
 * @file CartesianTopology.h
 * @brief Creating topologies with a grid structure.
 */
#pragma once

#include "Topology.h"

#include <array>
#include <numeric>

namespace WFC
{

template <size_t Dim>
using Vec = std::array<size_t, Dim>;

/**
 * @brief CartesianTopology for creating topologies with a grid structure.
 * 
 * Each node in the grid has adjacent nodes in each direction. Directions are determined by the number of dimensions (first negative, then positive).
 * For example, in 2D, the directions are [left, right, up, down]. In 3D, the directions are [left, right, up, down, back, front].
 * @tparam Dim The number of dimensions.
 * @tparam State The type of the states.
 */
template <size_t Dim, class State>
class CartesianTopology : public Topology<State>
{
public:
    const Vec<Dim> size;

    /**
     * @brief Create a cartesian topology with a specific size, states and weights.
     * 
     * If the weights are not specified, the default weight is 1.
     * 
     * @param size The size of the grid.
     * @param states The states of the nodes.
     * @param periods Whether the grid is periodic (true) or not (false) in each dimension.
     * @param weights The weights of the states.
     */
    CartesianTopology(const Vec<Dim>& size, const std::vector<State>& states, const std::array<bool, Dim>& periods = {}, const std::map<State, float>& weights = {});

    /**
     * @brief Create a cartesian topology with a specific size, adjacent states and weights.
     * 
     * If the weights are not specified, the default weight is 1.
     * The array of adjacent states contains the states that are compatible with the node in the corresponding direction.
     * 
     * @param size The size of the grid.
     * @param adjacent The adjacent states of the nodes.
     * @param periods Whether the grid is periodic (true) or not (false) in each dimension.
     * @param weights The weights of the states.
     */
    CartesianTopology(const Vec<Dim>& size, const std::map<State, std::array<std::vector<State>, Dim * 2>>& adjacent, const std::array<bool, Dim>& periods = {}, const std::map<State, float>& weights = {});

    CartesianTopology(const Vec<Dim>& size, const std::vector<State>& states, const std::array<std::function<bool(const State& a, const State& b)>, Dim>& rules, const std::array<bool, Dim>& periods = {}, const std::map<State, float>& weights = {});

    /**
     * @brief Create a cartesian topology with a specific size, tokens and weights.
     * 
     * If the weights are not specified, the default weight is 1.
     * Tokens are used to determine the compatibility of adjacent nodes. If two adjacent nodes have the same token, they are compatible.
     * 
     * @tparam Token The type of the tokens.
     * @param size The size of the grid.
     * @param tokens The tokens of the nodes.
     * @param periods Whether the grid is periodic (true) or not (false) in each dimension.
     * @param weights The weights of the states.
     */
    template <class Token>
    CartesianTopology(const Vec<Dim>& size, const std::map<State, std::array<std::vector<Token>, Dim * 2>>& tokens, const std::array<bool, Dim>& periods = {}, const std::map<State, float>& weights = {});

    /**
     * @brief Get the node with a specific coordinate.
     * @param coord The coordinate of the node.
     * @return The node.
     */
    Node<State>& getNode(const Vec<Dim>& coord);

    /**
     * @brief Get the node with a specific coordinate.
     * @param coord The coordinate of the node.
     * @return The node.
     */
    const Node<State>& getNode(const Vec<Dim>& coord) const;

    /**
     * @brief Get the index of a node with a specific coordinate.
     * @param coord The coordinate of the node.
     * @return The index of the node.
     */
    size_t getIndex(const Vec<Dim>& coord) const;

    /**
     * @brief Get the coordinate of a node with a specific index.
     * @param index The index of the node.
     * @param size The size of the grid.
     * @return The coordinate of the node.
     */
    Vec<Dim> getCoord(size_t index) const;
};

template <class Key, class Value>
std::vector<Key> __getKeys(const std::map<Key, Value>& map)
{
    std::vector<Key> keys;
    keys.reserve(map.size());
    std::transform(map.begin(), map.end(), std::back_inserter(keys), [](const auto& kv) { return kv.first; });
    return keys;
}

template <size_t Dim, class State>
CartesianTopology<Dim, State>::CartesianTopology(const Vec<Dim>& size, const std::vector<State>& states, const std::array<bool, Dim>& periods, const std::map<State, float>& weights)
    : size(size)
{
    this->nodes = std::vector<Node<State>>(std::reduce(size.begin(), size.end(), 1, std::multiplies<size_t>()));
    this->weights = weights;

    for (size_t i = 0; i < this->nodes.size(); i++)
    {
        Vec<Dim> coords = this->getCoord(i);

        this->nodes[i].states = states;
        this->nodes[i].adjacent = std::vector<Node<State>*>(Dim * 2);
        for (size_t a = 0; a < Dim; a++)
        {
            Vec<Dim> coordsNegative = coords, coordsPositive = coords;
            coordsNegative[a] = coords[a] != 0 ? coords[a] - 1 : size[a] - 1;
            coordsPositive[a] = coords[a] != size[a] - 1 ? coords[a] + 1 : 0;
            this->nodes[i].adjacent[2 * a] = coords[a] != 0 || periods[a] ? &this->getNode(coordsNegative) : nullptr;
            this->nodes[i].adjacent[2 * a + 1] = coords[a] != size[a] - 1 || periods[a] ? &this->getNode(coordsPositive) : nullptr;
        }
    }

    this->compatible = [](const Node<State>&, const State&, const Node<State>&, const State&) { return true; };
}

template <size_t Dim, class State>
CartesianTopology<Dim, State>::CartesianTopology(const Vec<Dim>& size, const std::map<State, std::array<std::vector<State>, Dim * 2>>& adjacent, const std::array<bool, Dim>& periods, const std::map<State, float>& weights)
    : CartesianTopology(size, __getKeys(adjacent), periods, weights)
{
    this->compatible = [adjacent](const Node<State>& a, const State& aState, const Node<State>& b, const State& bState)
    {
        for (size_t i = 0; i < Dim * 2; i++)
        {
            size_t j = i ^ 1;
            if (a.adjacent[i] == &b && b.adjacent[j] == &a)
            {
                const std::vector<State>& availableA = adjacent.at(aState)[i];
                const std::vector<State>& availableB = adjacent.at(bState)[j];
                return std::find(availableA.begin(), availableA.end(), bState) != availableA.end() &&
                    std::find(availableB.begin(), availableB.end(), aState) != availableB.end();
            }
        }

        return false;
    };
}

template <size_t Dim, class State>
CartesianTopology<Dim, State>::CartesianTopology(const Vec<Dim>& size, const std::vector<State>& states, const std::array<std::function<bool(const State& a, const State& b)>, Dim>& rules, const std::array<bool, Dim>& periods, const std::map<State, float>& weights)
    : CartesianTopology(size, states, periods, weights)
{
    this->compatible = [rules](const Node<State>& a, const State& aState, const Node<State>& b, const State& bState)
    {
        for (size_t i = 0; i < Dim * 2; i++)
        {
            size_t j = i ^ 1;
            if (a.adjacent[i] == &b && b.adjacent[j] == &a)
            {
                return i & 1 ? rules[i / 2](aState, bState) : rules[i / 2](bState, aState);
            }
        }

        return false;
    };
}

template <size_t Dim, class State>
template <class Token>
CartesianTopology<Dim, State>::CartesianTopology(const Vec<Dim>& size, const std::map<State, std::array<std::vector<Token>, Dim * 2>>& tokens, const std::array<bool, Dim>& periods, const std::map<State, float>& weights)
    : CartesianTopology(size, __getKeys(tokens), periods, weights)
{
    this->compatible = [tokens](const Node<State>& a, const State& aState, const Node<State>& b, const State& bState)
    {
        for (size_t i = 0; i < Dim * 2; i++)
        {
            size_t j = i ^ 1;
            if (a.adjacent[i] == &b && b.adjacent[j] == &a)
            {
                const std::vector<Token>& availableA = tokens.at(aState)[i];
                const std::vector<Token>& availableB = tokens.at(bState)[j];
                return std::any_of(availableB.begin(), availableB.end(), [&availableA](const Token& b)
                {
                    return std::find(availableA.begin(), availableA.end(), b) != availableA.end();
                }) &&
                std::any_of(availableA.begin(), availableA.end(), [&availableB](const Token& a)
                {
                    return std::find(availableB.begin(), availableB.end(), a) != availableB.end();
                });
            }
        }

        return false;
    };
}

template <size_t Dim, class State>
Node<State>& CartesianTopology<Dim, State>::getNode(const Vec<Dim>& coord)
{
    return this->nodes[this->getIndex(coord)];
}

template <size_t Dim, class State>
const Node<State>& CartesianTopology<Dim, State>::getNode(const Vec<Dim>& coord) const
{
    return this->nodes[this->getIndex(coord)];
}

template <size_t Dim, class State>
size_t CartesianTopology<Dim, State>::getIndex(const Vec<Dim>& coord) const
{
    size_t accum = 1;
    return std::inner_product(
        coord.begin(),
        coord.end(),
        this->size.begin(),
        0,
        std::plus<size_t>(),
        [&accum](size_t coord_val, size_t size_val)
        {
            size_t result = coord_val * accum;
            accum *= size_val;
            return result;
        });
}

template <size_t Dim, class State>
Vec<Dim> CartesianTopology<Dim, State>::getCoord(size_t index) const
{
    Vec<Dim> coords;
    std::transform(
        this->size.begin(),
        this->size.end(),
        coords.begin(),
        [&index](size_t d)
        {
            size_t coord = index % d;
            index /= d;
            return coord;
        });

    return coords;
}

}
