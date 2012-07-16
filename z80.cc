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

    constraints[OP_LOAD].push_back(InstRegConstraints(R_A , 0, 0));
    constraints[OP_LOAD].push_back(InstRegConstraints(R_BC, 0, 0));
    constraints[OP_LOAD].push_back(InstRegConstraints(R_DE, 0, 0));
    constraints[OP_LOAD].push_back(InstRegConstraints(R_HL, 0, 0));
    constraints[OP_LOAD].push_back(InstRegConstraints(R_IX, 0, 0));
    constraints[OP_LOAD].push_back(InstRegConstraints(R_IY, 0, 0));

    constraints[OP_STORE].push_back(InstRegConstraints(0, R_A , 0));
    constraints[OP_STORE].push_back(InstRegConstraints(0, R_BC, 0));
    constraints[OP_STORE].push_back(InstRegConstraints(0, R_DE, 0));
    constraints[OP_STORE].push_back(InstRegConstraints(0, R_HL, 0));
    constraints[OP_STORE].push_back(InstRegConstraints(0, R_IX, 0));
    constraints[OP_STORE].push_back(InstRegConstraints(0, R_IY, 0));

    constraints[OP_ADD].push_back(InstRegConstraints(R_A , R_A , R_B));
    constraints[OP_ADD].push_back(InstRegConstraints(R_A , R_A , R_C));
    constraints[OP_ADD].push_back(InstRegConstraints(R_A , R_A , R_D));
    constraints[OP_ADD].push_back(InstRegConstraints(R_A , R_A , R_E));
    constraints[OP_ADD].push_back(InstRegConstraints(R_A , R_A , R_H));
    constraints[OP_ADD].push_back(InstRegConstraints(R_A , R_A , R_L));
    constraints[OP_ADD].push_back(InstRegConstraints(R_A , R_A , R_IXH));
    constraints[OP_ADD].push_back(InstRegConstraints(R_A , R_A , R_IXL));
    constraints[OP_ADD].push_back(InstRegConstraints(R_A , R_A , R_IYH));
    constraints[OP_ADD].push_back(InstRegConstraints(R_A , R_A , R_IYL));
    constraints[OP_ADD].push_back(InstRegConstraints(R_HL, R_HL, R_BC));
    constraints[OP_ADD].push_back(InstRegConstraints(R_HL, R_HL, R_DE));
    constraints[OP_ADD].push_back(InstRegConstraints(R_IX, R_IX, R_BC));
    constraints[OP_ADD].push_back(InstRegConstraints(R_IX, R_IX, R_DE));
    constraints[OP_ADD].push_back(InstRegConstraints(R_IY, R_IY, R_BC));
    constraints[OP_ADD].push_back(InstRegConstraints(R_IY, R_IY, R_DE));

    constraints[OP_SUB] = constraints[OP_ADD];
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

void Machine::asmgen(RealRegMap &hardregs, Inst *inst) {
    switch(inst->opcode) {
        case OP_NOP:
            //cout << "\tnop" << endl;
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

