#include <iostream>
#include "ast.h"
#include "decl.h"
#include "env.h"
#include "ir.h"
#include "parse.h"
#include "z80.h"

using namespace std;

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

    if(env->errors != 0) {
        cout << env->errors << " errors found" << endl;
        return 2;
    }

#if DEBUG
    // debug
    cout << endl << "// AST" << endl;
    astprog->dump(cout);
#endif

    // code gen
    IR::Prog *irprog = astprog->codegen(env);

#if DEBUG
    // debug
    cout << endl << "// IR" << endl;
    irprog->dump(cout);
#endif

    // optimizations
    // TODO

    // machine code
    Z80::Machine *m = new Z80::Machine();
    m->codegen(irprog);

    return 0;
}

