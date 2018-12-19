#include <iostream>
#include <set>
#include <functional>
#include "tjoin.hpp"

namespace MMWC
{
std::vector<TJoin::metric_closure_member>
    TJoin::compute_shortest_paths(NodeId source) const
{
    std::vector<int> visited(_g.num_nodes(), false);
    std::vector<metric_closure_member> ret(_g.num_nodes(),
                                           {invalid_node_id, max_capacity});

    /*
    auto fncmp = [&ret](NodeId first, NodeId second) { return ret[first].dist <
    ret[second].dist; };

    std::set<NodeId, std::function<bool(NodeId, NodeId)>> blah(fncmp);
     */

    NodeId cur = source;
    capacity dist = 0;
    ret[source] = {invalid_edge_id, 0};
    // blah.insert(source);

    do
    {
        visited[cur] = true;
        for (const EdgeId e: _g.node(cur).incident_edges())
        {
            NodeId other = _g.get_other_node(e, cur);
            if (dist + get_cap(e) < ret[other].dist)
            {
                assert(!visited[other]);
                //blah.erase(other);
                ret[other].dist = dist + get_cap(e);
                //blah.insert(other);
                ret[other].incoming = e;
            }
        }
        cur = invalid_node_id;
        dist = max_capacity;
        /*
        if (!blah.empty())
        {
            cur = *blah.begin();
            continue;
        }
        else
        {
            break;
        }
         */
        for (NodeId i = 0; i < _g.num_nodes(); ++i)
        {
            if (!visited[i] && dist > ret[i].dist)
            {
                assert(ret[i].incoming != invalid_edge_id);
                cur = i;
                dist = ret[i].dist;
            }
        }
    } while (cur != invalid_node_id);

    return ret;
}

void TJoin::all_pairs_shortest_paths()
{
    for (NodeId n = 0; n < _g.num_nodes(); ++n)
    {
        if (odd_neg_deg(n))
        {
            _odd_nodes.push_back(n);
            _shortest_paths.push_back(std::move(compute_shortest_paths(n)));
        }
    }
}

PerfectMatching TJoin::init_perfect_matching()
{
    int T_size = static_cast<int>(_odd_nodes.size());

    PerfectMatching pm(T_size, T_size * T_size / 2);

    for (int i = 0; i < T_size; ++i)
    {
        for (int j = i + 1; j < T_size; ++j)
        {
            pm.AddEdge(i, j, _shortest_paths[i][_odd_nodes[j]].dist);
        }
    }

    return pm;
}

void TJoin::collect_used_edges(PerfectMatching &pm)
{
    int T_size = static_cast<int>(_odd_nodes.size());
    std::vector<int> t_join(_g.num_edges(), false);

    for (int i = 0; i < T_size; ++i)
    {
        size_t vis = 0;
        NodeId j = _odd_nodes[pm.GetMatch(i)];
        if (_odd_nodes[i] > j)
        {
            continue;
        }
        while (j != _odd_nodes[i])
        {
            assert(vis < _g.num_nodes());
            vis++;
            EdgeId cur_edge = _shortest_paths[i][j].incoming;
            t_join[cur_edge] = !t_join[cur_edge];
            j = _g.get_other_node(cur_edge, j);
        }
    }

    for (EdgeId e = 0; e < _g.num_edges(); ++e)
    {
        if (t_join[e] != (_g.get_capacity(e) < 0))
        {
            _used_edges.push_back(e);
        }
    }
}

bool TJoin::odd_neg_deg(NodeId v) const
{
    size_t num_neg = 0;
    for (auto id: _g.node(v).incident_edges())
    {
        if (_g.get_capacity(id) < 0)
        {
            ++num_neg;
        }
    }
    return (num_neg % 2 == 1);
}

void TJoin::asp()
{
    std::vector<std::vector<metric_closure_member>> metric_closure(
        _g.num_nodes(), {_g.num_nodes(), {invalid_edge_id, max_capacity}});
    for (EdgeId e = 0; e < _g.num_edges(); ++e)
    {
        const Edge &ed = _g.get_edges()[e];
        capacity dist = get_cap(e);
        if (dist < 0)
        {
            throw std::runtime_error("WTF");
        }
        metric_closure[ed.from][ed.to] = {e, dist};
        metric_closure[ed.to][ed.from] = {e, dist};
    }

    for (NodeId n = 0; n < _g.num_nodes(); ++n)
    {
        if (odd_neg_deg(n))
        {
            _odd_nodes.push_back(n);
        }
        for (NodeId m = 0; m < _g.num_nodes(); ++m)
        {
            for (NodeId l = 0; l < _g.num_nodes(); ++l)
            {
                if (metric_closure[m][n].dist < max_capacity &&
                    metric_closure[n][l].dist < max_capacity &&
                    metric_closure[m][l].dist >
                        metric_closure[m][n].dist + metric_closure[n][l].dist)
                {
                    if (metric_closure[m][l].dist < 0)
                    {
                        throw std::runtime_error("GAH");
                    }
                    metric_closure[m][l].dist =
                        metric_closure[m][n].dist + metric_closure[n][l].dist;
                    metric_closure[m][l].incoming = metric_closure[n][l].incoming;
                }
            }
        }
    }

    for (auto n: _odd_nodes)
    {
        _shortest_paths.push_back(std::move(metric_closure[n]));
    }
}
}   // namespace MMWC
