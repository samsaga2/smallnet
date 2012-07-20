#include "machine.h"
#include "reggraph.h"
#include <algorithm>
#include <iterator>

using namespace IR;
using namespace std;

void BaseMachine::codegen(Prog *irprog, ostream &o) {
    for(BlockList::iterator it = irprog->bl.begin(); it != irprog->bl.end(); it++)
        codegen(*it, o);
}

void BaseMachine::codegen(Block *b, ostream &o) {
    RealRegMap final = regallocator(b);
    asmgen(final, b, o);
}

set<RealReg> BaseMachine::get_regs_by_mask(RealReg reg_mask) {
    set<RealReg> s;

    for(DefaultRegs::iterator it = default_regs.begin(); it != default_regs.end(); it++)
        for(set<RealReg>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            if((*it2) & reg_mask)
                s.insert(*it2);

    return s;
}

void BaseMachine::add_graph_nodes(RegGraph &g, Block *b) {
    for(InstList::iterator it = b->il.begin(); it != b->il.end(); it++) {
        Inst *inst = *it;
        if(inst->rdst  != 0) g.add_reg(inst->rdst );
        if(inst->rsrc1 != 0) g.add_reg(inst->rsrc1);
        if(inst->rsrc2 != 0) g.add_reg(inst->rsrc2);
    }
}

void BaseMachine::add_graph_collapses(RegGraph &g, Block *b) {
    for(InstList::iterator it = b->il.begin(); it != b->il.end(); it++) {
        Inst *inst = *it;

        OpcodeRegConstraints::iterator orcit = constraints.find((*it)->opcode);
        if(orcit == constraints.end())
            continue;

        if(orcit->second.collapse_rdst_and_rsrc1 && inst->rdst != 0 && inst->rsrc1 != 0)
            g.collapse_regs(inst->rdst, inst->rsrc1);
    }
}

void BaseMachine::add_graph_edges(RegGraph &g, Block *b, BlockInfo &binfo) {
    for(BlockInfo::InstsLiveRegs::iterator it = binfo.live.begin(); it != binfo.live.end(); it++)
        for(BlockInfo::LiveRegs::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            for(BlockInfo::LiveRegs::iterator it3 = it->second.begin(); it3 != it->second.end(); it3++)
                if(*it2 != *it3)
                    g.add_edge(*it2, *it3);
}

void BaseMachine::add_graph_constraints(RegGraph &g, Block *b, BlockInfo &binfo) {
    map<VirtualReg, set<RealReg> > reg_constraints;

    // add g default regs
    for(InstList::iterator it = b->il.begin(); it != b->il.end(); it++) {
        VirtualReg rdst = (*it)->rdst;
        if(rdst != 0) {
            Type irtype = (*it)->type;
            set<RealReg> &regs = default_regs[irtype];
            for(set<RealReg>::iterator it = regs.begin(); it != regs.end(); it++)
                reg_constraints[rdst].insert(*it);
        }
    }

    // intersect g constraints with constraints
    for(InstList::iterator it = b->il.begin(); it != b->il.end(); it++) {
        OpcodeRegConstraints::iterator orcit = constraints.find((*it)->opcode);
        if(orcit == constraints.end())
            continue;

        // dst
        VirtualReg rdst = (*it)->rdst;
        if(rdst != 0 && orcit->second.rdsts.size() != 0) {
            set<RealReg> new_cands;
            set_intersection(
                reg_constraints[rdst].begin(), reg_constraints[rdst].end(),
                orcit->second.rdsts.begin(), orcit->second.rdsts.end(),
                inserter(new_cands, new_cands.end()));
            reg_constraints[rdst] = new_cands;
        }

        // src1
        VirtualReg rsrc1 = (*it)->rsrc1;
        if(rsrc1 != 0 && orcit->second.rsrcs1.size() != 0) {
            set<RealReg> new_cands;
            set_intersection(
                reg_constraints[rsrc1].begin(), reg_constraints[rsrc1].end(),
                orcit->second.rsrcs1.begin(), orcit->second.rsrcs1.end(),
                inserter(new_cands, new_cands.end()));
            reg_constraints[rsrc1] = new_cands;
        }

        // src2
        VirtualReg rsrc2 = (*it)->rsrc2;
        if(rsrc2 != 0 && orcit->second.rsrcs2.size() != 0) {
            set<RealReg> new_cands;
            set_intersection(
                reg_constraints[rsrc2].begin(), reg_constraints[rsrc2].end(),
                orcit->second.rsrcs2.begin(), orcit->second.rsrcs2.end(),
                inserter(new_cands, new_cands.end()));
            reg_constraints[rsrc2] = new_cands;
        }
    }

    // add constraints
    for(map<VirtualReg, set<RealReg> >::iterator it = reg_constraints.begin(); it != reg_constraints.end(); it++)
        for(set<RealReg>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            g.add_reg_candidate(it->first, *it2);
}

BaseMachine::RealRegMap BaseMachine::regallocator(Block *b) {
    BlockInfo binfo(b);

    RegGraph::VertexFinal vertex_final;
    RegGraph graph(this);
    add_graph_nodes(graph, b);
    add_graph_collapses(graph, b);
    add_graph_edges(graph, b, binfo);
    add_graph_constraints(graph, b, binfo);

    // colorize graph
    if(!graph.colorize(vertex_final)) {
        // TODO spill
        cout << "*** SPILL NEEDED ***" << endl;
    }

    return vertex_final;
}

void BaseMachine::asmgen(RealRegMap &hardregs, IR::Block *b, ostream &o) {
    o << b->label << ":" << endl;
    for(InstList::iterator it = b->il.begin(); it != b->il.end(); it++)
        asmgen(hardregs, *it, o);
}

