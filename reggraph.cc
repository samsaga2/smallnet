#include "reggraph.h"
#include <list>
#include <algorithm>
#include <iterator>

using namespace std;
using namespace IR;

void RegGraph::clear() {
    edges.clear();
    vertex_colors.clear();
}

void RegGraph::add_vertex(VirtualReg irreg) {
    vertices.insert(irreg);
}

void RegGraph::add_edge(VirtualReg irreg1, VirtualReg irreg2) {
    edges[irreg1].insert(irreg2);
    edges[irreg2].insert(irreg1);
}

void RegGraph::add_reg_candidate(VirtualReg irreg, RealReg mreg) {
    vertex_colors[irreg].insert(mreg);
}

void RegGraph::dump(ostream &o) {
    // edges
    // TODO iterar por .vertices no por edges asi se muestran los vertices sin edges
    for(Vertices::iterator it = vertices.begin(); it != vertices.end(); it++) {
        o << "edges for #" << *it << " ->";
        Vertices &v = edges[*it];
        for(Vertices::iterator it2 = v.begin(); it2 != v.end(); it2++)
            o << " #" << *it2;
        o << endl;
    }
}

bool RegGraph::colorize() {
    vertex_final.clear();

    // sort vertices by edges count
    typedef list<pair<size_t, IR::VirtualReg> > PairedVerts;
    PairedVerts v;
    for(Vertices::iterator it = vertices.begin(); it != vertices.end(); it++)
        v.push_back(make_pair(edges[*it].size(), *it));
    v.sort();

    // colorize
    EdgeMap e = edges;
    for(PairedVerts::iterator it = v.begin(); it != v.end(); it++) {
        VirtualReg irreg = it->second;
        set<RealReg> candidates = vertex_colors[irreg];
        Vertices &ve = e[irreg];
        for(Vertices::iterator it2 = ve.begin(); it2 != ve.end(); it2++) {
            VirtualReg irreg_edge = *it2;
            RealReg irreg_final = vertex_final[irreg_edge];
            if(irreg_final != 0) {
                set<RealReg> irreg_regs = machine->get_regs_by_mask(irreg_final);
                set<RealReg> result;
                set_difference(
                    candidates.begin(), candidates.end(),
                    irreg_regs.begin(), irreg_regs.end(),
                    inserter(result, result.end()));
                candidates = result;
            }

            if(candidates.size() == 0) {
                // TODO spill
                return false;
            }
        }

        vertex_final[irreg] = *candidates.begin();
    }
    
    return true;
}

