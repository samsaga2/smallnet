#include "ast.h"

using namespace AST;

Program::~Program() {
    for(NamespaceList::iterator it = nsl->begin(); it != nsl->end(); it++)
        delete *it;
    delete nsl;
}

Block::~Block() {
    for(StatementList::iterator it = sl->begin(); it != sl->end(); it++)
        delete *it;
    delete sl;
}

Namespace::~Namespace() {
    for(ClassList::iterator it = csl->begin(); it != csl->end(); it++)
        delete *it;
    delete csl;
}

Class::~Class() {
    for(FeatureList::iterator it = fl->begin(); it != fl->end(); it++)
        delete *it;
    delete fl;
}
