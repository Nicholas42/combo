#ifndef PROG2_TJOIN_HPP
#define PROG2_TJOIN_HPP

#include "graph.hpp"

namespace MIN_MEAN_WEIGHT_CYCLE
{
class TJoin
{
  public:

    TJoin (Graph &g)
    {}

    capacity get_capacity() const;

    EdgeId get_num_edges() const;
};
}   // namespace MIN_MEAN_WEIGHT_CYCLE

#endif   // PROG2_TJOIN_HPP
