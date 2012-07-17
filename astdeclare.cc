#include "ast.h"
#include "decl.h"

using namespace AST;
using namespace std;

void Program::declare(Declarations *decl) {
    for(NamespaceList::iterator it = nsl->begin(); it != nsl->end(); it++)
        (*it)->declare(decl);
}

void Namespace::declare(Declarations *decl) {
    decl->push_namespace(this);

    for(ClassList::iterator it = csl->begin(); it != csl->end(); it++)
        (*it)->declare(decl);

    decl->pop_namespace();
}

void Class::declare(Declarations *decl) {
    decl->push_class(this);

    for(FeatureList::iterator it = fl->begin(); it != fl->end(); it++)
        (*it)->declare(decl);

    decl->pop_class();
}

void FieldFeature::declare(Declarations *decl) {
    if(decl->get_current_cs()->is_static && !is_static)
        cerr << linenum << ": Non-static field declared `" << id << "'" << endl;

    if(decl->exists_feature(id)) 
        cerr << linenum << ": Already exists `" << id << "'" << endl;

    decl->add_field(this);
}

void MethodFeature::declare(Declarations *decl) {
    if(decl->get_current_cs()->is_static && !is_static)
        cerr << linenum << ": Non-static method declared `" << id << "'" << endl;

    if(decl->exists_feature(id)) 
        cerr << linenum << ": Already exists `" << id << "'" << endl;

    decl->add_method(this);
}

