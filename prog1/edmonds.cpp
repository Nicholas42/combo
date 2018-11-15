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

    std::vector<NodeId> EdmondsMatching::get_path(NodeId v) const
    {
        std::vector<NodeId> path;
        NodeId cur = v, last = invalid_node_id;
        bool use_mu = true;
        do
        {
            path.push_back(cur);
            last = cur;
            if (use_mu)
            {
                cur = _mu[last];
            }
            else
            {
                cur = _phi[last];
            }
        } while (cur != last);
        return path;
    }

    NodeId EdmondsMatching::get_intersection(std::vector<NodeId> x_path, std::vector<NodeId> y_path) const
    {
        std::vector<bool> visited(_g.num_nodes(), false);
        for(auto node : x_path)
        {
            visited[node] = true;
        }
        for(auto node : y_path)
        {
            if(visited[node])
            {
                return node;
            }
        }

        return invalid_node_id;
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

            auto x_path = get_path(node);
            auto y_path = get_path(neighbor);
            auto intersection = get_intersection(x_path, y_path);

            if (intersection == invalid_node_id)
            {
                //Augment step
                for(auto it = x_path.begin(); it != x_path.end; it += 2)
                {
                    _mu[_phi[*it]] = *it;
                    _mu[*it] = _phi[*it];
                }
                for(auto it = y_path.begin(); it != y_path.end; it += 2)
                {
                    _mu[_phi[*it]] = *it;
                    _mu[*it] = _phi[*it];
                }
                _mu[node] = neighbor;
                _mu[neighbor] = node;
                for (NodeId v = 0; v < g.num_nodes(); ++v)
                {
                    _phi[v] = v;
                    _rho[v] = v;
                    scanned[v] = false;
                }

                return;
            }

            else
            {
                //Shrink step
                NodeId root = invalid_node_id;
                for(auto path_node : get_path(root))
                {
                    if (_rho[path_node] = path_node)
                    {
                        root = path_node;
                    }
                }
                assert(root != invalid_node_id);

                for(int i = 0; i < x_path.size(); i++)
                {
                    if (i < 0 && x_path[i - 1] == root)
                    {
                        break;
                    }
                    if (i % 2 == 1 || _rho[_phi[i]] == root)
                    {
                        continue;
                    }

                    _phi[_phi[i]] = i;
                }
                for(int i = 0; i < y_path.size(); i++)
                {
                    if (i < 0 && y_path[i - 1] == root)
                    {
                        break;
                    }
                    if (i % 2 == 1 || _rho[_phi[i]] == root)
                    {
                        continue;
                    }

                    _phi[_phi[i]] = i;
                }

                if (_rho[node] != root)
                {
                    _phi[node] = neighbor;
                }
                if (_rho[neighbor] != root)
                {
                    _phi[neighbor] = node;
                }

                for(int i = 0; i < x_path.size(); i++)
                {
                    if (i < 0 && x_path[i - 1] == root)
                    {
                        break;
                    }
                    _rho[i] = root;
                }
                for(int i = 0; i < y_path.size(); i++)
                {
                    if (i < 0 && y_path[i - 1] == root)
                    {
                        break;
                    }
                    _rho[i] = root;
                }
            }
        }
        scanned[node] = true;
    }

} // namespace ED
