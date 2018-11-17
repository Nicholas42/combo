#ifndef EDMONDS_HPP
#define EDMONDS_HPP

#include "graph.hpp"
#include <cassert>
#include <iostream>


/**
 * @file edmonds.hpp
 *
 * @brief This file provides a class implementing Edmond's Maximum Cardinality Matching Algorithm. Any notation is
 * according to Combinatorial Optimization by Korte and Vygen.
 */
namespace ED
{
    using Path = std::vector<NodeId>;

    /**
     * We will partition the nodes into these three types.
     */
    enum class NodeType
    {
        outer,
        inner,
        out_of_forrest
    };

    /**
     * @class EdmondsMatching
     *
     * @brief This class implements the algorithm. For a given @c Graph g, we find a maximum matching in O(n^3) time,
     * where n is the number of nodes in g.
     */
    class EdmondsMatching
    {

    public:

        /**
         * Runs the algorithm on the given graph.
         * @param g @c Graph in which to find the maximum matching.
         * @return A @c Graph on the same set of \c Nodes containing exactly the edges of a maximum matching.
         */
        static Graph get_matching(const Graph &g);

    private:

        /**
         * Initializes the data for the given @c Graph @c g.
         */
        explicit EdmondsMatching(const Graph &g) :
                _g(g),
                _phi(_g.num_nodes()),
                _mu(_g.num_nodes()),
                _rho(_g.num_nodes()),
                scanned(_g.num_nodes(), false)
        {
            for (NodeId v = 0; v < _g.num_nodes(); ++v)
            {
                _phi[v] = v;
                _mu[v] = v;
                _rho[v] = v;
            }
        }

        /**
         * @return The current state of the matching.
         */
        Graph populate() const;

        /**
         * @return The type of the @c Node with @c NodeId @c n.
         */
        NodeType get_type(NodeId node) const;

        /**
         * @return True, if the edge between @c v and @c u is in the special blossom forest, false otherwise.
         */
        bool forest_edge(NodeId v, NodeId u) const;

        /**
         * @return The M-alternating path from @c v to the root of its connected component.
         */
        Path get_path(NodeId v) const;

        /**
         * @return The first node which lies on @c x_path and @c y_path.
         */
        NodeId get_intersection(Path x_path, Path y_path) const;

        /**
         * Grows the blossom forest, augments the matching or shrinks blossoms.
         * @param node The node from which these operations are executed.
         */
        void scan_node(NodeId node);

        /**
         * Augments along the path from the root of the connected component of @c x through @c x and @c y to the root of
         * the connected component of @c y.
         * @param x_path, y_path The paths starting in @c x and @c y to the roots of their respective connected
         * components, need to be vertex disjoint.
         */
        void augment(NodeId x, NodeId y, Path x_path, Path y_path);

        /**
         * Shrinks the blossom to which @c x, @c y and the @c intersection of the paths starting in @c x and @c y belong.
         * @param intersection The first node lying on both @c x_path and @c y_path.
         * @param x_path, y_path The paths starting in @c x and @c y to the roots of their respective connected
         * components.
         */
        void shrink(NodeId x, NodeId y, NodeId intersection, Path x_path, Path y_path);

        /**
         * Starts the algorithm. Iterates over all nodes and scans them until no further improvement can be done.
         * @warning The object has to be in a valid state (so encoding a valid matching and blossom forest) when this is
         * called. This is the case if it is called directly after calling the constructor.
         */
        void run();

        /**
         * Helper method to call @c f on every second node of @c p.
         * @fparam func The type of @c f. Should be a function taking a @c NodeId and returning a true value while the
         * execution should go. Method will stop when @c f returns a false value.
         * @param stop The method will stop when this @c Node is reached (even if it has even distance from the start).
         */
        template <typename func>
        void do_on_odd(Path &p, func f, NodeId stop);

        const Graph &_g;
        std::vector<NodeId> _phi;
        std::vector<NodeId> _mu;
        std::vector<NodeId> _rho;
        std::vector<int> scanned;

    };

    // BEGIN: Inline section

    inline
    NodeType EdmondsMatching::get_type(NodeId node) const
    {
        NodeId mu = _mu[node];
        if (mu == node || _phi[mu] != mu)
        {
            return NodeType::outer;
        }
        if (_phi[node] == node)
        {
            return NodeType::out_of_forrest;
        }

        return NodeType::inner;
    }

    // END: Inline section
} // namespace ED

#endif // EDMONDS_HPP
