#include <cstdlib>
#include <iostream>
#include <getopt.h>
#include <fstream>
#include "ast.h"
#include "decl.h"
#include "env.h"
#include "ir.h"
#include "parse.h"
#include "z80.h"
#include "iropt.h"

using namespace std;

int compile(const char *filename, bool optimize) {
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

    // ir optimizations
    if(optimize) {
        IR::Optimizator o;
        o.optimize(irprog);
    }

#if DEBUG
    // debug
    string ir_filename(filename);
    ir_filename += ".ir";

    ofstream fir;
    fir.open(ir_filename.c_str());
    irprog->dump(fir);
    fir.close();
#endif

    // machine code
    string asm_filename(filename);
    asm_filename += ".asm";

    ofstream fasm;
    fasm.open(asm_filename.c_str());
    Z80::Machine *m = new Z80::Machine();
    m->codegen(irprog, fasm);
    fasm.close();

    if(optimize) {
        // asm optimization
        // TODO
    }

    return 0;
}

int main(int argc, char **argv) {
    int opt;
    bool optimize = false;
    std::string filename;

    while((opt = getopt(argc, argv, "i:Oh")) != -1)
        switch(opt) {
            case 'O': optimize = true; break;
            case 'i': filename = optarg; break;
            case 'h':
            default:
                cout << "smallc -i file.sc [-O]" << endl
                     << "  -i filename      file to compile" << endl
                     << "  -O               enable optimization" << endl
                     << endl;

                return 1;
        }

    if(filename == "") {
        cerr << "Missing input file" << endl;
        return 1;
    }

    return compile(filename.c_str(), optimize);
}

