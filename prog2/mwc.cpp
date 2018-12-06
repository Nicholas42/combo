#include "mwc.hpp"

namespace MIN_MEAN_WEIGHT_CYCLE
{
bool MinMeanWeightCycle::has_circle() const
{
    std::vector<NodeId> parent(_g.num_nodes(), invalid_node_id);
    std::vector<NodeId> stack;

    for (NodeId node_id = 0; node_id < _g.num_nodes(); ++node_id)
    {
        if (parent[node_id] == invalid_node_id)
        {
            continue;
        }

        stack.push_back(node_id);

        while (!stack.empty())
        {
            NodeId cur_id = stack.back();
            stack.pop_back();
            NodeId cur_parent = parent[cur_id];

            for(EdgeId edge_id: _g.node(cur_id).incident_edges())
            {
                NodeId other_id = _g.get_other_node(edge_id, cur_id);

                if(other_id == cur_parent)
                {
                    continue;
                }

                if(parent[other_id] != invalid_node_id)
                {
                    return true;
                }

                parent[other_id] = cur_id;
                stack.push_back(other_id);
            }
        }
    }

    return false;
}
}   // namespace MIN_MEAN_WEIGHT_CYCLE
