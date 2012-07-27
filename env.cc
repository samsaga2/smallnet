#include "env.h"
#include <boost/algorithm/string/join.hpp>

using namespace std;

void Environment::push_namespace(AST::Namespace *ns) {
    ns_stack.push_back(ns->id);
}

void Environment::pop_namespace() {
    ns_stack.pop_back();
}

void Environment::push_class(AST::Class *cs) {
    cs_stack.push_back(cs);

    push_vars();

    // declare class fields
    ClassFieldDeclaration &f = decl->fields[get_current_ns()][get_current_cs()->id];
    for(ClassFieldDeclaration::iterator it = f.begin(); it != f.end(); it++) {
        AST::FieldFeature *ff = dynamic_cast<AST::FieldFeature*>(it->second);
        if(ff != NULL)
            add_var(EnvironmentVar(ff->id, ff->decl_type));
    }
}

void Environment::pop_class() {
    pop_vars();
    cs_stack.pop_back();
}

void Environment::push_method(AST::MethodFeature *mf) {
    mf_stack.push_back(mf);
}

void Environment::pop_method() {
    mf_stack.pop_back();
}

string Environment::get_current_ns() {
    return boost::algorithm::join(ns_stack, ".");
}

void Environment::push_vars() {
    vars_stack.push_back(map<string, EnvironmentVar>());
}

void Environment::pop_vars() {
    vars_stack.pop_back();
}

void Environment::add_var(const EnvironmentVar &v) {
    vars_stack.back()[v.id] = v;
}

EnvironmentVar *Environment::find_var(string id) {
    for(EnvironmentVarStack::reverse_iterator it = vars_stack.rbegin(); it != vars_stack.rend(); it++) {
        map<string, EnvironmentVar>::iterator it2 = it->find(id);
        if(it2 != it->end())
            return &it2->second;
    }

    return NULL;
}

AST::MethodFeature *Environment::find_method(std::string id) {
    for(list<AST::Class*>::iterator csit = cs_stack.begin(); csit != cs_stack.end(); csit++)  {
        AST::Class *cs = *csit;
        for(AST::FeatureList::iterator fit = cs->fl->begin(); fit != cs->fl->end(); fit++) {
            AST::MethodFeature *mf = dynamic_cast<AST::MethodFeature*>(*fit);
            if(mf != NULL && mf->id == id)
                return mf;
        }
    }

    // TODO maybe id is full name static-method (TestNamespace.TestClass.testMethod)

    return NULL;
}

