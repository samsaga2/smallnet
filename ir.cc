#include "ir.h"
#include <list>
#include <set>

using namespace IR;

Block::~Block() {
    for(InstList::iterator it = il.begin(); it != il.end(); it++)
        delete *it;
}

Prog::~Prog() {
    for(BlockList::iterator it = bl.begin(); it != bl.end(); it++)
        delete *it;
}

BlockInfo::BlockInfo(Block *b) {
    std::set<VirtualReg> live_regs;

    live.clear();
    for(InstList::reverse_iterator it = b->il.rbegin(); it != b->il.rend(); it++) {
        Inst *i = *it;
        if(i->rdst != 0)
            live_regs.erase(i->rdst);
        if(i->rsrc1 != 0)
            live_regs.insert(i->rsrc1);
        if(i->rsrc2 != 0)
            live_regs.insert(i->rsrc2);

        live[i] = std::vector<VirtualReg>(live_regs.begin(), live_regs.end());
    }
}

