#include "edmonds.hpp"

namespace ED
{

Graph EdmondsMatching::get_matching(const Graph &g)
{
    Graph ret(g.num_nodes());

    EdmondsMatching match(g);
    match.run();
    match.populate(ret);

        return ret;
    }

void EdmondsMatching::populate(Graph &g) const
{
    for (NodeId v = 0; v < g.num_nodes(); ++v)
    {
        NodeId other = _mu[v];

        if (other < v)
        {
            g.add_edge(v, other);
        }
    }
}

bool EdmondsMatching::forest_edge(NodeId v, NodeId u) const
{
    return (u == _mu[v] || v == _phi[u] || u == _phi[v]);
}


Path EdmondsMatching::get_path(NodeId v) const
{
    assert(get_type(v) == NodeType::outer);

    Path path;
    NodeId cur = v, last = invalid_node_id;
    bool use_mu = true;
    do
    {
        assert(path.size() <= _g.num_nodes());
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
        use_mu = !use_mu;
        } while (cur != last);

    return path;
}

NodeId EdmondsMatching::get_intersection(Path x_path, Path y_path) const
{
    std::vector<bool> visited(_g.num_nodes(), false);
    for (auto node : x_path)
    {
        visited[node] = true;
    }
    for (auto node : y_path)
    {
        if (visited[node])
        {
            return node;
        }
    }

    return invalid_node_id;
}

void EdmondsMatching::augment(NodeId x, NodeId y, Path x_path, Path y_path)
{
    for (size_t i = 0; i < x_path.size(); i++)
    {
        if (i % 2 == 0)
            continue;

        NodeId n = x_path[i];
        _mu[_phi[n]] = n;
        _mu[n] = _phi[n];
    }
    for (size_t i = 0; i < y_path.size(); i++)
    {
        if (i % 2 == 0)
            continue;

        NodeId n = y_path[i];
        _mu[_phi[n]] = n;
        _mu[n] = _phi[n];
    }
    _mu[x] = y;
    _mu[y] = x;
    //TODO: Maybe restrict to connected component
    for (NodeId v = 0; v < _g.num_nodes(); ++v)
    {
        _phi[v] = v;
        _rho[v] = v;
        scanned[v] = false;
    }
}

void EdmondsMatching::shrink(NodeId x, NodeId y, NodeId intersection, Path x_path, Path y_path)
{
    assert(intersection != invalid_node_id);

    NodeId root = _rho[intersection];
    assert(get_type(root) == NodeType::outer);

    for (size_t i = 0; i < x_path.size() && x_path[i] != root; i++)
    {
        if (i % 2 == 0 || _rho[_phi[i]] == root)
        {
            continue;
        }

        _phi[_phi[i]] = i;
    }
    for (size_t i = 0; i < y_path.size() && y_path[i] != intersection; i++)
    {
        if (i % 2 == 0 || _rho[_phi[i]] == root)
        {
            continue;
        }

        _phi[_phi[i]] = i;
    }

    if (_rho[x] != root)
    {
        _phi[x] = y;
    }
    if (_rho[y] != root)
    {
        _phi[y] = x;
    }

    std::vector<int> on_path(_g.num_nodes(), false);
    for(auto i: x_path)
    {
	    on_path[i] = true;
	    if(i == root)
	    {
		    break;
	    }
    }
    for(auto i: y_path)
    {
	    on_path[i] = true;
	    if(i == intersection)
	    {
		    break;
	    }
    }
    for (size_t i = 0; i < _g.num_nodes(); ++i)
    {
	if(on_path[_rho[i]])
	{
		_rho[i] = root;
	}
    }
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
            augment(node, neighbor, x_path, y_path);
            

            return;
        }

        else
        {
            shrink(node, neighbor, intersection, x_path, y_path);
            

        }
    }
    scanned[node] = true;
}

void EdmondsMatching::run()
{
    for (NodeId i = 0; i < _g.num_nodes(); i++)
    {
        _mu[i] = _phi[i] = _rho[i] = i;
    }

    while (true)
    {
        auto x = invalid_node_id;

        for (NodeId i = 0; i < _g.num_nodes(); i++)
        {
            NodeType type = get_type(i);
            if (type == NodeType::outer && !scanned[i])
            {
                x = i;
                break;
            }
        }

        if (x == invalid_node_id)
        {
            break;
        }
        scan_node(x);
    }

}

} // namespace ED
