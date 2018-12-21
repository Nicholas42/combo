#include "mwc.hpp"

namespace MMWC
{
std::vector<EdgeId> MinMeanWeightCycle::get_circle(std::vector<char> included_edges) const
{
    std::vector<walked_edge> parent(_g.num_nodes());
    std::vector<NodeId> stack;

    for (NodeId node_id = 0; node_id < _g.num_nodes(); ++node_id)
    {
        if (parent[node_id].walked_from == invalid_node_id)
        {
            continue;
        }

        stack.push_back(node_id);

        while (!stack.empty())
        {
            NodeId cur_id = stack.back();
            stack.pop_back();
            NodeId cur_parent = parent[cur_id].walked_from;

            for (EdgeId edge_id : _g.node(cur_id).incident_edges())
            {
                if(!included_edges[edge_id])
                {
                    continue;
                }

                NodeId other_id = _g.get_other_node(edge_id, cur_id);

                if (other_id == cur_parent)
                {
                    continue;
                }

                if (parent[other_id].walked_from != invalid_node_id)
                {
                    std::vector<EdgeId> circle;
                    circle.push_back(edge_id);
                    auto cur_node = node_id;

                    while(cur_id != other_id)
                    {
                        circle.push_back(parent[cur_node].edge);
                        cur_node = parent[cur_node].walked_from;
                    }

                    return circle;
                }

                walked_edge edge;
                edge.edge = edge_id;
                edge.walked_from = cur_id;
                parent[other_id] = edge;
                stack.push_back(other_id);
            }
        }
    }

    return std::vector<EdgeId>();
}

Graph MinMeanWeightCycle::get_min_mean_cycle(const Graph &g)
{
    MinMeanWeightCycle cycle(g);
    auto edges = cycle.run();
    return cycle.populate(edges);
}

std::vector<EdgeId> MinMeanWeightCycle::run()
{
    if (get_circle(std::vector<char>(_g.num_edges(), 1)).size() == 0)
    {
        return std::vector<EdgeId>();
    }

    capacity lambda = min_capacity;
    for (auto edge : _g.get_edges())
    {
        if (edge.cap > lambda)
        {
            lambda = edge.cap;
        }
    }

    _adjustment -= lambda;
    capacity tjoin_weight = 0;
    std::vector<char> last_edges;
    do
    {
        TJoin join(_g, _adjustment, _multiplicator);
        tjoin_weight = join.get_capacity();
        if (tjoin_weight != 0)
        {
            last_edges = join.get_used_edges();
        }
        auto divisor = gcd(std::abs(tjoin_weight), join.get_num_edges());
        _adjustment -= tjoin_weight / divisor;
        _multiplicator *= join.get_num_edges() / divisor;
    }
    while (tjoin_weight != 0);

    return get_circle(last_edges);
}

Graph MinMeanWeightCycle::populate(std::vector<EdgeId> edges) const
{
    std::vector<size_t> deg(_g.num_nodes(), 0);

    Graph g(_g.num_nodes());
    for(auto edge_id : edges)
    {
        auto edge = _g.get_edges()[edge_id];
        g.add_edge(edge.from, edge.to, edge.cap);
        deg[edge.from]++; deg[edge.to]++;
    }

    for (auto i : deg)
    {
        assert(i % 2 == 0);
    }

    return g;
}

} // namespace MMWC
