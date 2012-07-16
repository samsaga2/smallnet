#pragma once

#include <map>
#include <set>
#include <iostream>
#include "machine.h"

class RegGraph {
    private:
        typedef int Vertex;
        typedef int Color;

        typedef std::set<Vertex> Vertices;
        typedef std::set<Color> Colors;
        typedef std::map<Vertex, Vertices> EdgeMap;
        typedef std::map<Vertex, Colors> VertexColors;
        typedef std::map<Vertex, Color> VertexFinal;

        Vertices vertices;
        EdgeMap edges;
        VertexColors vertex_colors;
        IMachine *machine;

    public:
        VertexFinal vertex_final;

        RegGraph(IMachine *machine) : machine(machine) { }
        void clear();
        void add_vertex(int irreg);
        void add_edge(int irreg1, int irreg2);
        void add_reg_candidate(int irreg, int mreg);
        void dump(std::ostream &o);
        bool colorize();
};

