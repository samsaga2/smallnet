#pragma once

#include <map>
#include <set>
#include <iostream>
#include "machine.h"
#include "ir.h"

class RegGraph {
    public:
        typedef std::map<IR::VirtualReg, RealReg> VertexFinal;

    private:
        int node_count;

        typedef int Node;
        typedef std::set<Node> Nodes;
        typedef std::set<RealReg> Colors;
        typedef std::map<Node, Nodes> EdgeMap;
        typedef std::map<Node, Colors> NodeColors;
        typedef std::map<Node, RealReg> NodeFinal;
        typedef std::map<IR::VirtualReg, Node> Reg2Node;
        typedef std::map<Node, IR::VirtualReg> Node2Reg;

        Nodes nodes;
        EdgeMap edges;
        NodeColors node_colors;
        BaseMachine *machine;
        Reg2Node reg2node;
        Node2Reg node2reg;

        void build_final(NodeFinal &node_final, VertexFinal &vertex_final);

    public:
        VertexFinal vertex_final;

        RegGraph(BaseMachine *machine) : node_count(0), machine(machine) { }
        void add_reg(IR::VirtualReg irreg);
        void add_edge(IR::VirtualReg irreg1, IR::VirtualReg irreg2);
        void add_reg_candidate(IR::VirtualReg irreg, RealReg mreg);
        bool colorize(VertexFinal &vertex_final);
};

