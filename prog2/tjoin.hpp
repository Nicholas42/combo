#ifndef PROG2_TJOIN_HPP
#define PROG2_TJOIN_HPP

#include "graph.hpp"
#include "PerfectMatching.h"

namespace MMWC
{
class TJoin
{
  public:
    TJoin(const Graph &g, capacity offset) : _g(g), _offset(offset)
    {
        //all_pairs_shortest_paths();
        asp();
        PerfectMatching pm = init_perfect_matching();
        pm.Solve();
        collect_used_edges(pm);
    }

    capacity get_capacity() const;

    EdgeId get_num_edges() const;

    const std::vector<EdgeId> &get_used_edges() const;

  private:
    void asp();

    capacity get_cap(EdgeId e) const;

    PerfectMatching init_perfect_matching();

    void all_pairs_shortest_paths();

    void collect_used_edges(PerfectMatching &pm);

    struct metric_closure_member
    {
        EdgeId incoming;
        capacity dist;

        struct cmp
        {
            inline bool operator()(const metric_closure_member &first,
                                   const metric_closure_member &second)
            {
                return first.dist < second.dist;
            }
        };
    };

    bool odd_neg_deg(NodeId v) const;

    std::vector<metric_closure_member> compute_shortest_paths(NodeId source) const;

    const Graph &_g;
    capacity _offset;
    std::vector<NodeId> _odd_nodes;
    std::vector<EdgeId> _used_edges;
    std::vector<std::vector<metric_closure_member>> _shortest_paths;
};

inline const std::vector<EdgeId> &TJoin::get_used_edges() const
{
    return _used_edges;
}

inline capacity TJoin::get_cap(EdgeId e) const
{
    return std::abs(_g.get_capacity(e) - _offset);
}

}   // namespace MMWC

#endif   // PROG2_TJOIN_HPP
