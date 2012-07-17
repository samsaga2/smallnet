#include "decl.h"
#include <boost/algorithm/string/join.hpp>

using namespace std;

Declarations::~Declarations() {
    for(std::map<AST::MethodFeature*, MethodInfo*>::iterator it = mf_info.begin(); it != mf_info.end(); it++)
        delete it->second;

    for(std::map<AST::FieldFeature*, FieldInfo*>::iterator it = ff_info.begin(); it != ff_info.end(); it++)
        delete it->second;

    for(std::map<AST::Class*, ClassInfo*>::iterator it = cs_info.begin(); it != cs_info.end(); it++)
        delete it->second;
}

void Declarations::push_namespace(AST::Namespace *ns) {
    ns_stack.push_back(ns->id);
}

void Declarations::pop_namespace() {
    ns_stack.pop_back();
}

void Declarations::push_class(AST::Class *cs) {
    cs_stack.push_back(cs);

    // add class info
    ClassInfo *csi = new ClassInfo();
    csi->static_initializer_label = labels.static_class_initializer_label(get_current_ns(), cs);
    csi->initializer_label = labels.class_initializer_label(get_current_ns(), cs);
    cs_info[cs] = csi;
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

    // add field info
    FieldInfo *fi = new FieldInfo();
    if(f->is_static)
        fi->static_label = labels.static_field_label(get_current_ns(), get_current_cs(), f);
    else {
        // TODO calc field idx
    }
    ff_info[f] = fi;
}

void Declarations::add_method(AST::MethodFeature *m) {
    methods[get_current_ns()][get_current_cs()->id][m->id] = m;

    // add method info
    MethodInfo *mi = new MethodInfo();
    if(m->is_static)
        mi->static_label = labels.static_method_label(get_current_ns(), get_current_cs(), m);
    mf_info[m] = mi;
}

bool Declarations::exists_feature(const std::string &id) {
    ClassFieldDeclaration &fd = fields[get_current_ns()][get_current_cs()->id];
    return fd.find(id) != fd.end();
}

