%{
#include <cstdio>
#include <string>
#include <iostream>
#include "ast.h"

AST::Program *program;
const char* fname;
extern int linenum;
void yyerror(const char *s);

extern "C" int yyparse();
extern "C" int yylex();
extern "C" FILE *yyin;

%}

%union {
    int i;
    std::string *s;
    AST::Identifier *ident;
    AST::Namespace *ns;
    AST::NamespaceList *nsl;
    AST::Class *cs;
    AST::ClassList *csl;
    AST::Program *p;
    AST::AttrFeature *attr;
    AST::FeatureList *fl;
    AST::Expr *expr;
    AST::Integer *inte;
}

%token ERROR NS CLASS PUBLIC PRIVATE PROTECTED STATIC PLUS MINUS SUB DIV
%token <i> INT
%token <s> ID

%type <p> program
%type <nsl> nsl
%type <ident> ident
%type <ns> ns
%type <csl> csl
%type <cs> cs
%type <attr> attr
%type <fl> features
%type <expr> expr binop term
%type <inte> int

%start program

%left PLUS MINUS
%left MULT DIV

%%

program: nsl { program = new AST::Program(linenum, $1); }
       | { program = new AST::Program(linenum); }
       ;

nsl: ns { $$ = new AST::NamespaceList(); $$->push_back($<ns>1); }
   | nsl ns { $1->push_back($<ns>2); }
   ;

ns: NS ident '{' csl '}' { $$ = new AST::Namespace(linenum, $2, $4); }
  | NS ident '{' '}' { $$ =  new AST::Namespace(linenum, $2); }
  ;

csl: cs { $$ = new AST::ClassList(); $$->push_back($<cs>1); }
   | csl cs { $1->push_back($<cs>2); }
   ;

cs: CLASS ident '{' '}' { $$ = new AST::Class(linenum, $2); }
  | CLASS ident '{' features '}' { $$ = new AST::Class(linenum, $2, $4); }
  ;

features: attr { $$ = new AST::FeatureList(); $$->push_back($1); }
        | features attr { $1->push_back($2); }
        ;

attr: ident ident ';' { $$ = new AST::AttrFeature(linenum, $2, $1); }
    | ident ident '=' expr ';' { $$ = new AST::AttrFeature(linenum, $2, $1, $4); }
    ;

binop: expr PLUS expr { $$ = new AST::Plus(linenum, $1, $3); }
    | expr MINUS expr { $$ = new AST::Sub(linenum, $1, $3); }
    | expr MULT expr { $$ = new AST::Mult(linenum, $1, $3); }
    | expr DIV expr { $$ = new AST::Div(linenum, $1, $3); }
    ;

term: int { $$ = $1; }
    | '(' int  ')' { $$ = $2; }
    | ident { $$ = new AST::Object(linenum, $1); }
    ;

expr: binop
    | term
    ;

ident: ID { $$ = new AST::Identifier(*$1); delete $1; }
     ;

int: INT { $$ = new AST::Integer(linenum, $1); }
   ;

%%

AST::Program *parse(const char *filename) {
    fname = filename;
    yyin = fopen(fname, "r");
    if(!yyin) {
        std::cerr << "Error opening file `" << fname << "'." << std::endl;
        return NULL;
    }

    linenum = 1;
    do {
        yyparse();
    } while(!feof(yyin));

    fclose(yyin);

    return program;
}

void yyerror(const char *s) {
    std::cerr << fname << ":" << linenum << ": " << s << std::endl;
    exit(-1);
}

