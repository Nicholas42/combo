#include <iostream>
#include <cstdlib>

#include "graph.hpp"
#include "edmonds.hpp"

int main(int argc, char **argv)
{
   if (argc != 2)
   {
      std::cerr << "Wrong number of arguments. Program call: <program_name> <input_graph>" << std::endl;
      return EXIT_FAILURE;
   }

   ED::Graph graph = ED::Graph::build_graph(argv[1]);

   ED::Graph matching = ED::EdmondsMatching::get_matching(graph);

   std::cout << matching;
   return EXIT_SUCCESS;
}
