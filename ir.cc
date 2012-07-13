#include "ir.h"

using namespace IR;

Block::~Block() {
    for(InstList::iterator it = il.begin(); it != il.end(); it++)
        delete *it;
}

Prog::~Prog() {
    for(BlockList::iterator it = bl.begin(); it != bl.end(); it++)
        delete *it;
}

