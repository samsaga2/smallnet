#pragma once

#include <map>
#include "ir.h"
#include "machine.h"
#include "reggraph.h"

namespace Z80 {
    typedef enum {
        R_A = 1 << 1,
        R_B = 1 << 2,
        R_C = 1 << 3,
        R_D = 1 << 4,
        R_E = 1 << 5,
        R_H = 1 << 6,
        R_L = 1 << 7,
        R_IXH = 1 << 8,
        R_IXL = 1 << 9,
        R_IYH = 1 << 10,
        R_IYL = 1 << 11,
    } RegTypes;

#define R_BC (R_B|R_C)
#define R_DE (R_D | R_E)
#define R_HL (R_H | R_L)
#define R_IX (R_IXH | R_IXL)
#define R_IY (R_IYH | R_IYL)

    class Machine : public BaseMachine {
        protected:
            void asmgen(RealRegMap &hardregs, IR::Inst *inst, std::ostream &o);

        public:
            Machine();
            void dump_reg(RealReg reg, std::ostream &o);
    };
}

