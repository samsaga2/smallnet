#include "iropt.h"
#include <map>

using namespace IR;
using namespace std;

class BinOpOptimization : public IOptimization {
    bool optimize(Prog *prog);
};

class RemoveUnusedLoadOptimization : public IOptimization {
    bool optimize(Prog *prog);
};

void Optimizator::optimize(Prog *prog) {
    IOptimization *o[] = {
        new BinOpOptimization(),
        new RemoveUnusedLoadOptimization(),
        NULL
    };

    bool optimized;
    int max_passes = 1000;
    do {
        optimized = false;
        for(size_t i = 0; o[i] != NULL; i++)
            if(o[i]->optimize(prog))
                optimized = true;
    } while(optimized && max_passes--);

    for(size_t i = 0; o[i] != NULL; i++)
        delete o[i];
}

bool BinOpOptimization::optimize(Prog *prog) {
    for(BlockList::iterator it = prog->bl.begin(); it != prog->bl.end(); it++) {
        map<VirtualReg, int> values;
        for(InstList::iterator it2 = (*it)->il.begin(); it2 != (*it)->il.end(); it2++) {
            Inst *i = *it2;
            if(i->opcode == OP_LOADIMM)
                values[i->rdst] = i->vsrc;
            else if(i->opcode == OP_ADD && values.find(i->rsrc1) != values.end() && values.find(i->rsrc2) != values.end()) {
                i->opcode = OP_LOADIMM;
                i->vsrc = values[i->rsrc1] + values[i->rsrc2];
                i->rsrc1 = 0;
                i->rsrc2 = 0;
                return true;
            } else if(i->opcode == OP_SUB && values.find(i->rsrc1) != values.end() && values.find(i->rsrc2) != values.end()) {
                i->opcode = OP_LOADIMM;
                i->vsrc = values[i->rsrc1] - values[i->rsrc2];
                i->rsrc1 = 0;
                i->rsrc2 = 0;
                return true;
            } else if(i->opcode == OP_MULT && values.find(i->rsrc1) != values.end() && values.find(i->rsrc2) != values.end()) {
                i->opcode = OP_LOADIMM;
                i->vsrc = values[i->rsrc1] * values[i->rsrc2];
                i->rsrc1 = 0;
                i->rsrc2 = 0;
                return true;
            } else if(i->opcode == OP_DIV && values.find(i->rsrc1) != values.end() && values.find(i->rsrc2) != values.end()) {
                i->opcode = OP_LOADIMM;
                i->vsrc = values[i->rsrc1] / values[i->rsrc2];
                i->rsrc1 = 0;
                i->rsrc2 = 0;
                return true;
            }
        }
    }

    return false;
}

bool RemoveUnusedLoadOptimization::optimize(Prog *prog) {
    for(BlockList::iterator it = prog->bl.begin(); it != prog->bl.end(); it++)
        for(InstList::iterator it2 = (*it)->il.begin(); it2 != (*it)->il.end(); it2++) {
            Inst *i = *it2;
            if(i->opcode == OP_LOADIMM || i->opcode == OP_LOAD) {
                bool found = false;
                for(InstList::iterator it3 = (*it)->il.begin(); it3 != (*it)->il.end(); it3++)
                    if((*it3)->rsrc1 == i->rdst || (*it3)->rsrc2 == i->rdst) {
                        found = true;
                        break;
                    }

                if(!found) {
                    (*it)->il.erase(it2);
                    return true;
                }
            }
        }

    return false;
}

