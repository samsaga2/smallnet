#include "reggraph.h"
#include <list>
#include <algorithm>
#include <iterator>

using namespace std;
using namespace IR;

void RegGraph::collapse_regs(IR::VirtualReg irreg1, IR::VirtualReg irreg2) {
    int new_node = reg2node[irreg1];
    int node_to_delete = reg2node[irreg2];

    for(Reg2Node::iterator it = reg2node.begin(); it != reg2node.end(); it++)
        if(it->second == node_to_delete)
            reg2node[it->first] = new_node;

    nodes.erase(node_to_delete);
}

void RegGraph::add_reg(VirtualReg irreg) {
    if(reg2node[irreg] != 0)
        return;

    reg2node[irreg] = node_count++;
    nodes.insert(reg2node[irreg]);
}

void RegGraph::add_edge(VirtualReg irreg1, VirtualReg irreg2) {
    edges[reg2node[irreg1]].insert(reg2node[irreg2]);
    edges[reg2node[irreg2]].insert(reg2node[irreg1]);
}

void RegGraph::add_reg_candidate(VirtualReg irreg, RealReg mreg) {
    node_colors[reg2node[irreg]].insert(mreg);
}

void RegGraph::build_final(NodeFinal &node_final, VertexFinal &vertex_final) {
    for(Reg2Node::iterator it = reg2node.begin(); it != reg2node.end(); it++)
        vertex_final[it->first] = node_final[it->second];
}

bool RegGraph::colorize(VertexFinal &vertex_final) {
    vertex_final.clear();

    // sort vertices by edges count
    typedef list<pair<size_t, Node> > PairedNodes;
    PairedNodes pn;
    for(Nodes::iterator it = nodes.begin(); it != nodes.end(); it++)
        pn.push_back(make_pair(edges[*it].size(), *it));
    pn.sort();

    // colorize
    NodeFinal node_final;
    EdgeMap e = edges;
    for(PairedNodes::iterator it = pn.begin(); it != pn.end(); it++) {
        Node node = it->second;
        set<RealReg> candidates = node_colors[node];
        Nodes &ne = e[node];
        for(Nodes::iterator it2 = ne.begin(); it2 != ne.end(); it2++) {
            Node node_edge = *it2;
            RealReg reg_final = node_final[node_edge];
            if(reg_final != 0) {
                set<RealReg> irreg_regs = machine->get_regs_by_mask(reg_final);
                set<RealReg> result;
                set_difference(
                    candidates.begin(), candidates.end(),
                    irreg_regs.begin(), irreg_regs.end(),
                    inserter(result, result.end()));
                candidates = result;
            }

            if(candidates.size() == 0) {
                // TODO spill
                build_final(node_final, vertex_final); // TODO delete this line when spill is done
                return false;
            }
        }

        node_final[node] = *candidates.begin();
    }
    
    build_final(node_final, vertex_final);
    return true;
}

