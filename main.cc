#include <iostream>
#include "parse.h"
#include "ast.h"
#include "env.h"

int main() {
    // parse
    AST::Program *program = parse("test.sc");
    if(program == NULL)
        return 1;

    // type checker
    Environment *env = new Environment();
    program->semant(env);

    std::cout << std::endl;

#if DEBUG
    // debug
    program->dump(std::cout);
#endif

    return 0;
}

