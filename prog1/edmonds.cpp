#include "edmonds.hpp"

namespace ED
{

  bool EdmondsMatching::check() const
  {
    for(size_t i = 0; i < _g.num_nodes(); ++i)
      {
	if(_phi[i] == _mu[i] && _mu[i] != i)
	  {
	    return false;
	  }
      }
    return true;
  }
Graph EdmondsMatching::get_matching(const Graph &g)
{
    Graph ret(g.num_nodes());

    EdmondsMatching match(g);
    match.run();
    match.populate(ret);

    for(size_t i = 0; i < ret.num_nodes(); ++i)
    {
	    if(ret.node(i).degree() > 1)
	    {
		    throw std::runtime_error("No matching.");
	    }
    }

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

bool EdmondsMatching::forest_edge(NodeId v, NodeId u) const
{
    return (u == _mu[v] || v == _mu[u] || u == _phi[v]);
}

bool EdmondsMatching::matching_edge(NodeId v, NodeId u) const
{
    return (u == _phi[v]);
}

Path EdmondsMatching::get_path(NodeId v) const
{
    assert(get_type(v) == NodeType::outer);

    std::cout << "start" << std::endl;

    Path path;
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
        use_mu = !use_mu;
        if(path.size() > _g.num_nodes())
        {
            for(auto i: path)
            {
                std::cout << i << ",";
            }
            std::cout << std::endl;
            throw std::runtime_error("M");
        }
    } while (cur != last);

    std::cout << "end" << std::endl;

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
    std::cout << "Augment " << x << " to " << y << std::endl;
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
    NodeId root = invalid_node_id;
    for (auto path_node : get_path(intersection))
    {
        if (_rho[path_node] == path_node)
        {
            root = path_node;
	    break;
        }
    }
    assert(root != invalid_node_id);
    std::cout << "Shrinking with root " << root << std::endl;

    for (size_t i = 0; i < x_path.size() && i != root; i++)
    {
        if (i % 2 == 0 || _rho[_phi[i]] == root)
        {
            continue;
        }

        _phi[_phi[i]] = i;
    }
    for (size_t i = 0; i < y_path.size() && i != root; i++)
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

    if(!check())
    {
	std::cout << _rho[x_path[0]] << " " << _rho[y_path[0]] << std::endl;
	for(auto i: x_path)
	{
		std::cout << i << " ";
	}
	std::cout << std::endl;
	for(auto i: y_path)
	{
		std::cout << i << " ";
	}
	std::cout << std::endl;
	assert(false);
    }
    std::vector<bool> on_path(_g.num_nodes(), false);
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
	    if(i == root)
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
  assert(check());
}

void EdmondsMatching::scan_node(NodeId node)
{
  assert(check());
    const Node &n = _g.node(node);
    for (auto neighbor : n.neighbors())
    {
        NodeType type = get_type(neighbor);

        std::cout << "Looking at " << node << " " << neighbor << std::endl;
	/*
        std::cout << "Values are:" << std::endl;
        for (int i = 0; i < _g.num_nodes(); i++)
        {
            std::cout << i << "\t" << _mu[i] << "\t" << _phi[i] << "\t" << _rho[i] << std::endl;
	    }*/
        std::cout << "Type of neigbor is " << (int)type << std::endl;

        if (type == NodeType::out_of_forrest)
        {
            // Grow step
            std::cout << "Grow by " << neighbor << " at " << node << std::endl;
            _phi[neighbor] = node;
            continue;
        }
        if (!(type == NodeType::outer && _rho[node] != _rho[neighbor]))
        {
            continue;
        }

        std::cout << "getting paths\n";

        auto x_path = get_path(node);
        auto y_path = get_path(neighbor);

        std::cout << "gotten paths\n";
        auto intersection = get_intersection(x_path, y_path);
        std::cout << "gotten intersection\n";

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
        std::cout << "Scanning node " << x << std::endl;
        scan_node(x);
    }

    std::cout << "Values are:" << std::endl;
    for (int i = 0; i < _g.num_nodes(); i++)
    {
        std::cout << i << "\t" << _mu[i] << "\t" << _phi[i] << "\t" << _rho[i] << std::endl;
    }
}

} // namespace ED
