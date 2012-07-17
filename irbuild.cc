#include "ir.h"

using namespace IR;
using namespace std;

Inst *Build::nop() {
    Inst *i = new Inst();
    i->type = TYPE_VOID;
    i->opcode = OP_NOP;
    return i;
}

Inst *Build::move(Type type, int rdst, int rsrc) {
    Inst *i = new Inst();
    i->type = type;
    i->opcode = OP_MOVE;
    i->rdst = rdst;
    i->rsrc1 = rsrc;
    return i;
}

Inst *Build::loadimm(Type type, int rdst, int vsrc) {
    Inst *i = new Inst();
    i->type = type;
    i->opcode = OP_LOADIMM;
    i->rdst = rdst;
    i->vsrc = vsrc;
    return i;
}

Inst *Build::load(Type type, int rdst, string lsrc) {
    Inst *i = new Inst();
    i->type = type;
    i->opcode = OP_LOAD;
    i->rdst = rdst;
    i->lsrc = lsrc;
    return i;
}

Inst *Build::store(Type type, string ldst, int rsrc) {
    Inst *i = new Inst();
    i->type = type;
    i->opcode = OP_STORE;
    i->ldst = ldst;
    i->rsrc1 = rsrc;
    return i;
}

Inst *Build::add(Type type, int rdst, int rsrc1, int rsrc2) {
    Inst *i = new Inst();
    i->type = type;
    i->opcode = OP_ADD;
    i->rdst = rdst;
    i->rsrc1 = rsrc1;
    i->rsrc2 = rsrc2;
    return i;
}

Inst *Build::sub(Type type, int rdst, int rsrc1, int rsrc2) {
    Inst *i = new Inst();
    i->type = type;
    i->opcode = OP_SUB;
    i->rdst = rdst;
    i->rsrc1 = rsrc1;
    i->rsrc2 = rsrc2;
    return i;
}

Inst *Build::mult(Type type, int rdst, int rsrc1, int rsrc2) {
    Inst *i = new Inst();
    i->type = type;
    i->opcode = OP_MULT;
    i->rdst = rdst;
    i->rsrc1 = rsrc1;
    i->rsrc2 = rsrc2;
    return i;
}

Inst *Build::div(Type type, int rdst, int rsrc1, int rsrc2) {
    Inst *i = new Inst();
    i->type = type;
    i->opcode = OP_DIV;
    i->rdst = rdst;
    i->rsrc1 = rsrc1;
    i->rsrc2 = rsrc2;
    return i;
}

Inst *Build::retvoid() {
    Inst *i = new Inst();
    i->type = TYPE_VOID;
    i->opcode = OP_RET;
    return i;
}

Inst *Build::callvoid(string lsrc) {
    Inst *i = new Inst();
    i->type = TYPE_VOID;
    i->opcode = OP_CALL;
    i->lsrc = lsrc;
    return i;
}

