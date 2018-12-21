#ifndef PROG2_TJOIN_HPP
#define PROG2_TJOIN_HPP

#include "graph.hpp"
#include "PerfectMatching.h"

namespace MMWC
{
class TJoin
{
  public:
    TJoin(const Graph &g, capacity offset, capacity multiplicator) : 
        _g(g), 
        _offset(offset),
        _multiplicator(multiplicator),
        _capacity(0)
    {
        //all_pairs_shortest_paths();
        asp();
        PerfectMatching pm = init_perfect_matching();
        pm.Solve();
        collect_used_edges(pm);
    }

    capacity get_capacity() const;

    const std::vector<char> &get_used_edges() const;

    size_t get_num_edges() const;

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
    capacity _multiplicator;
    capacity _capacity;
    std::vector<NodeId> _odd_nodes;
    std::vector<char> _used_edges;
    std::vector<std::vector<metric_closure_member>> _shortest_paths;
    size_t _num_used_edges;
};

inline const std::vector<char> &TJoin::get_used_edges() const
{
    return _used_edges;
}

inline size_t TJoin::get_num_edges() const
{
    return _num_used_edges;
}

inline capacity TJoin::get_cap(EdgeId e) const
{
    return std::abs(_g.get_capacity(e) * _multiplicator - _offset);
}

}   // namespace MMWC

#endif   // PROG2_TJOIN_HPP
