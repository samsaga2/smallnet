#include "ast.h"
#include "env.h"

using namespace AST;
using namespace std;

void Integer::semant(Environment *e) {
    type = new Identifier("int");
}

void Program::semant(Environment *e) {
    for(NamespaceIterator it = nsl->begin(); it != nsl->end(); it++)
        (*it)->semant(e);
}

void Object::semant(Environment *e) {
    type = e->find_var(id);
    if(type == NULL) {
        std::cerr << linenum << ": `" << id->name << "' undefined" << std::endl;
        type = new Identifier("_ErrorType_");
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
    if(expr->type->name != decl_type->name && expr->type->name != "_ErrorType_")
        std::cerr << linenum << ": Non-matching types `" << expr->type->name << "' and `" << decl_type->name << "'" << std::endl;
}

void AttrFeature::semant_declare(Environment *e) {
    if(e->find_var(id) != NULL)
        std::cerr << linenum << ": Duplicated attribute `" << id->name << "'" << std::endl;
        
    e->add_var(id, decl_type);
}

void MethodFeature::semant(Environment *e) {
    // TODO
    type = ret_type;
}

void MethodFeature::semant_declare(Environment *e) {
    if(e->find_var(id) != NULL)
        std::cerr << linenum << ": Duplicated method `" << id->name << "'" << std::endl;

    e->add_var(id, ret_type);
}

void Namespace::semant(Environment *e) {
    string prev_ns = e->ns;
    e->ns += "." + id->name;

    for(ClassIterator it = csl->begin(); it != csl->end(); it++)
        (*it)->semant(e);

    e->ns = prev_ns;
}

void Class::semant(Environment *e) {
    e->push();

    for(FeatureIterator it = fl->begin(); it != fl->end(); it++)
        (*it)->semant_declare(e);

    for(FeatureIterator it = fl->begin(); it != fl->end(); it++)
        (*it)->semant(e);

    e->pop();
}

void BinOp::semant(Environment *e) {
    e1->semant(e);
    e2->semant(e);
    if(e1->type->name != e2->type->name) {
        if(e1->type->name != "_ErrorType_" && e2->type->name != "_ErrorType_")
        {
            std::cerr << linenum << ": Non-matching types `" << e1->type->name << "' and `" << e2->type->name << "'" << std::endl;
            type = new Identifier("_ErrorType_");
        } else if(e1->type->name != "_ErrorType_")
            type = new Identifier(e2->type->name);
        else
            type = new Identifier(e1->type->name);
    } else
        type = new Identifier(e1->type->name.c_str());
}

