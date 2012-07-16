#pragma once

#include <map>
#include <set>
#include <iostream>
#include "machine.h"
#include "ir.h"

class RegGraph {
    private:
        typedef int Color;

        typedef std::set<IR::VirtualReg> Vertices;
        typedef std::set<Color> Colors;
        typedef std::map<IR::VirtualReg, Vertices> EdgeMap;
        typedef std::map<IR::VirtualReg, Colors> VertexColors;
        typedef std::map<IR::VirtualReg, Color> VertexFinal;

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

