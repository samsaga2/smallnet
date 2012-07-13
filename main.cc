#include "ast.h"
#include "decl.h"
#include "env.h"
#include "ir.h"
#include "parse.h"
#include <iostream>

int main() {
    // parse
    AST::Program *astprog = parse("test.sc");
    if(astprog == NULL)
        return 1;

    // declareations
    Declarations *decl = new Declarations();
    astprog->declare(decl);

    // type checker
    Environment *env = new Environment(decl);
    astprog->semant(env);

#if DEBUG
    // debug
    std::cout << std::endl << "// AST" << std::endl;
    astprog->dump(std::cout);
#endif

    // code gen
    IR::Prog *irprog = astprog->codegen(env);

#if DEBUG
    // debug
    std::cout << std::endl << "// IR" << std::endl;
    irprog->dump(std::cout);
#endif

    return 0;
}

