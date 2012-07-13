#include "ast.h"
#include "env.h"
#include "ir.h"

using namespace AST;
using namespace std;

int reg_count;

IR::Prog *Program::codegen(Environment *env) {
    IR::Prog *irprog = new IR::Prog();

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

void Class::codegen(IR::Prog *irprog, Environment *env) {
    env->push_class(this);

    // initializier
    ClassInfo *csi = env->decl->get_class_info(this);
    IR::Block *b = new IR::Block(csi->initializer_label);
    reg_count = 0;
    for(FeatureList::iterator it = fl->begin(); it != fl->end(); it++) {
        FieldFeature *f = dynamic_cast<FieldFeature*>(*it);
        if(f != NULL && f->expr != NULL) {
            FieldInfo *fi = env->decl->get_field_info(f);
            int rsrc = f->expr->codegen(b, env);
            if(f->is_static)
                b->add_inst(new IR::Store(f->expr->get_irtype(), fi->static_label, rsrc));
            else {
                // TODO non-static fields
            }
        }
    }
    b->add_inst(new IR::Ret());
    irprog->add_block(b);

    // methods
    for(FeatureList::iterator it = fl->begin(); it != fl->end(); it++)
        (*it)->codegen(irprog, env);

    env->pop_class();
}

void FieldFeature::codegen(IR::Prog *ir, Environment *env) {
}

void MethodFeature::codegen(IR::Prog *ir, Environment *env) {
}

void Block::codegen(IR::Block *b, Environment *env) {
    for(StatementIterator it = sl->begin(); it != sl->end(); it++)
        (*it)->codegen(b, env);
}

IR::Type Expr::get_irtype() {
    if(type == "int")
        return IR::IRTYPE_S16;
    else if(type == "uint")
        return IR::IRTYPE_U16;
    else if(type == "byte")
        return IR::IRTYPE_S8;
    else if(type == "ubyte")
        return IR::IRTYPE_U8;

    throw "INTERNAL ERROR";
}

int Integer::codegen(IR::Block *b, Environment *env) {
    int rdst = reg_count++;
    b->add_inst(new IR::LoadImm(get_irtype(), rdst, value));
    return rdst;
}

int Plus::codegen(IR::Block *b, Environment *env) {
    int rsrc1 = e1->codegen(b, env);
    int rsrc2 = e2->codegen(b, env);
    int rdst = reg_count++;
    b->add_inst(new IR::Add(get_irtype(), rdst, rsrc1, rsrc2));
    return rdst;
}

int Sub::codegen(IR::Block *b, Environment *env) {
    int rsrc1 = e1->codegen(b, env);
    int rsrc2 = e2->codegen(b, env);
    int rdst = reg_count++;
    b->add_inst(new IR::Sub(get_irtype(), rdst, rsrc1, rsrc2));
    return rdst;
}

int Mult::codegen(IR::Block *b, Environment *env) {
    int rsrc1 = e1->codegen(b, env);
    int rsrc2 = e2->codegen(b, env);
    int rdst = reg_count++;
    b->add_inst(new IR::Mult(get_irtype(), rdst, rsrc1, rsrc2));
    return rdst;
}

int Div::codegen(IR::Block *b, Environment *env) {
    int rsrc1 = e1->codegen(b, env);
    int rsrc2 = e2->codegen(b, env);
    int rdst = reg_count++;
    b->add_inst(new IR::Div(get_irtype(), rdst, rsrc1, rsrc2));
    return rdst;
}

int Object::codegen(IR::Block *b, Environment *env) {
    int rdst = reg_count++;
    // TODO
    b->add_inst(new IR::Load(get_irtype(), rdst, "_molo_mucho_label_"));
    return rdst;
}

