#!/bin/env python3
import sys, os

OPTIMA_FILE = os.path.join("graphs", "optima.md")
SOL_EXT = ".sol"
DMX_EXT = ".dmx"

def get_edges(filename):
    if not os.path.isfile(filename):
        print("Couldn't find file %s."%filename)
        return None

    with open(filename) as f:
        for i in f:
            if i.startswith("c"):
                continue
            if not i.startswith("p"):
                return None

            return int(i.strip().split(" ")[-1])


def get_graph_name(filename):
    if filename.endswith(SOL_EXT):
        return filename[:-len(SOL_EXT)]
    return filename

def get_optimum(graph):
    if not os.path.isfile(OPTIMA_FILE):
        print("Couldn't finde optimafile %s."%OPTIMA_FILE)
        return None

    with open(OPTIMA_FILE) as f:
        for i in f:
            if i.startswith(graph):
                return int(i.rpartition("|")[2].strip())

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("""Usage: %s solutionfile.sol"""%sys.argv[0])
        sys.exit()

    num_edges = get_edges(sys.argv[1])

    if num_edges is None:
        print("File %s is no dmx file.")
        sys.exit()

    optimum = get_optimum(get_graph_name(sys.argv[1]))

    if optimum is None:
        print("Couldn't find graph %s in %s."%(get_graph_name(sys.argv[1]), OPTIMA_FILE))
        sys.exit()

    if optimum == num_edges:
        print("Solution is optimal.")
    else:
        print("Solution is not optimal. Optimum: %s, Solution: %s"%(optimum, num_edges))
        if optimum < num_edges:
            print("WARNING: Solution is to big.")
