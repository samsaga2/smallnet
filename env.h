#pragma once

#include <string>
#include <list>
#include <map>
#include "ast.h"

typedef std::map<AST::Identifier*, AST::Identifier*> EnvironmentVarMap;
typedef std::list<EnvironmentVarMap*> EnvironmentVarList;

class Environment
{
    private:
        EnvironmentVarList vars;

    public:
        std::string ns;

        void push();
        void pop();

        void add_var(AST::Identifier *id, AST::Identifier *type);
        AST::Identifier *find_var(AST::Identifier *id);
};

