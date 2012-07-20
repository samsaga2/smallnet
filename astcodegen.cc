#include <sstream>
#include <cstdlib>
#include "ast.h"
#include "env.h"
#include "ir.h"

using namespace AST;
using namespace std;

int reg_count;

void Program::codegen_start(IR::Prog *irprog, Environment *env) {
    IR::Block *b = new IR::Block(Labels::global_start);
    for(ClassInfoMap::iterator it = env->decl->begin_classes(); it != env->decl->end_classes(); it++) {
        ClassInfo *csi = it->second;
        b->add(IR::Build::callvoid(csi->static_initializer_label));
    }
    b->add(IR::Build::retvoid());
    irprog->add(b);
}

IR::Prog *Program::codegen(Environment *env) {
    IR::Prog *irprog = new IR::Prog();

    codegen_start(irprog, env);

    for(NamespaceList::iterator it = nsl->begin(); it != nsl->end(); it++)
        (*it)->codegen(irprog, env);

    return irprog;
}

void Namespace::codegen(IR::Prog *irprog, Environment *env) {
    env->push_namespace(this);

    for(ClassList::iterator it = csl->begin(); it != csl->end(); it++)
        (*it)->codegen(irprog, env);

    env->pop_namespace();
}

void Class::codegen_static_initializer(IR::Prog *irprog, Environment *env) {
    ClassInfo *csi = env->decl->get_class_info(this);
    IR::Block *b = new IR::Block(csi->static_initializer_label);
    reg_count = 1;
    for(FeatureList::iterator it = fl->begin(); it != fl->end(); it++) {
        FieldFeature *f = dynamic_cast<FieldFeature*>(*it);
        if(f != NULL && f->expr != NULL && f->is_static) {
            FieldInfo *fi = env->decl->get_field_info(f);
            int rsrc = f->expr->codegen(b, env);
            b->add(IR::Build::store(f->expr->get_irtype(), fi->static_label, rsrc));
        }
    }
    b->add(IR::Build::retvoid());
    irprog->add(b);
}

void Class::codegen_initializer(IR::Prog *irprog, Environment *env) {
    ClassInfo *csi = env->decl->get_class_info(this);
    IR::Block *b = new IR::Block(csi->initializer_label);
    reg_count = 1;
    for(FeatureList::iterator it = fl->begin(); it != fl->end(); it++) {
        FieldFeature *f = dynamic_cast<FieldFeature*>(*it);
        if(f != NULL && f->expr != NULL && !f->is_static) {
            int rsrc = f->expr->codegen(b, env);
            // TODO field index
            string label("_chachi_");
            b->add(IR::Build::store(f->expr->get_irtype(), label, rsrc));
        }
    }
    b->add(IR::Build::retvoid());
    irprog->add(b);
}

void Class::codegen(IR::Prog *irprog, Environment *env) {
    env->push_class(this);

    codegen_static_initializer(irprog, env);
    codegen_initializer(irprog, env);

    env->push_vars();

    for(FeatureList::iterator it = fl->begin(); it != fl->end(); it++) {
        AST::FieldFeature *ff = dynamic_cast<AST::FieldFeature*>(*it);
        if(ff != NULL) {
            FieldInfo *fi = env->decl->get_field_info(ff);
            env->add_var(EnvironmentVar(ff->id, fi));
        }
    }

    for(FeatureList::iterator it = fl->begin(); it != fl->end(); it++)
        (*it)->codegen(irprog, env);

    env->pop_vars();

    env->pop_class();
}

void FieldFeature::codegen(IR::Prog *irprog, Environment *env) {
}

void MethodFeature::codegen(IR::Prog *irprog, Environment *env) {
    MethodInfo *mi = env->decl->get_method_info(this);
    IR::Block *b = new IR::Block(mi->static_label);
    reg_count = 1;
    block->codegen(b, env);
    b->add(IR::Build::retvoid());
    irprog->add(b);
}
#include <typeinfo>

void Block::codegen(IR::Block *b, Environment *env) {
    for(StatementIterator it = sl->begin(); it != sl->end(); it++)
        (*it)->codegen(b, env);
}

IR::Type Expr::get_irtype() {
    if(type == "int")
        return IR::TYPE_S2;
    else if(type == "uint")
        return IR::TYPE_U2;
    else if(type == "byte")
        return IR::TYPE_S1;
    else if(type == "ubyte")
        return IR::TYPE_U1;
    else if(type == "void")
        return IR::TYPE_VOID;

    cerr << "INTERNAL ERROR: " << type << endl;
    exit(-2);
}

int Integer::codegen(IR::Block *b, Environment *env) {
    int rdst = reg_count++;
    b->add(IR::Build::loadimm(get_irtype(), rdst, value));
    return rdst;
}

int Plus::codegen(IR::Block *b, Environment *env) {
    int rsrc1 = e1->codegen(b, env);
    int rsrc2 = e2->codegen(b, env);
    int rdst = reg_count++;
    b->add(IR::Build::add(get_irtype(), rdst, rsrc1, rsrc2));
    return rdst;
}

int Sub::codegen(IR::Block *b, Environment *env) {
    int rsrc1 = e1->codegen(b, env);
    int rsrc2 = e2->codegen(b, env);
    int rdst = reg_count++;
    b->add(IR::Build::sub(get_irtype(), rdst, rsrc1, rsrc2));
    return rdst;
}

int Mult::codegen(IR::Block *b, Environment *env) {
    int rsrc1 = e1->codegen(b, env);
    int rsrc2 = e2->codegen(b, env);
    int rdst = reg_count++;
    b->add(IR::Build::mult(get_irtype(), rdst, rsrc1, rsrc2));
    return rdst;
}

int Div::codegen(IR::Block *b, Environment *env) {
    int rsrc1 = e1->codegen(b, env);
    int rsrc2 = e2->codegen(b, env);
    int rdst = reg_count++;
    b->add(IR::Build::div(get_irtype(), rdst, rsrc1, rsrc2));
    return rdst;
}

int Object::codegen(IR::Block *b, Environment *env) {
    int rdst = reg_count++;

    EnvironmentVar *ev = env->find_var(id);
    if(ev->fi != NULL && ev->fi->static_label != "")
        b->add(IR::Build::load(get_irtype(), rdst, ev->fi->static_label));
    else if(ev->temp_reg != 0)
        b->add(IR::Build::move(get_irtype(), rdst, ev->temp_reg));
    else {
        // TODO field index
        string label("_molo_mogollon_");
        b->add(IR::Build::load(get_irtype(), rdst, label));
    }
    return rdst;
}

int Assign::codegen(IR::Block *b, Environment *env) {
    EnvironmentVar *ev = env->find_var(id);

    int rsrc = expr->codegen(b, env);
    
    if(ev->fi != NULL && ev->fi->static_label != "")
        b->add(IR::Build::store(get_irtype(), ev->fi->static_label, rsrc));
    else if(ev->temp_reg != 0)
        b->add(IR::Build::move(get_irtype(), ev->temp_reg, rsrc));
    else {
        // TODO field index
        string label("_molo_mogollon_");
        b->add(IR::Build::store(get_irtype(), label, rsrc));
    }
    return 0;
}

int Decl::codegen(IR::Block *b, Environment *env) {
    int rdst = expr->codegen(b, env);
    env->add_var(EnvironmentVar(id, rdst));
    return 0;
}

