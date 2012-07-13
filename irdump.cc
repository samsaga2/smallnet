#include "ir.h"

using namespace std;
using namespace IR;

void dump_type(Type type, ostream &o) {
    switch(type) {
        case TYPE_VOID: break;
        case TYPE_U1: o << "(u1)"; break;
        case TYPE_S1: o << "(s1)"; break;
        case TYPE_U2: o << "(u2)"; break;
        case TYPE_S2: o << "(s2)"; break;
    }
}

void dump_opcode(Opcode opcode, ostream &o) {
    switch(opcode) {
        case OP_NOP:     o << "nop "; break;
        case OP_MOVE:    o << "mov "; break;
        case OP_LOADIMM: o << "ldi "; break;
        case OP_LOAD:    o << "ld  "; break;
        case OP_STORE:   o << "str "; break;
        case OP_ADD:     o << "add "; break;
        case OP_SUB:     o << "sub "; break;
        case OP_MULT:    o << "mlt "; break;
        case OP_DIV:     o << "div "; break;
        case OP_RET:     o << "ret "; break;
        case OP_CALL:    o << "br  "; break;
    }
}

void Inst::dump(std::ostream &o) {
    if(rdst != 0)
        o << "#" << rdst << " = ";
    else if(ldst != "")
        o << ldst << " = ";

    dump_opcode(opcode, o);
    dump_type(type, o);

    if(rsrc1 != 0)
        o << " #" << rsrc1;
    else if(lsrc != "")
        o << " " << lsrc;
    else if(rdst != 0)
        o << " " << vsrc;

    if(rsrc2 != 0)
        o << " #" << rsrc2;
}

void Prog::dump(ostream &o) {
    for(BlockList::iterator it = bl.begin(); it != bl.end(); it++) {
        (*it)->dump(o);
        o << endl;
    }
}

void Block::dump(ostream &o) {
    o << label << ":" << endl;
    for(InstList::iterator it = il.begin(); it != il.end(); it++) {
        o << "\t";
        (*it)->dump(o);
        o << endl;
    }
}

