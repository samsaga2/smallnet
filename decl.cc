#include "decl.h"
#include <boost/algorithm/string/join.hpp>

using namespace std;

void Declarations::push_namespace(AST::Namespace *ns) {
    ns_stack.push_back(ns->id);
}

void Declarations::pop_namespace() {
    ns_stack.pop_back();
}

void Declarations::push_class(AST::Class *cs) {
    cs_stack.push_back(cs);
}

void Declarations::pop_class() {
    cs_stack.pop_back();
}

std::string Declarations::get_current_ns() {
    return boost::algorithm::join(ns_stack, ".");
}

void Declarations::add_class(AST::Class *c) {
    classes[get_current_ns()][c->id] = c;
}

void Declarations::add_field(AST::FieldFeature *f) {
    fields[get_current_ns()][get_current_cs()->id][f->id] = f;
}

void Declarations::add_method(AST::MethodFeature *m) {
    // TODO
}

