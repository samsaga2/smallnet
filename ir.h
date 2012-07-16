#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <map>

namespace IR {
    typedef int VirtualReg;

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
            VirtualReg rdst;
            VirtualReg rsrc1;
            VirtualReg rsrc2;
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

    class BlockInfo {
        public:
            typedef std::vector<VirtualReg> LiveRegs;
            typedef std::map<Inst*, LiveRegs> InstsLiveRegs;
            InstsLiveRegs live;

            BlockInfo(Block *b);
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
            static Inst *move(Type type, VirtualReg rdst, VirtualReg rsrc);
            static Inst *loadimm(Type type, VirtualReg rdst, VirtualReg vsrc);
            static Inst *load(Type type, VirtualReg rdst, std::string &lsrc);
            static Inst *store(Type type, std::string &ldst, VirtualReg rsrc);
            static Inst *add(Type type, VirtualReg rdst, VirtualReg rsrc1, VirtualReg rsrc2);
            static Inst *sub(Type type, VirtualReg rdst, VirtualReg rsrc1, VirtualReg rsrc2);
            static Inst *mult(Type type, VirtualReg rdst, VirtualReg rsrc1, VirtualReg rsrc2);
            static Inst *div(Type type, VirtualReg rdst, VirtualReg rsrc1, VirtualReg rsrc2);
            static Inst *retvoid();
            static Inst *callvoid(std::string &lsrc);
    };
}

