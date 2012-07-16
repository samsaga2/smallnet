#pragma once

#include "ir.h"
#include <iostream>
#include <set>
#include <map>

typedef int RealReg;

class IMachine {
    public:
        virtual std::set<RealReg> get_regs_by_mask(RealReg reg_mask) = 0;
        virtual void codegen(IR::Block *b) = 0;
};

