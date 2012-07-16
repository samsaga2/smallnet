#pragma once

#include "ir.h"
#include <iostream>
#include <set>
#include <map>

typedef int RealReg;

class RegGraph;

class InstRegConstraints {
    public:
        IR::VirtualReg dst;
        IR::VirtualReg src1;
        IR::VirtualReg src2;

        InstRegConstraints(IR::VirtualReg dst, IR::VirtualReg src1, IR::VirtualReg src2) : dst(dst), src1(src1), src2(src2) { }
};

class BaseMachine {
    protected:
        typedef std::map<IR::VirtualReg, std::set<RealReg> > DefaultRegs;
        typedef std::map<IR::VirtualReg, RealReg> HardRegs;
        typedef std::vector<InstRegConstraints> RegConstraints;
        typedef std::map<IR::Opcode, RegConstraints> OpcodeRegConstraints;

        OpcodeRegConstraints constraints;
        DefaultRegs default_regs;

        void addGraphNodes(RegGraph &g, IR::Block *b);
        void addGraphEdges(RegGraph &g, IR::Block *b, IR::BlockInfo &binfo);
        void addGraphConstraints(RegGraph &g, IR::Block *b, IR::BlockInfo &binfo);
        HardRegs regallocator(IR::Block *b);
        void codegen(IR::Block *b);

        void asmgen(HardRegs &hardregs, IR::Block *b);
        virtual void asmgen(HardRegs &hardregs, IR::Inst *inst) = 0;

    public:
        std::set<RealReg> get_regs_by_mask(RealReg reg_mask);
        void codegen(IR::Prog *irprog);
};

