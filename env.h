#pragma once

#include <string>
#include <list>
#include <map>
#include "ast.h"
#include "decl.h"

class EnvironmentVar
{
    public:
        std::string id;
        std::string type;
        FieldInfo *fi;
        EnvironmentVar() { }
        EnvironmentVar(const EnvironmentVar& v) : id(v.id), type(v.type), fi(v.fi) { }
        EnvironmentVar(std::string id, std::string type) : id(id), type(type), fi(NULL) { }
        EnvironmentVar(std::string id, FieldInfo *fi) : id(id), fi(fi) { }
};

typedef std::list<std::map<std::string, EnvironmentVar> > EnvironmentVarStack;

class Environment
{
    private:
        std::list<std::string> ns_stack;
        std::list<AST::Class*> cs_stack;
        EnvironmentVarStack vars_stack;

    public:
        Declarations *decl;
        int errors;

        Environment(Declarations *decl) : decl(decl), errors(0) { }

        void push_namespace(AST::Namespace *ns);
        void pop_namespace();

        void push_class(AST::Class *cs);
        void pop_class();

        std::string get_current_ns();
        AST::Class *get_current_cs() { return cs_stack.back(); }

        void push_vars();
        void pop_vars();

        void add_var(const EnvironmentVar &v);
        EnvironmentVar *find_var(std::string id);
};

