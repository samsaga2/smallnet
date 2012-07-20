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
        int temp_reg;
        EnvironmentVar() { }
        EnvironmentVar(const EnvironmentVar& v) : id(v.id), type(v.type), fi(v.fi), temp_reg(v.temp_reg) { }
        EnvironmentVar(std::string id, std::string type) : id(id), type(type), fi(NULL), temp_reg(0) { }
        EnvironmentVar(std::string id, FieldInfo *fi) : id(id), fi(fi), temp_reg(0) { }
        EnvironmentVar(std::string id, int temp_reg) : id(id), fi(NULL), temp_reg(temp_reg) { }
};

typedef std::list<std::map<std::string, EnvironmentVar> > EnvironmentVarStack;

class Environment {
    private:
        std::list<std::string> ns_stack;
        std::list<AST::Class*> cs_stack;
        std::list<AST::MethodFeature*> mf_stack;
        EnvironmentVarStack vars_stack;

    public:
        Declarations *decl;
        int errors;

        Environment(Declarations *decl) : decl(decl), errors(0) { }

        void push_namespace(AST::Namespace *ns);
        void pop_namespace();

        void push_class(AST::Class *cs);
        void pop_class();

        void push_method(AST::MethodFeature *mf);
        void pop_method();

        std::string get_current_ns();
        AST::Class *get_current_cs() { return cs_stack.back(); }
        AST::MethodFeature *get_current_mf() { return mf_stack.back(); }

        void push_vars();
        void pop_vars();

        void add_var(const EnvironmentVar &v);

        EnvironmentVar *find_var(std::string id);
        AST::MethodFeature *find_method(std::string id);
};

