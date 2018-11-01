#ifndef EDMONDS_HPP
#define EDMONDS_HPP

#include "graph.hpp"

namespace ED
{
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

        bool matching_edge(NodeId v, NodeId u) const;

        void scan_node(NodeId node);

        const Graph &_g;
        std::vector<NodeId> _phi;
        std::vector<NodeId> _mu;
        std::vector<NodeId> _rho;
        std::vector<bool> scanned;

    };

} // namespace ED

#endif //EDMONDS_HPP
