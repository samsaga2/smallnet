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

using namespace std;
%}

%union {
    int i;
    std::string *s;
    AST::Namespace *ns;
    AST::NamespaceList *nsl;
    AST::Class *cs;
    AST::ClassList *csl;
    AST::Program *p;
    AST::Feature *fe;
    AST::FeatureList *fl;
    AST::Expr *expr;
    AST::Integer *inte;
    AST::Block *block;
    AST::Arg *arg;
    AST::ArgList *args;
}

%token ERROR NS CLASS PUBLIC PRIVATE PROTECTED STATIC PLUS MINUS SUB DIV
%token INTTYPE UINTTYPE BYTETYPE UBYTETYPE RETURN
%token <i> INT
%token <s> ID

%type <p> program
%type <nsl> nsl
%type <ns> ns
%type <csl> csl
%type <cs> cs
%type <fe> field feature method
%type <fl> features
%type <expr> expr binop term stmt call
%type <inte> int
%type <block> block stmts
%type <arg> arg
%type <args> args

%start program

%left PLUS MINUS
%left MULT DIV

%%

program: nsl { program = new AST::Program(linenum, $1); }
       |     { program = new AST::Program(linenum); }
       ;

nsl: ns { $$ = new AST::NamespaceList(); $$->push_back($<ns>1); }
   | nsl ns { $1->push_back($<ns>2); }
   ;

ns: NS ID '{' csl '}' { $$ = new AST::Namespace(linenum, *$2, $4); delete $2; }
  | NS ID '{' '}'     { $$ =  new AST::Namespace(linenum, *$2); delete $2; }
  ;

csl: cs     { $$ = new AST::ClassList(); $$->push_back($<cs>1); }
   | csl cs { $1->push_back($<cs>2); }
   ;

cs: CLASS ID '{' '}'                    { $$ = new AST::Class(linenum, *$2, false); delete $2; }
  | STATIC CLASS ID '{' '}'             { $$ = new AST::Class(linenum, *$3, true); delete $3; }
  | CLASS ID '{' features '}'           { $$ = new AST::Class(linenum, *$2, $4, false); delete $2; }
  | STATIC CLASS ID '{' features '}'    { $$ = new AST::Class(linenum, *$3, $5, true); delete $3; }
  ;

features: feature          { $$ = new AST::FeatureList(); $$->push_back($1); }
        | features feature { $1->push_back($2); }
        ;

feature: field  { $$ = $1; }
       | method { $$ = $1; }
       ;

field: ID ID ';'                         { $$ = new AST::FieldFeature(linenum, *$2, *$1, NULL, false); delete $2; delete $1; }
     | STATIC ID ID ';'                  { $$ = new AST::FieldFeature(linenum, *$3, *$2, NULL, true); delete $3; delete $2; }
     | ID ID '=' expr ';'                { $$ = new AST::FieldFeature(linenum, *$2, *$1, $4, false); delete $2; delete $1; }
     | STATIC ID ID '=' expr ';'         { $$ = new AST::FieldFeature(linenum, *$3, *$2, $5, true); delete $3; delete $2; }
     ;

method: ID ID '(' args ')' block         { $$ = new AST::MethodFeature(linenum, *$2, *$1, $4, $6, false); delete $2; delete $1; }
      | STATIC ID ID '(' args ')' block  { $$ = new AST::MethodFeature(linenum, *$3, *$2, $5, $7, true); delete $3; delete $2; }
      | ID ID '(' ')' block              { $$ = new AST::MethodFeature(linenum, *$2, *$1, new AST::ArgList(), $5, false); delete $2; delete $1; }
      | STATIC ID ID '(' ')' block       { $$ = new AST::MethodFeature(linenum, *$3, *$2, new AST::ArgList(), $6, true); delete $3; delete $2; }
      ;

args: arg           { $$ = new AST::ArgList(); $$->push_back($1); }
    | args ',' arg { $1->push_back($3); }
    ;

arg: ID ID { $$ = new AST::Arg(*$2, *$1); delete $2; delete $1; }
   ;

block: '{' stmts '}' { $$ = $2; }
     | '{' '}'       { $$ = new AST::Block(linenum); }
     ;

stmts: stmt       { $$ = new AST::Block(linenum); $$->sl->push_back($1); }
     | stmts stmt { $1->sl->push_back($2); }
     ;

stmt: ID '=' expr ';'    { $$ = new AST::Assign(linenum, *$1, $3); delete $1; }
    | ID ID '=' expr ';' { $$ = new AST::Decl(linenum, *$2, *$1, $4); delete $2; delete $1; }
    | call ';'           { $$ = $1; }
    | RETURN ';'         { $$ = new AST::Return(linenum); }
    | RETURN expr ';'    { $$ = new AST::Return(linenum, $2); }
    ;

binop: expr PLUS expr  { $$ = new AST::Plus(linenum, $1, $3); }
     | expr MINUS expr { $$ = new AST::Sub(linenum, $1, $3); }
     | expr MULT expr   { $$ = new AST::Mult(linenum, $1, $3); }
     | expr DIV expr   { $$ = new AST::Div(linenum, $1, $3); }
     ;

term: int          { $$ = $1; }
    | '(' expr ')' { $$ = $2; }
    | ID           { $$ = new AST::Object(linenum, *$1); delete $1; }
    ;

expr: binop
    | term
    | call
    ;

call: ID '(' ')' { $$ = new AST::Call(linenum, *$1); delete $1; }
    ;

int: INT INTTYPE   { $$ = new AST::Integer(linenum, $1, "int"); }
   | INT UINTTYPE  { $$ = new AST::Integer(linenum, $1, "uint"); }
   | INT BYTETYPE  { $$ = new AST::Integer(linenum, $1, "byte"); }
   | INT UBYTETYPE { $$ = new AST::Integer(linenum, $1, "ubyte"); }
   | INT           { $$ = new AST::Integer(linenum, $1, "int"); }
   ;

%%

int errors;

AST::Program *parse(const char *filename) {
    errors = 0;
    fname = filename;
    yyin = fopen(fname, "r");
    if(!yyin) {
        cerr << "Error opening file `" << fname << "'." << endl;
        return NULL;
    }

    linenum = 1;
    do {
        yyparse();
    } while(!feof(yyin));

    fclose(yyin);

    if(errors) {
        cout << errors << " errors found" << endl;
        exit(2);
    }

    return program;
}

void yyerror(const char *s) {
    cerr << fname << ":" << linenum << ": " << s << endl;
    errors++;
}

