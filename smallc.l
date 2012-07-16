%{
#include <string>
#include "ast.h"
#include "smallc.tab.h"

int linenum;
extern "C" int yylex();

%}

%x COMMENTS

%%

\n                       ++linenum;
[ \t]                    ;
"namespace"              return NS;
"public"                 return PUBLIC;
"protected"              return PROTECTED;
"private"                return PRIVATE;
"static"                 return STATIC;
"class"                  return CLASS;
"{"                      return '{';
"}"                      return '}';
"("                      return '(';
")"                      return ')';
";"                      return ';';
"="                      return '=';
"+"                      return PLUS;
"-"                      return MINUS;
"*"                      return MULT;
"/"                      return DIV;
0b[0-1]+                 { yylval.i = (int)strtol(yytext + 2, NULL, 2); return INT; }
0x[0-9a-fa-f]+           { yylval.i = (int)strtol(yytext + 2, NULL, 16); return INT; }
[0-9]+                   { yylval.i = (int)strtol(yytext, NULL, 10); return INT; }
[a-zA-Z_][a-zA-Z_0-9\.]* { std::cout << yytext << std::endl; yylval.s = new std::string(yytext); return ID; }

\/\/.*                   ;
\/\*                     BEGIN(COMMENTS);
<COMMENTS>\*\/           BEGIN(INITIAL);
<COMMENTS>\n             ++linenum;
<COMMENTS>.              ;

%%

