#include "ast.h"
#include "env.h"

using namespace AST;
using namespace std;

void Integer::semant(Environment *env) {
    type = "int";
}

void Program::semant(Environment *env) {
    for(NamespaceIterator it = nsl->begin(); it != nsl->end(); it++)
        (*it)->semant(env);
}

void Object::semant(Environment *env) {
    EnvironmentVar* var = env->find_var(id);
    if(var == NULL) {
        std::cerr << linenum << ": `" << id << "' undefined" << std::endl;
        type = "_ErrorType_";
    } else
        type = var->type;
}

void Block::semant(Environment *env) {
    for(StatementIterator it = sl->begin(); it != sl->end(); it++)
        (*it)->semant(env);
}

void FieldFeature::semant(Environment *env) {
    // TODO forbidden dot on id
    
    type = decl_type;
    if(expr == NULL)
        return;

    expr->semant(env);
    if(expr->type != decl_type && expr->type != "_ErrorType_")
        std::cerr << linenum << ": Non-matching types `" << expr->type << "' and `" << decl_type << "'" << std::endl;
}

void MethodFeature::semant(Environment *env) {
    // TODO forbidden dot on id
    // TODO
    type = ret_type;
}

void Namespace::semant(Environment *env) {
    env->push_namespace(this);

    for(ClassIterator it = csl->begin(); it != csl->end(); it++)
        (*it)->semant(env);

    env->pop_namespace();
}

void Class::semant(Environment *env) {
    // TODO forbidden dot on id
    
    env->push_class(this);

    for(FeatureIterator it = fl->begin(); it != fl->end(); it++)
        (*it)->semant(env);

    env->pop_class();
}

void BinOp::semant(Environment *env) {
    e1->semant(env);
    e2->semant(env);
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

