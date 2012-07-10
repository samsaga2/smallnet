#pragma once

#include <string>
#include <list>
#include <map>
#include "ast.h"

typedef std::map<std::string, std::string> EnvironmentVarMap;
typedef std::list<EnvironmentVarMap*> EnvironmentVarList;

class Environment
{
    private:
        EnvironmentVarList vars;

    public:
        std::string ns;
        AST::Class *c;

        void push();
        void pop();

        void add_var(std::string id, std::string type);
        std::string find_var(std::string id);
};

