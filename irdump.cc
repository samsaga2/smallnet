#include "ir.h"

using namespace std;
using namespace IR;

std::string dump_type(Type type) {
    switch(type) {
        case IRTYPE_U8:  return "u8 ";
        case IRTYPE_S8:  return "s8 ";
        case IRTYPE_U16: return "u16";
        case IRTYPE_S16: return "s16";
    }

    return "";
}

void Block::dump(ostream &o) {
    cout << label << ":" << endl;
    for(InstList::iterator it = insts.begin(); it != insts.end(); it++) {
        cout << "\t";
        (*it)->dump(o);
        cout << endl;
    }
}

void Prog::dump(ostream &o) {
    for(BlockList::iterator it = bl.begin(); it != bl.end(); it++) {
        (*it)->dump(o);
        cout << endl;
    }
}

void Move::dump(ostream &o) {
    cout << "move." << dump_type(type) << "  #" << rdst << ",#" << rsrc;
}

void LoadImm::dump(ostream &o) {
    cout << "ldimm." << dump_type(type) << " #" << rdst << "," << imm_src;
}

void Load::dump(ostream &o) {
    cout << "load." << dump_type(type) << "  #" << rdst << "," << label_src;
}

void Store::dump(ostream &o) {
    cout << "store." << dump_type(type) << "  " << label_dst << ",#" << rsrc;
}

void Add::dump(ostream &o) {
    cout << "add." << dump_type(type) << "    " << rdst << ",#" << rsrc1 << ",#" << rsrc2;
}

void Sub::dump(ostream &o) {
    cout << "sub." << dump_type(type) << "    " << rdst << ",#" << rsrc1 << ",#" << rsrc2;
}

void Mult::dump(ostream &o) {
    cout << "mult." << dump_type(type) << "   " << rdst << ",#" << rsrc1 << ",#" << rsrc2;
}

void Div::dump(ostream &o) {
    cout << "div." << dump_type(type) << "    " << rdst << ",#" << rsrc1 << ",#" << rsrc2;
}

