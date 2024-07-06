/**
 * @file Node.h
 * @brief Node class for the topology.
 * 
 * A node is a container for states and the adjacent nodes.
 */

#pragma once

#include <vector>

/**
 * @brief Node class for the topology.
 * 
 * A node is a container for states and the adjacent nodes.
 * 
 * @tparam T The type of the states.
 */
template <class T>
struct Node
{
    std::vector<T> states;
    std::vector<Node*> adjacent;
};
