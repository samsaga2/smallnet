#pragma once

#include <vector>
#include <string>
#include <iostream>

namespace IR {
    typedef enum {
        TYPE_VOID,
        TYPE_U1,
        TYPE_S1,
        TYPE_U2,
        TYPE_S2
    } Type;

    typedef enum {
        OP_NOP,
        OP_MOVE,
        OP_LOADIMM,
        OP_LOAD,
        OP_STORE,
        OP_ADD,
        OP_SUB,
        OP_MULT,
        OP_DIV,
        OP_RET,
        OP_CALL
    } Opcode;

    class Inst {
        public:
            Type type;
            Opcode opcode;
            int rdst;
            int rsrc1;
            int rsrc2;
            std::string ldst;
            std::string lsrc;
            int vsrc;

            Inst() : type(TYPE_VOID), opcode(OP_NOP), rdst(0), rsrc1(0), rsrc2(0), ldst(""), lsrc(""), vsrc(0) { }
            void dump(std::ostream &o);
    };

    typedef std::vector<Inst*> InstList;

    class Block {
        public:
            std::string label;
            InstList il;

            Block(std::string label) : label(label) { }
            ~Block();
            void add(Inst *i) { il.push_back(i); }
            void dump(std::ostream &o);
    };

    typedef std::vector<Block*> BlockList;

    class Prog {
        public:
            BlockList bl;

            ~Prog();
            void dump(std::ostream &o);
            void add(Block *b) { bl.push_back(b); }
    };

    class Build {
        public:
            static Inst *nop();
            static Inst *move(Type type, int rdst, int rsrc);
            static Inst *loadimm(Type type, int rdst, int vsrc);
            static Inst *load(Type type, int rdst, std::string &lsrc);
            static Inst *store(Type type, std::string &ldst, int rsrc);
            static Inst *add(Type type, int rdst, int rsrc1, int rsrc2);
            static Inst *sub(Type type, int rdst, int rsrc1, int rsrc2);
            static Inst *mult(Type type, int rdst, int rsrc1, int rsrc2);
            static Inst *div(Type type, int rdst, int rsrc1, int rsrc2);
            static Inst *retvoid();
            static Inst *callvoid(std::string &lsrc);
    };
}

