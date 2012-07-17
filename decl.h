#pragma once

#include <string>
#include <map>
#include <list>
#include "ast.h"
#include "labels.h"

typedef std::pair<std::string, std::string> FullClassName;
typedef std::map<std::string, std::map<std::string, AST::Class*> > ClassDeclaration;
typedef std::map<std::string, AST::FieldFeature*> ClassFieldDeclaration;
typedef std::map<std::string, std::map<std::string, ClassFieldDeclaration> > FieldDeclaration;
typedef std::map<std::string, AST::MethodFeature*> ClassMethodDeclaration;
typedef std::map<std::string, std::map<std::string, ClassMethodDeclaration> > MethodDeclaration;

class ClassInfo {
    public:
        std::string static_initializer_label;
        std::string initializer_label;
};

class FieldInfo {
    public:
        std::string static_label;
        // TODO object idx
};

class MethodInfo {
    public:
        std::string static_label;
};

typedef std::map<AST::Class*, ClassInfo*> ClassInfoMap;
typedef std::map<AST::FieldFeature*, FieldInfo*> FieldInfoMap;
typedef std::map<AST::MethodFeature*, MethodInfo*> MethodInfoMap;

class Declarations {
    private:
        std::list<std::string> ns_stack;
        std::list<AST::Class*> cs_stack;
        ClassInfoMap cs_info;
        FieldInfoMap ff_info;
        MethodInfoMap mf_info;
        Labels labels;

    public:
        ClassDeclaration classes;
        FieldDeclaration fields;
        MethodDeclaration methods;

        ~Declarations();

        void push_namespace(AST::Namespace *ns);
        void pop_namespace();

        void push_class(AST::Class *cs);
        void pop_class();

        std::string get_current_ns();
        AST::Class *get_current_cs() { return cs_stack.back(); }

        void add_class(AST::Class *c);
        void add_field(AST::FieldFeature *f);
        void add_method(AST::MethodFeature *m);

        bool exists_feature(const std::string &id);

        ClassInfo *get_class_info(AST::Class *cs) { return cs_info[cs]; }
        FieldInfo *get_field_info(AST::FieldFeature *f) { return ff_info[f]; }
        MethodInfo *get_method_info(AST::MethodFeature *m) { return mf_info[m]; }
        
        ClassInfoMap::iterator begin_classes() { return cs_info.begin(); }
        ClassInfoMap::iterator end_classes() { return cs_info.end(); }
};

