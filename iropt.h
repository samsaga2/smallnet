#pragma once

#include "ir.h"

namespace IR {
    class IOptimization {
        public:
            virtual bool optimize(Prog *prog) = 0;
    };

    class Optimizator {
        public:
            void optimize(Prog *prog);
    };
}

