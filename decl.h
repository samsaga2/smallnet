#pragma once

#include <string>
#include <map>
#include <list>
#include "ast.h"

typedef std::pair<std::string, std::string> FullClassName;
typedef std::map<std::string, std::map<std::string, AST::Class*> > ClassDeclaration;
typedef std::map<std::string, AST::FieldFeature*> ClassFieldDeclaration;
typedef std::map<std::string, std::map<std::string, ClassFieldDeclaration> > FieldDeclaration;

class Declarations
{
    private:
        std::list<std::string> ns_stack;
        std::list<AST::Class*> cs_stack;

    public:
        ClassDeclaration classes;
        FieldDeclaration fields;

        void push_namespace(AST::Namespace *ns);
        void pop_namespace();

        void push_class(AST::Class *cs);
        void pop_class();

        std::string get_current_ns();
        AST::Class *get_current_cs() { return cs_stack.back(); }

        void add_class(AST::Class *c);
        void add_field(AST::FieldFeature *f);
        void add_method(AST::MethodFeature *m);
};

