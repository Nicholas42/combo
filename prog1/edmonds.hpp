#ifndef EDMONDS_HPP
#define EDMONDS_HPP

#include "graph.hpp"
#include <cassert>
#include <iostream>

namespace ED
{
    using Path = std::vector<NodeId>;
    enum class NodeType
    {
        outer,
        inner,
        out_of_forrest
    };

    class EdmondsMatching
    {

    public:
        static Graph get_matching(const Graph &g);

    private:

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

        void populate(Graph &g) const;

        NodeType get_type(NodeId node) const;
        
        bool forest_edge(NodeId v, NodeId u) const;
        
        Path get_path(NodeId v) const;

        NodeId get_intersection(Path x_path, Path y_path) const;

        void scan_node(NodeId node);

        void augment(NodeId x, NodeId y, Path x_path, Path y_path);

        void shrink(NodeId x, NodeId y, NodeId intersection, Path x_path, Path y_path);

        void run();

        const Graph &_g;
        std::vector<NodeId> _phi;
        std::vector<NodeId> _mu;
        std::vector<NodeId> _rho;
        std::vector<bool> scanned;

    };

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

} // namespace ED

#endif //EDMONDS_HPP
