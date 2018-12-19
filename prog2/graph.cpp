#include "graph.hpp"   // always include corresponding header first

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace MMWC
{
/////////////////////////////////////////////
//! \c Node definitions
/////////////////////////////////////////////

void Node::add_neighbor(EdgeId edge_id)
{
    _incident_edges.push_back(edge_id);
}

/////////////////////////////////////////////
//! \c Graph definitions
/////////////////////////////////////////////

Graph Graph::build_graph(const std::string &filename)
{
    std::ifstream ifs(filename);
    if (!ifs.is_open())
    {
        throw std::runtime_error("Could not open input file.");
    }

    std::string line;

    do
    {
        if (!std::getline(ifs, line))
        {
            throw std::runtime_error("Could not find problem line in DIMACS stream.");
        }
    } while (line[0] == 'c');

    int num_nodes = 0;
    int num_edges = 0;

    if (line[0] == 'p')
    {
        std::stringstream stream;
        stream << line;
        std::string str;
        stream >> str >> str >> num_nodes >> num_edges;
    }
    else
    {
        throw std::runtime_error("Unexpected format of input file.");
    }

    Graph graph(num_nodes);
    while (std::getline(ifs, line))
    {
        if (line.empty() or line[0] != 'e')
        {
            continue;
        }
        std::stringstream stream;
        stream << line;
        char c;
        DimacsId i;
        DimacsId j;
        capacity cap;
        stream >> c >> i >> j >> cap;
        graph.add_edge(from_dimacs_id(i), from_dimacs_id(j), cap);
    }

    return graph;
}

Graph::Graph(NodeId const num_nodes) : _nodes(num_nodes), _edges() {}

void Graph::add_edge(NodeId node1_id, NodeId node2_id, capacity cap)
{
    if (node1_id == node2_id)
    {
        throw std::runtime_error("ED::Graph class does not support loops!");
    }

    _edges.push_back({node1_id, node2_id, cap});

    const EdgeId edge_id = _edges.size() - 1;
    _nodes.at(node1_id).add_neighbor(edge_id);
    _nodes.at(node2_id).add_neighbor(edge_id);
}

std::ostream &operator<<(std::ostream &str, Graph const &graph)
{
    str << "c This encodes a graph in DIMACS format\n"
        << "p edge " << graph.num_nodes() << " " << graph.num_edges() << "\n";

    for (const auto &edge: graph._edges)
    {
        str << "e " << to_dimacs_id(edge.from) << " " << to_dimacs_id(edge.to) << " "
            << edge.cap << "\n";
    }

    str << std::flush;
    return str;
}

/////////////////////////////////////////////
//! global functions
/////////////////////////////////////////////

NodeId from_dimacs_id(DimacsId const dimacs_id)
{
    if (dimacs_id == 0)
    {
        throw std::runtime_error("Invalid (0) DIMACS id.");
    }

    return static_cast<NodeId>(dimacs_id - 1);
}

DimacsId to_dimacs_id(NodeId const node_id)
{
    if (node_id == std::numeric_limits<NodeId>::max())
    {
        throw std::runtime_error("Invalid (inf) node id.");
    }

    return static_cast<DimacsId>(node_id + 1);
}

}   // namespace MMWC
