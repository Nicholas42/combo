#include "edmonds.hpp"

namespace ED
{

Graph EdmondsMatching::get_matching(const Graph &g)
{

    EdmondsMatching match(g);
    match.run();
    return match.populate();
}

Graph EdmondsMatching::populate() const
{
    Graph g (_g.num_nodes());
    for (NodeId v = 0; v < g.num_nodes(); ++v)
    {
        NodeId other = _mu[v];

        if (other < v)
        {
            g.add_edge(v, other);
        }
    }
    return g;
}

bool EdmondsMatching::forest_edge(NodeId v, NodeId u) const
{
    return (u == _mu[v] || v == _phi[u] || u == _phi[v]);
}


Path EdmondsMatching::get_path(NodeId v) const
{
    assert(get_type(v) == NodeType::outer);

        Path path;
        NodeId cur = v, last;
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
        std::vector<int> visited(_g.num_nodes(), false);
        std::vector<NodeId> stack;
        stack.push_back(x);
        while(!stack.empty())
        {
            NodeId cur = stack.back();
            stack.pop_back();
            visited[cur] = true;

            for(auto v: _g.node(cur).neighbors())
            {
                if(!visited[v] && forest_edge(cur, v))
                {

                    stack.push_back(v);
                }
            }
        }

        auto func = [this] (NodeId n) {
            _mu[_phi[n]] = n;
            _mu[n] = _phi[n];
            return true;
        };

        do_on_odd(x_path, func, invalid_node_id);
        do_on_odd(y_path, func, invalid_node_id);

        _mu[x] = y;
        _mu[y] = x;

        for(NodeId i = 0; i < _g.num_nodes(); ++i)
        {
            if(visited[i])
            {
                _phi[i] = i;
                _rho[i] = i;
                scanned[i] = false;
            }
        }
    }

void EdmondsMatching::shrink(NodeId x, NodeId y, NodeId intersection, Path x_path, Path y_path)
{
    assert(intersection != invalid_node_id);

    NodeId root = _rho[intersection];
    assert(get_type(root) == NodeType::outer);

        auto func = [this, root](NodeId n)
        {
            if(_rho[_phi[n]] == root)
            {
                return true;
            }
            if (n == root)
            {
                return false;
            }
            _phi[_phi[n]] = n;
            return true;
        };

        do_on_odd(x_path, func, root);
        do_on_odd(y_path, func, intersection);

    if (_rho[x] != root)
    {
        _phi[x] = y;
    }
    if (_rho[y] != root)
    {
        _phi[y] = x;
    }

        std::vector<int> on_path(_g.num_nodes(), false);
        auto mark_path_nodes = [&on_path, this] (Path &p, NodeId abort)
        {
        for (auto i: p)
        {
            if(i == abort)
            {
                return;
            }
            on_path[i] = true;
        }
        };

        mark_path_nodes(x_path, root);
        mark_path_nodes(y_path, intersection);

        for (NodeId i = 0; i < _g.num_nodes(); ++i)
        {
            if (on_path[_rho[i]])
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
                if (get_type(i) == NodeType::outer && !scanned[i])
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

    template <typename func>
    void EdmondsMatching::do_on_odd(Path &p, func f, NodeId stop)
    {
        bool go_on = true;
        for (size_t i = 1; i < p.size() && go_on; i += 2)
        {
            if(p[i-1] == stop || p[i] == stop)
            {
                break;
            }
            go_on = f(p[i]);
        }

}

} // namespace ED
