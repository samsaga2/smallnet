#include "ir.h"

using namespace std;
using namespace IR;

std::string dump_type(Type type) {
    switch(type) {
        case IRTYPE_VOID: break;
        case IRTYPE_U8:   return ".u8 ";
        case IRTYPE_S8:   return ".s8 ";
        case IRTYPE_U16:  return ".u16";
        case IRTYPE_S16:  return ".s16";
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
    o << "move" << dump_type(type) << "  #" << rdst << ",#" << rsrc;
}

void LoadImm::dump(ostream &o) {
    o << "ldimm" << dump_type(type) << " #" << rdst << "," << imm_src;
}

void Load::dump(ostream &o) {
    o << "load" << dump_type(type) << "  #" << rdst << "," << label_src;
}

void Store::dump(ostream &o) {
    o << "store" << dump_type(type) << " " << label_dst << ",#" << rsrc;
}

void Add::dump(ostream &o) {
    o << "add" << dump_type(type) << "   #" << rdst << ",#" << rsrc1 << ",#" << rsrc2;
}

void Sub::dump(ostream &o) {
    o << "sub" << dump_type(type) << "   #" << rdst << ",#" << rsrc1 << ",#" << rsrc2;
}

void Mult::dump(ostream &o) {
    o << "mult" << dump_type(type) << "  #" << rdst << ",#" << rsrc1 << ",#" << rsrc2;
}

void Div::dump(ostream &o) {
    o << "div" << dump_type(type) << "   #" << rdst << ",#" << rsrc1 << ",#" << rsrc2;
}

void Ret::dump(ostream &o) {
    o << "ret";
}

