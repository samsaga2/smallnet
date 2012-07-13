#include <sstream>
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
        b->add_inst(new IR::Call(csi->static_initializer_label));
    }
    b->add_inst(new IR::Ret());
    irprog->add_block(b);
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
    reg_count = 0;
    for(FeatureList::iterator it = fl->begin(); it != fl->end(); it++) {
        FieldFeature *f = dynamic_cast<FieldFeature*>(*it);
        if(f != NULL && f->expr != NULL && f->is_static) {
            stringstream comment;
            comment << "initializating field " << f->id;
            b->add_inst(new IR::Comment(comment.str()));

            FieldInfo *fi = env->decl->get_field_info(f);
            int rsrc = f->expr->codegen(b, env);
            b->add_inst(new IR::Store(f->expr->get_irtype(), fi->static_label, rsrc));
        }
    }
    b->add_inst(new IR::Ret());
    irprog->add_block(b);
}

void Class::codegen_initializer(IR::Prog *irprog, Environment *env) {
    ClassInfo *csi = env->decl->get_class_info(this);
    IR::Block *b = new IR::Block(csi->initializer_label);
    reg_count = 0;
    for(FeatureList::iterator it = fl->begin(); it != fl->end(); it++) {
        FieldFeature *f = dynamic_cast<FieldFeature*>(*it);
        if(f != NULL && f->expr != NULL && !f->is_static) {
            stringstream comment;
            comment << "initializating field " << f->id;
            b->add_inst(new IR::Comment(comment.str()));

            FieldInfo *fi = env->decl->get_field_info(f);
            int rsrc = f->expr->codegen(b, env);
            // TODO field index
            b->add_inst(new IR::Store(f->expr->get_irtype(), "_molo_mogollon_", rsrc));
        }
    }
    b->add_inst(new IR::Ret());
    irprog->add_block(b);
}

void Class::codegen(IR::Prog *irprog, Environment *env) {
    env->push_class(this);

    codegen_static_initializer(irprog, env);
    codegen_initializer(irprog, env);

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
        return IR::IRTYPE_S2;
    else if(type == "uint")
        return IR::IRTYPE_U2;
    else if(type == "byte")
        return IR::IRTYPE_S1;
    else if(type == "ubyte")
        return IR::IRTYPE_U1;

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

