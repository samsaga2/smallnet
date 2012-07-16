#include "z80.h"
#include <algorithm>
#include <iterator>

using namespace Z80;
using namespace IR;
using namespace std;

Machine::Machine() {
    byte_regs.insert(R_A);
    byte_regs.insert(R_B);
    byte_regs.insert(R_C);
    byte_regs.insert(R_D);
    byte_regs.insert(R_E);
    byte_regs.insert(R_H);
    byte_regs.insert(R_L);
    byte_regs.insert(R_IXH);
    byte_regs.insert(R_IXL);
    byte_regs.insert(R_IYH);
    byte_regs.insert(R_IYL);

    word_regs.insert(R_BC);
    word_regs.insert(R_DE);
    word_regs.insert(R_HL);
    word_regs.insert(R_IX);
    word_regs.insert(R_IY);

    candidates[OP_LOAD].push_back(InstRegCandidates(R_A , 0, 0));
    candidates[OP_LOAD].push_back(InstRegCandidates(R_BC, 0, 0));
    candidates[OP_LOAD].push_back(InstRegCandidates(R_DE, 0, 0));
    candidates[OP_LOAD].push_back(InstRegCandidates(R_HL, 0, 0));
    candidates[OP_LOAD].push_back(InstRegCandidates(R_IX, 0, 0));
    candidates[OP_LOAD].push_back(InstRegCandidates(R_IY, 0, 0));

    candidates[OP_STORE].push_back(InstRegCandidates(0, R_A , 0));
    candidates[OP_STORE].push_back(InstRegCandidates(0, R_BC, 0));
    candidates[OP_STORE].push_back(InstRegCandidates(0, R_DE, 0));
    candidates[OP_STORE].push_back(InstRegCandidates(0, R_HL, 0));
    candidates[OP_STORE].push_back(InstRegCandidates(0, R_IX, 0));
    candidates[OP_STORE].push_back(InstRegCandidates(0, R_IY, 0));

    candidates[OP_ADD].push_back(InstRegCandidates(R_A , R_A , R_B));
    candidates[OP_ADD].push_back(InstRegCandidates(R_A , R_A , R_C));
    candidates[OP_ADD].push_back(InstRegCandidates(R_A , R_A , R_D));
    candidates[OP_ADD].push_back(InstRegCandidates(R_A , R_A , R_E));
    candidates[OP_ADD].push_back(InstRegCandidates(R_A , R_A , R_H));
    candidates[OP_ADD].push_back(InstRegCandidates(R_A , R_A , R_L));
    candidates[OP_ADD].push_back(InstRegCandidates(R_A , R_A , R_IXH));
    candidates[OP_ADD].push_back(InstRegCandidates(R_A , R_A , R_IXL));
    candidates[OP_ADD].push_back(InstRegCandidates(R_A , R_A , R_IYH));
    candidates[OP_ADD].push_back(InstRegCandidates(R_A , R_A , R_IYL));
    candidates[OP_ADD].push_back(InstRegCandidates(R_HL, R_HL, R_BC));
    candidates[OP_ADD].push_back(InstRegCandidates(R_HL, R_HL, R_DE));
    candidates[OP_ADD].push_back(InstRegCandidates(R_IX, R_IX, R_BC));
    candidates[OP_ADD].push_back(InstRegCandidates(R_IX, R_IX, R_DE));
    candidates[OP_ADD].push_back(InstRegCandidates(R_IY, R_IY, R_BC));
    candidates[OP_ADD].push_back(InstRegCandidates(R_IY, R_IY, R_DE));

    candidates[OP_SUB] = candidates[OP_ADD];
}

void Machine::dump_reg(int reg, std::ostream &o) {
    switch(reg) {
        case R_BC: o << "bc"; break;
        case R_DE: o << "de"; break;
        case R_HL: o << "hl"; break;
        case R_IX: o << "ix"; break;
        case R_IY: o << "iy"; break;
        case R_A: o << "a"; break;
        case R_B: o << "b"; break;
        case R_C: o << "c"; break;
        case R_D: o << "d"; break;
        case R_E: o << "e"; break;
        case R_H: o << "h"; break;
        case R_L: o << "l"; break;
        case R_IXH: o << "ixh"; break;
        case R_IXL: o << "ixl"; break;
        case R_IYH: o << "iyh"; break;
        case R_IYL: o << "iyl"; break;
        default: o << "#" << reg; break; // internal error
    }
}

set<int> Machine::get_regs_by_mask(int reg_mask) {
    set<int> s;

    if(reg_mask & R_A  ) s.insert(R_A);
    if(reg_mask & R_B  ) s.insert(R_B);
    if(reg_mask & R_C  ) s.insert(R_C);
    if(reg_mask & R_D  ) s.insert(R_D);
    if(reg_mask & R_E  ) s.insert(R_E);
    if(reg_mask & R_H  ) s.insert(R_H);
    if(reg_mask & R_L  ) s.insert(R_L);
    if(reg_mask & R_IXH) s.insert(R_IXH);
    if(reg_mask & R_IXL) s.insert(R_IXL);
    if(reg_mask & R_IYH) s.insert(R_IYH);
    if(reg_mask & R_IYL) s.insert(R_IYL);
    if(reg_mask & R_BC ) s.insert(R_BC);
    if(reg_mask & R_DE ) s.insert(R_DE);
    if(reg_mask & R_HL ) s.insert(R_HL);
    if(reg_mask & R_IX ) s.insert(R_IX);
    if(reg_mask & R_IY ) s.insert(R_IY);

    return s;
}

void Machine::codegen(Prog *irprog) {
    for(BlockList::iterator it = irprog->bl.begin(); it != irprog->bl.end(); it++)
        codegen(*it);
}

void Machine::addGraphNodes(RegGraph &g, Block *b) {
    for(InstList::iterator it = b->il.begin(); it != b->il.end(); it++) {
        Inst *inst = *it;
        if(inst->rdst  != 0) g.add_vertex(inst->rdst );
        if(inst->rsrc1 != 0) g.add_vertex(inst->rsrc1);
        if(inst->rsrc2 != 0) g.add_vertex(inst->rsrc2);
    }
}

void Machine::addGraphEdges(RegGraph &g, Block *b, BlockInfo &binfo) {
    for(BlockInfo::InstsLiveRegs::iterator it = binfo.live.begin(); it != binfo.live.end(); it++)
        for(BlockInfo::LiveRegs::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            for(BlockInfo::LiveRegs::iterator it3 = it->second.begin(); it3 != it->second.end(); it3++)
                if(*it2 != *it3)
                    g.add_edge(*it2, *it3);
}

void Machine::addGraphCandidates(RegGraph &g, Block *b, BlockInfo &binfo) {
    // add g general candidates
    map<int, set<int> > reg_candidates;
    for(InstList::iterator it = b->il.begin(); it != b->il.end(); it++) {
        Type irtype = (*it)->type;
        int rdst = (*it)->rdst;
        if(irtype == TYPE_VOID || rdst == 0)
            continue;

        set<int> regs = irtype == TYPE_U1 || irtype == TYPE_S1 ? byte_regs : word_regs;
        for(set<int>::iterator it = regs.begin(); it != regs.end(); it++)
            reg_candidates[rdst].insert(*it);
    }

    // intersect g candidates with dest candidates
    for(InstList::iterator it = b->il.begin(); it != b->il.end(); it++) {
        Type irtype = (*it)->type;
        int rdst = (*it)->rdst;
        Opcode iropcode = (*it)->opcode;
        if(irtype == TYPE_VOID || rdst == 0)
            continue;

        RegCandidates &c = candidates[iropcode];
        if(c.size() == 0)
            continue;

        set<int> dst_cands;
        for(RegCandidates::iterator it2 = c.begin(); it2 != c.end(); it2++)
            dst_cands.insert(it2->dst);

        if(dst_cands.size() == 0)
            continue;

        set<int> new_cands;
        set_intersection(
            reg_candidates[rdst].begin(), reg_candidates[rdst].end(),
            dst_cands.begin(), dst_cands.end(),
            inserter(new_cands, new_cands.end()));
        reg_candidates[rdst] = new_cands;
    }
    
    // intersect g candidates with src1 candidates
    for(InstList::iterator it = b->il.begin(); it != b->il.end(); it++) {
        Type irtype = (*it)->type;
        int rdst = (*it)->rdst;
        int rsrc1 = (*it)->rsrc1;
        Opcode iropcode = (*it)->opcode;
        if(irtype == TYPE_VOID || rsrc1 == 0)
            continue;

        set<int> dstc = reg_candidates[rdst];

        RegCandidates &c = candidates[iropcode];
        if(c.size() == 0)
            continue;

        // reg src candidates
        set<int> src_cands;
        for(RegCandidates::iterator it2 = c.begin(); it2 != c.end(); it2++)
            if(dstc.size() == 0 || dstc.find(it2->dst) != dstc.end())
                src_cands.insert(it2->src1);

        // intersection
        set<int> new_cands;
        set_intersection(
            reg_candidates[rsrc1].begin(), reg_candidates[rsrc1].end(),
            src_cands.begin(), src_cands.end(),
            inserter(new_cands, new_cands.end()));
        reg_candidates[rsrc1] = new_cands;
    }
    
    // intersect g candidates with src2 candidates
    for(InstList::iterator it = b->il.begin(); it != b->il.end(); it++) {
        Type irtype = (*it)->type;
        int rdst = (*it)->rdst;
        int rsrc2 = (*it)->rsrc2;
        Opcode iropcode = (*it)->opcode;
        if(irtype == TYPE_VOID || rsrc2 == 0)
            continue;

        set<int> dstc = reg_candidates[rdst];

        RegCandidates &c = candidates[iropcode];
        if(c.size() == 0)
            continue;

        // reg src candidates
        set<int> src_cands;
        for(RegCandidates::iterator it2 = c.begin(); it2 != c.end(); it2++)
            if(dstc.size() == 0 || dstc.find(it2->dst) != dstc.end())
                src_cands.insert(it2->src2);

        // intersection
        set<int> new_cands;
        set_intersection(
            reg_candidates[rsrc2].begin(), reg_candidates[rsrc2].end(),
            src_cands.begin(), src_cands.end(),
            inserter(new_cands, new_cands.end()));
        reg_candidates[rsrc2] = new_cands;
    }

    // add candidates
    for(map<int, set<int> >::iterator it = reg_candidates.begin(); it != reg_candidates.end(); it++)
        for(set<int>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            g.add_reg_candidate(it->first, *it2);
}

Machine::HardRegs Machine::regallocator(Block *b) {
    BlockInfo binfo(b);

    RegGraph graph(this);
    addGraphNodes(graph, b);
    addGraphEdges(graph, b, binfo);
    addGraphCandidates(graph, b, binfo);

    // colorize graph
    if(!graph.colorize()) {
        // TODO spill
        cout << "*** SPILL NEEDED ***" << endl;
    }

    return graph.vertex_final;
}

void Machine::asmgen(HardRegs &hardregs, Inst *inst) {
    switch(inst->opcode) {
        case OP_NOP:
            cout << "\tnop" << endl;
            break;
        case OP_MOVE:
            cout << "\tld ";
            dump_reg(hardregs[inst->rdst], cout);
            cout << ",";
            dump_reg(hardregs[inst->rdst], cout);
            cout << endl;
            break;
        case OP_LOADIMM:
            cout << "\tld ";
            dump_reg(hardregs[inst->rdst], cout);
            cout << "," << inst->vsrc;
            cout << endl;
            break;
        case OP_LOAD:
            cout << "\tld ";
            dump_reg(hardregs[inst->rdst], cout);
            cout << ",(" << inst->lsrc << ")";
            cout << endl;
            break;
        case OP_STORE:
            cout << "\tld ";
            cout << "(" << inst->ldst << "),";
            dump_reg(hardregs[inst->rsrc1], cout);
            cout << endl;
            break;
        case OP_ADD:
            cout << "\tadd ";
            dump_reg(hardregs[inst->rdst], cout);
            cout << ",";
            dump_reg(hardregs[inst->rsrc2], cout);
            cout << endl;
            break;
        case OP_SUB:
            cout << "\tand a; clear carry flag" << endl;
            cout << "\tsbc ";
            dump_reg(hardregs[inst->rdst], cout);
            cout << ",";
            dump_reg(hardregs[inst->rsrc2], cout);
            cout << endl;
            break;
        case OP_MULT:
            cout << "\tcall _op_mul" << endl;
            break;
        case OP_DIV:
            cout << "\tcall _op_div" << endl;
            break;
        case OP_RET:
            cout << "\tret" << endl;
            break;
        case OP_CALL:
            cout << "\tcall " << inst->lsrc << endl;
            break;
    }
}

void Machine::asmgen(HardRegs &hardregs, IR::Block *b) {
    cout << b->label << ":" << endl;
    for(InstList::iterator it = b->il.begin(); it != b->il.end(); it++)
        asmgen(hardregs, *it);
}

void Machine::codegen(Block *b) {
    HardRegs final = regallocator(b);
    
/*
#if DEBUG
    // mostrar colores elegidos
    for(HardRegs::iterator it = final.begin(); it != final.end(); it++) {
        cout << "final reg for #" << it->first << " -> ";
        dump_reg(it->second, cout);
        cout << endl;
    }
#endif
*/

    asmgen(final, b);
}

