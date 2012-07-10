#pragma once

#include <vector>
#include <string>
#include <iostream>

class Environment;

namespace AST {
    class Statement;
    class Namespace;
    class Class;
    class Feature;

    typedef std::vector<Statement*> StatementList;
    typedef std::vector<Namespace*> NamespaceList;
    typedef std::vector<Class*> ClassList;
    typedef std::vector<Feature*> FeatureList;

    typedef std::vector<Statement*>::iterator StatementIterator;
    typedef std::vector<Namespace*>::iterator NamespaceIterator;
    typedef std::vector<Class*>::iterator ClassIterator;
    typedef std::vector<Feature*>::iterator FeatureIterator;

    class Identifier {
        public:
            std::string name;
            Identifier(const std::string &name) : name(name) { }
            void dump(std::ostream &o) { o << name; }
    };

    class Node
    {
        public:
            int linenum;
            Node(int linenum) : linenum(linenum) { }
            virtual void dump(std::ostream &o) = 0;
            virtual void semant(Environment *e) = 0;
            // TODO codegen
    };

    class Program : public Node
    {
        public:
            NamespaceList *nsl;
            Program(int linenum, NamespaceList *nsl) : Node(linenum), nsl(nsl) { }
            Program(int linenum) : Node(linenum), nsl(new NamespaceList()) { }
            ~Program() { delete nsl; }
            void dump(std::ostream &o);
            void semant(Environment *e);
    };

    class Expr : public Node {
        public:
            Identifier* type;
            Expr(int linenum) : Node(linenum) { }
            ~Expr() { delete type; }
    };

    class Statement : public Node {
    };

    class Integer : public Expr {
        public:
            int value;
            Integer(int linenum, int value) : Expr(linenum), value(value) { }
            void dump(std::ostream &o) { o << value; }
            void semant(Environment *e);
    };

    class Object : public Expr {
        public:
            Identifier *id;
            Object(int linenum, Identifier *id) : Expr(linenum), id(id) { }
            ~Object() { delete id; }
            void dump(std::ostream &o) { id->dump(o); }
            void semant(Environment *e);
    };

    class Block : public Node {
        public:
            StatementList *sl;
            Block(int linenum, StatementList *sl) : Node(linenum), sl(sl) { }
            Block(int linenum) : Node(linenum), sl(new StatementList()) { }
            ~Block() { delete sl; }
            void dump(std::ostream &o);
            void semant(Environment *e);
    };

    class Feature : public Node {
        public:
            Feature(int linenum) : Node(linenum) { }
            virtual void semant_declare(Environment *e) = 0;
    };

    class AttrFeature : public Feature {
        public:
            Identifier *id;
            Identifier *decl_type;
            Identifier *type;
            Expr *expr;
            AttrFeature(int linenum, Identifier *id, Identifier *decl_type, Expr *expr = NULL) : Feature(linenum), id(id), decl_type(decl_type), expr(expr) { }
            ~AttrFeature() { delete id; delete decl_type; if(expr) delete expr; }
            void dump(std::ostream &o);
            void semant(Environment *e);
            void semant_declare(Environment *e);
    };

    class MethodFeature : public Feature {
        public:
            Identifier *id;
            Identifier *ret_type;
            Identifier *type;
            Block *block;
            // TODO args
            void dump(std::ostream &o);
            void semant(Environment *e);
            void semant_declare(Environment *e);
    };

    class Namespace : public Node {
        public:
            Identifier *id;
            ClassList *csl;
            Namespace(int linenum, Identifier *id, ClassList *csl) : Node(linenum), id(id), csl(csl) { }
            Namespace(int linenum, Identifier *id) : Node(linenum), id(id), csl(new ClassList()) { }
            ~Namespace() { delete id; delete csl; }
            void dump(std::ostream &o);
            void semant(Environment *e);
    };

    class Class : public Node {
        public:
            Identifier *id;
            FeatureList *fl;
            Class(int linenum, Identifier *id, FeatureList *fl) : Node(linenum), id(id), fl(fl) { }
            Class(int linenum, Identifier *id) : Node(linenum), id(id), fl(new FeatureList()) { }
            ~Class() { delete id; delete fl; }
            void dump(std::ostream &o);
            void semant(Environment *e);
    };

    class BinOp : public Expr {
        public:
            Expr* e1;
            Expr* e2;
            BinOp(int linenum, Expr *e1, Expr *e2) : Expr(linenum), e1(e1), e2(e2) { }
            ~BinOp() { delete e1; delete e2; }
            void semant(Environment *e);
    };

    class Plus : public BinOp {
        public:
            Plus(int linenum, Expr *e1, Expr *e2) : BinOp(linenum, e1, e2) { }
            ~Plus() { delete e1; delete e2; }
            void dump(std::ostream &o) { o << "("; e1->dump(o); o << ")+("; e2->dump(o); o << ")"; }
    };

    class Sub : public BinOp {
        public:
            Sub(int linenum, Expr *e1, Expr *e2) : BinOp(linenum, e1,  e2) { }
            ~Sub() { delete e1; delete e2; }
            void dump(std::ostream &o) { o << "("; e1->dump(o); o << ")-("; e2->dump(o); o << ")"; }
    };

    class Mult : public BinOp {
        public:
            Mult(int linenum, Expr *e1, Expr *e2) : BinOp(linenum, e1,  e2) { }
            ~Mult() { delete e1; delete e2; }
            void dump(std::ostream &o) { o << "("; e1->dump(o); o << ")*("; e2->dump(o); o << ")"; }
    };

    class Div : public BinOp {
        public:
            Div(int linenum, Expr *e1, Expr *e2) : BinOp(linenum, e1,  e2) { }
            ~Div() { delete e1; delete e2; }
            void dump(std::ostream &o) { o << "("; e1->dump(o); o << ")/("; e2->dump(o); o << ")"; }
    };
}

