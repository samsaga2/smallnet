#include "z80.h"

using namespace Z80;
using namespace IR;
using namespace std;

Machine::Machine() {
    std::set<RealReg> byte_regs;
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

    std::set<RealReg> word_regs;
    word_regs.insert(R_BC);
    word_regs.insert(R_DE);
    word_regs.insert(R_HL);
    word_regs.insert(R_IX);
    word_regs.insert(R_IY);

    default_regs[TYPE_U1].insert(byte_regs.begin(), byte_regs.end());
    default_regs[TYPE_S1].insert(byte_regs.begin(), byte_regs.end());
    default_regs[TYPE_U2].insert(word_regs.begin(), word_regs.end());
    default_regs[TYPE_S2].insert(word_regs.begin(), word_regs.end());

    constraints[OP_LOAD].rdsts.insert(R_A);
    constraints[OP_LOAD].rdsts.insert(R_BC);
    constraints[OP_LOAD].rdsts.insert(R_DE);
    constraints[OP_LOAD].rdsts.insert(R_HL);
    constraints[OP_LOAD].rdsts.insert(R_IX);
    constraints[OP_LOAD].rdsts.insert(R_IY);

    constraints[OP_STORE].rsrcs1.insert(R_A);
    constraints[OP_STORE].rsrcs1.insert(R_BC);
    constraints[OP_STORE].rsrcs1.insert(R_DE);
    constraints[OP_STORE].rsrcs1.insert(R_HL);
    constraints[OP_STORE].rsrcs1.insert(R_IX);
    constraints[OP_STORE].rsrcs1.insert(R_IY);

    constraints[OP_ADD].collapse_rdst_and_rsrc1 = true;
    constraints[OP_ADD].rdsts.insert(R_A);
    constraints[OP_ADD].rdsts.insert(R_HL);
    constraints[OP_ADD].rdsts.insert(R_IX);
    constraints[OP_ADD].rsrcs1.insert(R_A);
    constraints[OP_ADD].rsrcs1.insert(R_HL);
    constraints[OP_ADD].rsrcs1.insert(R_IX);
    constraints[OP_ADD].rsrcs2.insert(R_B);
    constraints[OP_ADD].rsrcs2.insert(R_C);
    constraints[OP_ADD].rsrcs2.insert(R_D);
    constraints[OP_ADD].rsrcs2.insert(R_E);
    constraints[OP_ADD].rsrcs2.insert(R_H);
    constraints[OP_ADD].rsrcs2.insert(R_L);
    constraints[OP_ADD].rsrcs2.insert(R_IXH);
    constraints[OP_ADD].rsrcs2.insert(R_IXL);
    constraints[OP_ADD].rsrcs2.insert(R_IYH);
    constraints[OP_ADD].rsrcs2.insert(R_IYL);
    constraints[OP_ADD].rsrcs2.insert(R_BC);
    constraints[OP_ADD].rsrcs2.insert(R_DE);

    constraints[OP_SUB] = constraints[OP_ADD];

    constraints[OP_CALL].rdsts.insert(R_HL);
    constraints[OP_CALL].rsrcs1.insert(R_DE);
    constraints[OP_CALL].rsrcs1.insert(R_E);
    constraints[OP_CALL].rsrcs1.insert(R_D);
    constraints[OP_CALL].rsrcs1.insert(R_HL);
    constraints[OP_CALL].rsrcs1.insert(R_L);
    constraints[OP_CALL].rsrcs1.insert(R_H);
    constraints[OP_CALL].rsrcs1.insert(R_BC);
    constraints[OP_CALL].rsrcs1.insert(R_C);
    constraints[OP_CALL].rsrcs1.insert(R_B);
    constraints[OP_CALL].rsrcs1.insert(R_IY);
    constraints[OP_CALL].rsrcs1.insert(R_IYL);
    constraints[OP_CALL].rsrcs1.insert(R_IYH);

    constraints[OP_RET].rsrcs1.insert(R_HL);
}

void Machine::dump_reg(RealReg reg, std::ostream &o) {
    switch(reg) {
        case R_BC:  o << "bc"; break;
        case R_DE:  o << "de"; break;
        case R_HL:  o << "hl"; break;
        case R_IX:  o << "ix"; break;
        case R_IY:  o << "iy"; break;
        case R_A:   o << "a"; break;
        case R_B:   o << "b"; break;
        case R_C:   o << "c"; break;
        case R_D:   o << "d"; break;
        case R_E:   o << "e"; break;
        case R_H:   o << "h"; break;
        case R_L:   o << "l"; break;
        case R_IXH: o << "ixh"; break;
        case R_IXL: o << "ixl"; break;
        case R_IYH: o << "iyh"; break;
        case R_IYL: o << "iyl"; break;
        default:    o << "#" << reg; break; // internal error
    }
}

int low_reg(int reg) {
    switch(reg) {
        case R_BC: return R_C;
        case R_DE: return R_E;
        case R_HL: return R_L;
        case R_IX: return R_IXL;
        case R_IY: return R_IYL;
    }
    return 0;
}

int high_reg(int reg) {
    switch(reg) {
        case R_BC: return R_B;
        case R_DE: return R_D;
        case R_HL: return R_H;
        case R_IX: return R_IXH;
        case R_IY: return R_IYH;
    }
    return 0;
}


void Machine::asmgen(RealRegMap &hardregs, Inst *inst, ostream &o) {
    switch(inst->opcode) {
        case OP_NOP:
            //o << "\tnop" << endl;
            break;
        case OP_MOVE:
            if(inst->type == TYPE_U2 || inst->type == TYPE_S2) {
                int lowdst = low_reg(hardregs[inst->rdst]);
                int highdst = high_reg(hardregs[inst->rdst]);
                int lowsrc = low_reg(hardregs[inst->rsrc1]);
                int highsrc = high_reg(hardregs[inst->rsrc1]);
                o << "\tld "; dump_reg(lowdst, o); o << ","; dump_reg(lowsrc, o); o << endl;
                o << "\tld "; dump_reg(highdst, o); o << ","; dump_reg(highsrc, o); o << endl;
            } else {
                o << "\tld ";
                dump_reg(hardregs[inst->rdst], o);
                o << ",";
                dump_reg(hardregs[inst->rsrc1], o);
                o << endl;
            }
            break;
        case OP_LOADIMM:
            o << "\tld ";
            dump_reg(hardregs[inst->rdst], o);
            o << "," << inst->vsrc;
            o << endl;
            break;
        case OP_LOAD:
            if(hardregs[inst->rdst] == R_IX) {
                o << "\tld a,(" << inst->lsrc << ")" << endl
                  << "\tld ixl,a" << endl
                  << "\tld a,(" << inst->lsrc << "+1)" << endl
                  << "\tld ixh,a" << endl;
            } else if(hardregs[inst->rdst] == R_IY) {
                o << "\tld a,(" << inst->lsrc << ")" << endl
                  << "\tld iyl,a" << endl
                  << "\tld a,(" << inst->lsrc << "+1)" << endl
                  << "\tld iyh,a" << endl;
            } else {
                o << "\tld ";
                dump_reg(hardregs[inst->rdst], o);
                o << ",(" << inst->lsrc << ")" << endl;
            }
            break;
        case OP_STORE:
            if(hardregs[inst->rsrc1] == R_IX) {
                o << "\tld a,ixl" << endl
                  << "\tld (" << inst->ldst << "),a" << endl
                  << "\tld a,ixh" << endl
                  << "\tld (" << inst->ldst << "+1),a" << endl;
            } else if(hardregs[inst->rsrc1] == R_IY) {
                o << "\tld a,iyl" << endl
                  << "\tld (" << inst->ldst << "),a" << endl
                  << "\tld a,iyh" << endl
                  << "\tld (" << inst->ldst << "+1),a" << endl;
            } else {
                o << "\tld (" << inst->ldst << "),";
                dump_reg(hardregs[inst->rsrc1], o);
                o << endl;
            }
            break;
        case OP_ADD:
            o << "\tadd ";
            dump_reg(hardregs[inst->rdst], o);
            o << ",";
            dump_reg(hardregs[inst->rsrc2], o);
            o << endl;
            break;
        case OP_SUB:
            o << "\tand a; clear carry flag" << endl;
            o << "\tsbc ";
            dump_reg(hardregs[inst->rdst], o);
            o << ",";
            dump_reg(hardregs[inst->rsrc2], o);
            o << endl;
            break;
        case OP_MULT:
            o << "\tcall _op_mul" << endl;
            break;
        case OP_DIV:
            o << "\tcall _op_div" << endl;
            break;
        case OP_RET:
            o << "\tret" << endl;
            break;
        case OP_CALL:
            o << "\tcall " << inst->lsrc << endl;
            break;
    }
}

