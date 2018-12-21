#ifndef PROG2_MWC_HPP
#define PROG2_MWC_HPP

#include "graph.hpp"
#include "tjoin.hpp"
#include <numeric>
#include <iostream>

namespace MMWC
{
class MinMeanWeightCycle
{
  public:
    static Graph get_min_mean_cycle(const Graph &g);

  private:
    explicit MinMeanWeightCycle(const Graph &g) : 
        _g(g),
        _adjustment(0),
        _multiplicator(1)
    { }

    std::vector<EdgeId> run();
    Graph populate(std::vector<EdgeId> edges) const;

    std::vector<EdgeId> get_circle(std::vector<char> included_edges) const;

    capacity adjusted_cap(EdgeId edge_id) const;

    capacity gcd(capacity a, capacity b) const;

    struct walked_edge
    {
        EdgeId edge = invalid_edge_id;
        NodeId walked_from = invalid_node_id;
    };

    const Graph &_g;
    capacity _adjustment;
    capacity _multiplicator;
};

// BEGIN INLINE SECTION

inline capacity MinMeanWeightCycle::adjusted_cap(MMWC::EdgeId edge_id) const
{
    return _g.get_capacity(edge_id) - _adjustment;
}

inline capacity MinMeanWeightCycle::gcd(capacity a, capacity b) const
{
    return b == 0 ? a : gcd(b, a % b);
}

// END INLINE SECTION
}   // namespace MMWC

#endif   // PROG2_MWC_HPP
