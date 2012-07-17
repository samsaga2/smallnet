#include <iostream>
#include <getopt.h>
#include <fstream>
#include "ast.h"
#include "decl.h"
#include "env.h"
#include "ir.h"
#include "parse.h"
#include "z80.h"

using namespace std;

int compile(const char *filename) {
    // parse
    AST::Program *astprog = parse(filename);
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
    string ast_filename(filename);
    ast_filename += ".ast";

    ofstream fast;
    fast.open(ast_filename.c_str());
    astprog->dump(fast);
    fast.close();
#endif

    // code gen
    IR::Prog *irprog = astprog->codegen(env);

#if DEBUG
    // debug
    string ir_filename(filename);
    ir_filename += ".ir";

    ofstream fir;
    fir.open(ir_filename.c_str());
    irprog->dump(fir);
    fir.close();
#endif

    // optimizations
    // TODO

    // machine code
    string asm_filename(filename);
    asm_filename += ".asm";

    ofstream fasm;
    fasm.open(asm_filename.c_str());
    Z80::Machine *m = new Z80::Machine();
    m->codegen(irprog, fasm);
    fasm.close();

    return 0;
}

int main(int argc, char **argv) {
    if(argc == 1) {
        cout << argv[0] << " file.sc" << endl;
        return 0;
    }

    return compile(argv[1]);
}

