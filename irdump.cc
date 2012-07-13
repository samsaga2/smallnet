#include "ir.h"

using namespace std;
using namespace IR;

std::string dump_type(Type type) {
    switch(type) {
        case IRTYPE_VOID: break;
        case IRTYPE_U1: return ".u1";
        case IRTYPE_S1: return ".s1";
        case IRTYPE_U2: return ".u2";
        case IRTYPE_S2: return ".s2";
    }

    return "";
}

void Block::dump(ostream &o) {
    o << label << ":" << endl;
    for(InstList::iterator it = il.begin(); it != il.end(); it++) {
        o << "\t";
        (*it)->dump(o);
        o << endl;
    }
}

void Prog::dump(ostream &o) {
    for(BlockList::iterator it = bl.begin(); it != bl.end(); it++) {
        (*it)->dump(o);
        o << endl;
    }
}

void Move::dump(ostream &o) {
    o << "#" << rdst << " = move" << dump_type(type) << " #" << rsrc;
}

void LoadImm::dump(ostream &o) {
    o << "#" << rdst << " = ldimm" << dump_type(type) << " " << imm_src;
}

void Load::dump(ostream &o) {
    o << "#" << rdst << " = load" << dump_type(type) << " " << label_src;
}

void Store::dump(ostream &o) {
    o << "store" << dump_type(type) << " " << label_dst << ",#" << rsrc;
}

void Add::dump(ostream &o) {
    o << "#" << rdst << " = add" << dump_type(type) << " #" << rsrc1 << ",#" << rsrc2;
}

void Sub::dump(ostream &o) {
    o << "#" << rdst << " = sub" << dump_type(type) << " #" << rsrc1 << ",#" << rsrc2;
}

void Mult::dump(ostream &o) {
    o << "#" << rdst << " = mult" << dump_type(type) << " #" << rsrc1 << ",#" << rsrc2;
}

void Div::dump(ostream &o) {
    o << "#" << rdst << " = div" << dump_type(type) << " #" << rsrc1 << ",#" << rsrc2;
}

void Ret::dump(ostream &o) {
    o << "ret";
}

void Call::dump(ostream &o) {
    if(type == IRTYPE_VOID)
        o << "call " << label;
    else
        o << "#" << rdst << " = call" << dump_type(type) << " " << label;
}

