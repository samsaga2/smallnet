#include "ast.h"
#include "env.h"

using namespace AST;
using namespace std;

void Integer::semant(Environment *e) {
    type = "int";
}

void Program::semant(Environment *e) {
    for(NamespaceIterator it = nsl->begin(); it != nsl->end(); it++)
        (*it)->semant(e);
}

void Object::semant(Environment *e) {
    type = e->find_var(id);
    if(type == "") {
        std::cerr << linenum << ": `" << id << "' undefined" << std::endl;
        type = "_ErrorType_";
    }
}

void Block::semant(Environment *e) {
    for(StatementIterator it = sl->begin(); it != sl->end(); it++)
        (*it)->semant(e);
}

void AttrFeature::semant(Environment *e) {
    type = decl_type;
    if(expr == NULL)
        return;

    expr->semant(e);
    if(expr->type != decl_type && expr->type != "_ErrorType_")
        std::cerr << linenum << ": Non-matching types `" << expr->type << "' and `" << decl_type << "'" << std::endl;
}

void AttrFeature::semant_declare(Environment *e) {
    if(e->find_var(id) != "")
        std::cerr << linenum << ": Duplicated attribute `" << id << "'" << std::endl;
        
    e->add_var(id, decl_type);
}

void MethodFeature::semant(Environment *e) {
    // TODO
    type = ret_type;
}

void MethodFeature::semant_declare(Environment *e) {
    if(e->find_var(id) != "")
        std::cerr << linenum << ": Duplicated method `" << id << "'" << std::endl;

    e->add_var(id, ret_type);
}

void Namespace::semant(Environment *e) {
    string prev_ns = e->ns;
    e->ns += "." + id;

    for(ClassIterator it = csl->begin(); it != csl->end(); it++)
        (*it)->semant(e);

    e->ns = prev_ns;
}

void Class::semant(Environment *e) {
    Class *prev_c = e->c;
    e->c = this;
    e->push();

    for(FeatureIterator it = fl->begin(); it != fl->end(); it++)
        (*it)->semant_declare(e);

    for(FeatureIterator it = fl->begin(); it != fl->end(); it++)
        (*it)->semant(e);

    e->pop();
    e->c = prev_c;
}

void BinOp::semant(Environment *e) {
    e1->semant(e);
    e2->semant(e);
    if(e1->type != e2->type) {
        if(e1->type != "_ErrorType_" && e2->type != "_ErrorType_") {
            std::cerr << linenum << ": Non-matching types `" << e1->type << "' and `" << e2->type << "'" << std::endl;
            type = "_ErrorType_";
        } else if(e1->type != "_ErrorType_")
            type = e2->type;
        else
            type = e1->type;
    } else
        type = e1->type;
}

