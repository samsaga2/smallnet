#include "ast.h"

using namespace AST;

IR::Prog *Program::codegen(Environment *env) {
    IR::Prog *irprog = new IR::Prog();

    for(NamespaceList::iterator it = nsl->begin(); it != nsl->end(); it++)
        (*it)->codegen(irprog, env);

    return irprog;
}

void Namespace::codegen(IR::Prog *irprog, Environment *env) {
    for(ClassList::iterator it = csl->begin(); it != csl->end(); it++)
        (*it)->codegen(irprog, env);
}

void Class::codegen(IR::Prog *irprog, Environment *env) {
    for(FeatureList::iterator it = fl->begin(); it != fl->end(); it++)
        (*it)->codegen(irprog, env);
}

void FieldFeature::codegen(IR::Prog *ir, Environment *env) {
}

void MethodFeature::codegen(IR::Prog *ir, Environment *env) {
}

