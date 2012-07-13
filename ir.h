#pragma once

#include <vector>
#include <string>
#include <iostream>

namespace IR {
    typedef enum {
        IRTYPE_VOID,
        IRTYPE_U1,
        IRTYPE_S1,
        IRTYPE_U2,
        IRTYPE_S2
    } Type;

    class Inst {
        public:
            Type type;

            Inst(Type type) : type(type) { }
            virtual ~Inst() { }
            virtual void dump(std::ostream &o) = 0;
    };

    typedef std::vector<Inst*> InstList;

    class Block {
        public:
            std::string label;
            InstList il;

            Block(std::string label) : label(label) { }
            ~Block();
            void add_inst(Inst *i) { il.push_back(i); }
            void dump(std::ostream &o);
    };

    typedef std::vector<Block*> BlockList;

    class Prog {
        public:
            BlockList bl;

            ~Prog();
            void dump(std::ostream &o);
            void add_block(Block *b) { bl.push_back(b); }
    };

    class Move : public Inst {
        public:
            int rdst;
            int rsrc;

            Move(Type type, int rdst, int rsrc) : Inst(type), rdst(rdst), rsrc(rsrc) { }
            void dump(std::ostream &o);
    };

    class LoadImm : public Inst {
        public:
            int rdst;
            int imm_src;

            LoadImm(Type type, int rdst, int imm_src) : Inst(type), rdst(rdst), imm_src(imm_src) { }
            void dump(std::ostream &o);
    };

    class Load : public Inst {
        public:
            int rdst;
            std::string label_src;

            Load(Type type, int rdst, std::string label_src) : Inst(type), rdst(rdst), label_src(label_src) { }
            void dump(std::ostream &o);
    };

    class Store : public Inst {
        public:
            std::string label_dst;
            int rsrc;

            Store(Type type, std::string label_dst, int rsrc) : Inst(type), label_dst(label_dst), rsrc(rsrc) { }
            void dump(std::ostream &o);
    };

    class Add : public Inst {
        public:
            int rdst;
            int rsrc1;
            int rsrc2;

            Add(Type type, int rdst, int rsrc1, int rsrc2) : Inst(type), rdst(rdst), rsrc1(rsrc1), rsrc2(rsrc2) { }
            void dump(std::ostream &o);
    };

    class Sub : public Inst {
        public:
            int rdst;
            int rsrc1;
            int rsrc2;

            Sub(Type type, int rdst, int rsrc1, int rsrc2) : Inst(type), rdst(rdst), rsrc1(rsrc1), rsrc2(rsrc2) { }
            void dump(std::ostream &o);
    };

    class Mult : public Inst {
        public:
            int rdst;
            int rsrc1;
            int rsrc2;

            Mult(Type type, int rdst, int rsrc1, int rsrc2) : Inst(type), rdst(rdst), rsrc1(rsrc1), rsrc2(rsrc2) { }
            void dump(std::ostream &o);
    };

    class Div : public Inst {
        public:
            int rdst;
            int rsrc1;
            int rsrc2;

            Div(Type type, int rdst, int rsrc1, int rsrc2) : Inst(type), rdst(rdst), rsrc1(rsrc1), rsrc2(rsrc2) { }
            void dump(std::ostream &o);
    };

    class Ret : public Inst {
        public:
            Ret() : Inst(IRTYPE_VOID) { }
            void dump(std::ostream &o);
    };

    class Call : public Inst {
        public:
            int rdst;
            std::string label;

            Call(std::string label) : Inst(IRTYPE_VOID), label(label) { }
            Call(Type type, int rdst, std::string label) : Inst(type), rdst(rdst), label(label) { }
            void dump(std::ostream &o);
    };
}

