#include "ast.h"
#include "env.h"

using namespace AST;
using namespace std;

void Program::dump(std::ostream &o) {
    for(NamespaceIterator it = nsl->begin(); it != nsl->end(); it++)
        (*it)->dump(o);
}

void Block::dump(std::ostream &o) {
    for(StatementIterator it = sl->begin(); it != sl->end(); it++) {
        (*it)->dump(o);
        o << ";" << std::endl;
    }
}

void Namespace::dump(std::ostream &o) {
    o << "namespace "; id->dump(o); o << std::endl << "{" << std::endl;
    for(ClassIterator it = csl->begin(); it != csl->end(); it++)
        (*it)->dump(o);
    o << "}" << std::endl;
}

void Class::dump(std::ostream &o) {
    o << "class "; id->dump(o); o << std::endl << "{" << std::endl;
    for(FeatureIterator it = fl->begin(); it != fl->end(); it++)
        (*it)->dump(o);
    o << "}" << std::endl;
}

void AttrFeature::dump(std::ostream &o) {
    if(type == NULL)
        decl_type->dump(o);
    else {
        o << "#";
        type->dump(o);
    }
    o << " "; id->dump(o);
    if(expr != NULL) {
        o << " = ";
        expr->dump(o);
    }
    o << ";" << std::endl;
}

void MethodFeature::dump(std::ostream &o) {
    if(type == NULL)
        ret_type->dump(o);
    else {
        o << "#";
        type->dump(o);
    }
    o << " "; id->dump(o);
    o << "(" << ")" << std::endl << "{" << std::endl;
    block->dump(o);
    o << "}" << std::endl;
}

