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
        env->errors++;
        cerr << linenum << ": `" << id << "' undefined" << endl;
        type = "int";
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
    if(expr->type != decl_type) {
        env->errors++;
        cerr << linenum << ": Non-matching types `" << expr->type << "' and `" << decl_type << "'" << endl;
    }
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
        env->errors++;
        cerr << linenum << ": Non-matching types `" << e1->type << "' and `" << e2->type << "'" << endl;
        type = "int";
    } else
        type = e1->type;
}

void Assign::semant(Environment *env) {
    expr->semant(env);
    type = "void";
}

void Decl::semant(Environment *env) {
    expr->semant(env);
    if(decl_type != expr->type) {
        env->errors++;
        cerr << linenum << ": Non-matching types `" << decl_type << "' and `" << expr->type << "'" << endl;
    }

    type = "void";
}

