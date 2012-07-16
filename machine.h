#pragma once

#include "ir.h"
#include <iostream>
#include <set>
#include <map>

class IMachine {
    public:
        virtual void dump_reg(int reg, std::ostream &o) = 0;
        virtual std::set<int> get_regs_by_mask(int reg_mask) = 0;
        virtual void codegen(IR::Block *b) = 0;
};

