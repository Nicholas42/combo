#include "edmonds.hpp"

namespace ED
{
    Graph EdmondsMatching::get_matching(const Graph &g)
    {
        Graph ret(g.num_nodes());

        EdmondsMatching match(g);

        match.populate(ret);

        return ret;
    }

    void EdmondsMatching::populate(Graph &g) const
    {
        for (NodeId v = 0; v < g.num_nodes(); ++v)
        {
            NodeId other = _phi[v];

            if (other < v)
            {
                g.add_edge(v, other);
            }
        }
    }

    NodeType EdmondsMatching::get_type(NodeId node) const
    {
        NodeId mu = _mu[node];
        if (mu == node || _phi[mu] == mu)
        {
            return NodeType::outer;
        }
        if (_phi[node] == node)
        {
            return NodeType::out_of_forrest;
        }

        return NodeType::inner;
    }

    bool EdmondsMatching::forest_edge(NodeId v, NodeId u) const
    {
        return (u == _mu[v] || v == _mu[u] || u == _phi[v]);
    }

    bool EdmondsMatching::matching_edge(NodeId v, NodeId u) const
    {
        return (u == _phi[v]);
    }

    void EdmondsMatching::scan_node(NodeId node)
    {
        const Node &n = _g.node(node);
        for (auto neighbor : n.neighbors())
        {
            NodeType type = get_type(neighbor);
            if (type == NodeType::out_of_forrest)
            {
                // Grow step
                _phi[neighbor] = node;
                continue;
            }
            if (!(type == NodeType::outer && _rho[node] != _rho[neighbor]))
            {
                continue;
            }

            //TODO: augment and shrink
        }
    }

} // namespace ED
