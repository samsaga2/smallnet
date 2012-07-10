#include "env.h"

using namespace std;

void Environment::push() {
    EnvironmentVarMap *m = new EnvironmentVarMap();
    vars.push_back(m);
}

void Environment::pop() {
    delete vars.back();
    vars.pop_back();
}

void Environment::add_var(std::string id, std::string type) {
    (*vars.back())[id] = type;
}

std::string Environment::find_var(std::string id) {
    for(EnvironmentVarList::reverse_iterator it = vars.rbegin(); it != vars.rend(); it++) {
        EnvironmentVarMap* m = *it;
        if(m->find(id) != m->end())
            return (*m)[id];
    }

    return "";
}

