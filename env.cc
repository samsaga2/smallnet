#include "env.h"

using namespace std;
using namespace AST;

void Environment::push() {
    EnvironmentVarMap *m = new EnvironmentVarMap();
    vars.push_back(m);
}

void Environment::pop() {
    delete vars.back();
    vars.pop_back();
}

void Environment::add_var(AST::Identifier *id, AST::Identifier *type) {
    (*vars.back())[id] = type;
}

AST::Identifier *Environment::find_var(AST::Identifier *id) {
    for(EnvironmentVarList::reverse_iterator it = vars.rbegin(); it != vars.rend(); it++) {
        EnvironmentVarMap* m = *it;
        if(m->find(id) != m->end())
            return (*m)[id];
    }

    return NULL;
}

