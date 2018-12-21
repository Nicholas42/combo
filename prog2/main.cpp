#include <iostream>
#include <cstdlib>
#include <algorithm>

#include "graph.hpp"
#include "tjoin.hpp"
#include "mwc.hpp"

struct blah
{
    bool operator()(const double &a, const double &b) const
    {
        return a < b;
    }
};

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        return 1;
    }
    /*
    blah b;
    PQ::BinaryHeap<double, blah> bh;

    bh.insert(10);
    bh.insert(4);
    bh.insert(4.5);
    bh.insert(1);
    bh.insert(20);
    while (bh.size() > 0)
    {
        std::cout << bh.extract_min() << std::endl;
    }
    //*/
    //*
        MMWC::Graph g = MMWC::Graph::build_graph(argv[1]);
        auto result = MMWC::MinMeanWeightCycle::get_min_mean_cycle(g);

        std::cout << g;

        /*MMWC::TJoin tj(g, 0);

        MMWC::capacity sum = 0;
        std::vector<size_t> degrees(g.num_nodes(), 0);
        for(auto e: tj.get_used_edges())
        {
            auto &ed = g.get_edges()[e];
            sum += ed.cap;
            ++degrees[ed.from];
            ++degrees[ed.to];
           // std::cout << MMWC::to_dimacs_id(ed.from) << "->" << MMWC::to_dimacs_id(ed.to) << "\n";
        }
        for(MMWC::NodeId n = 0; n < g.num_nodes(); ++n)
        {
            if(degrees[n] %2 != 0)
            {
                std::cout << "Odd degree at Node " << n << "\n";
            }
        }
        std::cout << sum << std::endl;
        //*/
        //*/
}
