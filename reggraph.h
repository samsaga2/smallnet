#pragma once

#include <map>
#include <set>
#include <iostream>
#include "machine.h"
#include "ir.h"

class RegGraph {
    private:
        typedef std::set<IR::VirtualReg> Vertices;
        typedef std::set<RealReg> Colors;
        typedef std::map<IR::VirtualReg, Vertices> EdgeMap;
        typedef std::map<IR::VirtualReg, Colors> VertexColors;
        typedef std::map<IR::VirtualReg, RealReg> VertexFinal;

        Vertices vertices;
        EdgeMap edges;
        VertexColors vertex_colors;
        IMachine *machine;

    public:
        VertexFinal vertex_final;

        RegGraph(IMachine *machine) : machine(machine) { }
        void clear();
        void add_vertex(IR::VirtualReg irreg);
        void add_edge(IR::VirtualReg irreg1, IR::VirtualReg irreg2);
        void add_reg_candidate(IR::VirtualReg irreg, RealReg mreg);
        void dump(std::ostream &o);
        bool colorize();
};

