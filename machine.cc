#include "machine.h"
#include "reggraph.h"
#include <algorithm>
#include <iterator>

using namespace IR;
using namespace std;

void BaseMachine::codegen(Prog *irprog) {
    for(BlockList::iterator it = irprog->bl.begin(); it != irprog->bl.end(); it++)
        codegen(*it);
}

void BaseMachine::codegen(Block *b) {
    RealRegMap final = regallocator(b);
    asmgen(final, b);
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
        if(inst->rdst  != 0) g.add_vertex(inst->rdst );
        if(inst->rsrc1 != 0) g.add_vertex(inst->rsrc1);
        if(inst->rsrc2 != 0) g.add_vertex(inst->rsrc2);
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
    // add g general constraints
    map<VirtualReg, set<RealReg> > reg_constraints;
    for(InstList::iterator it = b->il.begin(); it != b->il.end(); it++) {
        Type irtype = (*it)->type;
        VirtualReg rdst = (*it)->rdst;
        if(irtype == TYPE_VOID || rdst == 0)
            continue;

        set<RealReg> &regs = default_regs[irtype];
        for(set<RealReg>::iterator it = regs.begin(); it != regs.end(); it++)
            reg_constraints[rdst].insert(*it);
    }

    // intersect g constraints with dest constraints
    for(InstList::iterator it = b->il.begin(); it != b->il.end(); it++) {
        Type irtype = (*it)->type;
        VirtualReg rdst = (*it)->rdst;
        Opcode iropcode = (*it)->opcode;
        if(irtype == TYPE_VOID || rdst == 0)
            continue;

        RegConstraints &c = constraints[iropcode];
        if(c.size() == 0)
            continue;

        set<RealReg> dst_cands;
        for(RegConstraints::iterator it2 = c.begin(); it2 != c.end(); it2++)
            dst_cands.insert(it2->dst);

        if(dst_cands.size() == 0)
            continue;

        set<RealReg> new_cands;
        set_intersection(
            reg_constraints[rdst].begin(), reg_constraints[rdst].end(),
            dst_cands.begin(), dst_cands.end(),
            inserter(new_cands, new_cands.end()));
        reg_constraints[rdst] = new_cands;
    }
    
    // intersect g constraints with src1 constraints
    for(InstList::iterator it = b->il.begin(); it != b->il.end(); it++) {
        Type irtype = (*it)->type;
        VirtualReg rdst = (*it)->rdst;
        VirtualReg rsrc1 = (*it)->rsrc1;
        Opcode iropcode = (*it)->opcode;
        if(irtype == TYPE_VOID || rsrc1 == 0)
            continue;

        set<RealReg> dstc = reg_constraints[rdst];

        RegConstraints &c = constraints[iropcode];
        if(c.size() == 0)
            continue;

        // reg src constraints
        set<RealReg> src_cands;
        for(RegConstraints::iterator it2 = c.begin(); it2 != c.end(); it2++)
            if(dstc.size() == 0 || dstc.find(it2->dst) != dstc.end())
                src_cands.insert(it2->src1);

        // intersection
        set<RealReg> new_cands;
        set_intersection(
            reg_constraints[rsrc1].begin(), reg_constraints[rsrc1].end(),
            src_cands.begin(), src_cands.end(),
            inserter(new_cands, new_cands.end()));
        reg_constraints[rsrc1] = new_cands;
    }
    
    // intersect g constraints with src2 constraints
    for(InstList::iterator it = b->il.begin(); it != b->il.end(); it++) {
        Type irtype = (*it)->type;
        VirtualReg rdst = (*it)->rdst;
        VirtualReg rsrc2 = (*it)->rsrc2;
        Opcode iropcode = (*it)->opcode;
        if(irtype == TYPE_VOID || rsrc2 == 0)
            continue;

        set<RealReg> dstc = reg_constraints[rdst];

        RegConstraints &c = constraints[iropcode];
        if(c.size() == 0)
            continue;

        // reg src constraints
        set<RealReg> src_cands;
        for(RegConstraints::iterator it2 = c.begin(); it2 != c.end(); it2++)
            if(dstc.size() == 0 || dstc.find(it2->dst) != dstc.end())
                src_cands.insert(it2->src2);

        // intersection
        set<RealReg> new_cands;
        set_intersection(
            reg_constraints[rsrc2].begin(), reg_constraints[rsrc2].end(),
            src_cands.begin(), src_cands.end(),
            inserter(new_cands, new_cands.end()));
        reg_constraints[rsrc2] = new_cands;
    }

    // add constraints
    for(map<VirtualReg, set<RealReg> >::iterator it = reg_constraints.begin(); it != reg_constraints.end(); it++)
        for(set<RealReg>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            g.add_reg_candidate(it->first, *it2);
}

BaseMachine::RealRegMap BaseMachine::regallocator(Block *b) {
    BlockInfo binfo(b);

    RegGraph graph(this);
    add_graph_nodes(graph, b);
    add_graph_edges(graph, b, binfo);
    add_graph_constraints(graph, b, binfo);

    // colorize graph
    if(!graph.colorize()) {
        // TODO spill
        cout << "*** SPILL NEEDED ***" << endl;
    }

    return graph.vertex_final;
}

void BaseMachine::asmgen(RealRegMap &hardregs, IR::Block *b) {
    cout << b->label << ":" << endl;
    for(InstList::iterator it = b->il.begin(); it != b->il.end(); it++)
        asmgen(hardregs, *it);
}

