#include "z80.h"
#include "reggraph.h"

using namespace Z80;
using namespace IR;
using namespace std;

Machine::Machine() {
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
        default: o << "??"; break; // internal error
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

Machine::HardRegs Machine::regallocator(Block *b) {
    BlockInfo binfo(b);
    RegGraph graph(this);

    // add nodes
    for(InstList::iterator it = b->il.begin(); it != b->il.end(); it++) {
        Inst *inst = *it;
        if(inst->rdst  != 0) graph.add_vertex(inst->rdst );
        if(inst->rsrc1 != 0) graph.add_vertex(inst->rsrc1);
        if(inst->rsrc2 != 0) graph.add_vertex(inst->rsrc2);
    }

    // add edges
    for(BlockInfo::InstsLiveRegs::iterator it = binfo.live.begin(); it != binfo.live.end(); it++)
        for(BlockInfo::LiveRegs::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            for(BlockInfo::LiveRegs::iterator it3 = it->second.begin(); it3 != it->second.end(); it3++)
                if(*it2 != *it3)
                    graph.add_edge(*it2, *it3);

    // add graph candidates
    for(InstList::iterator it = b->il.begin(); it != b->il.end(); it++) {
        // add candidates
        Type irtype = (*it)->type;
        int rdst = (*it)->rdst;
        if(irtype == TYPE_VOID || rdst == 0)
            continue;

        switch(irtype) {
            case TYPE_VOID: break; // only for warning
            case TYPE_U1:
            case TYPE_S1:
                graph.add_reg_candidate(rdst, R_A);
                graph.add_reg_candidate(rdst, R_B);
                graph.add_reg_candidate(rdst, R_C);
                graph.add_reg_candidate(rdst, R_D);
                graph.add_reg_candidate(rdst, R_E);
                graph.add_reg_candidate(rdst, R_H);
                graph.add_reg_candidate(rdst, R_L);
                graph.add_reg_candidate(rdst, R_IXH);
                graph.add_reg_candidate(rdst, R_IXL);
                graph.add_reg_candidate(rdst, R_IYH);
                graph.add_reg_candidate(rdst, R_IYL);
                break;

            case TYPE_U2:
            case TYPE_S2:
                graph.add_reg_candidate(rdst, R_BC);
                graph.add_reg_candidate(rdst, R_DE);
                graph.add_reg_candidate(rdst, R_HL);
                graph.add_reg_candidate(rdst, R_IX);
                graph.add_reg_candidate(rdst, R_IY);
                break;
        }
    }

    // colorize graph
    if(!graph.colorize()) {
        // TODO spill
        cout << "*** SPILL NEEDED ***" << endl;
    }

    return graph.vertex_final;
}

void Machine::codegen(Block *b) {
    HardRegs final = regallocator(b);
    
#if DEBUG
    // mostrar colores elegidos
    for(HardRegs::iterator it = final.begin(); it != final.end(); it++) {
        cout << "final reg for #" << it->first << " -> ";
        dump_reg(it->second, cout);
        cout << endl;
    }
#endif
}

