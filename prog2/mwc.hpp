#ifndef PROG2_MWC_HPP
#define PROG2_MWC_HPP

#include "graph.hpp"
#include "tjoin.hpp"

namespace MMWC
{
class MinMeanWeightCycle
{
  public:
    static Graph mean_weight_cycle(const Graph &g);

  private:
    MinMeanWeightCycle(const Graph &g) : _g(g), _adjustment(min_capacity)
    {
        for (EdgeId edge_id = 0; edge_id < g.num_edges(); ++edge_id)
        {
            _adjustment = std::min(_adjustment, g.get_capacity(edge_id));
        }
    }

    bool has_circle() const;

    capacity adjusted_cap(EdgeId edge_id) const;

    const Graph &_g;
    capacity _adjustment;
};

// BEGIN INLINE SECTION

inline capacity MinMeanWeightCycle::adjusted_cap(MMWC::EdgeId edge_id) const
{
    return _g.get_capacity(edge_id) - _adjustment;
}

// END INLINE SECTION
}   // namespace MMWC

#endif   // PROG2_MWC_HPP
