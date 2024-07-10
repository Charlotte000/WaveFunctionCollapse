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
