%{
/*$Id: animals-lex.l,v 1.1.1.1 1996/10/30 00:12:54 chase Exp $*/
/* 
	This is the lexer to be used by the simulator generator.
 */
#include <iostream>
#include "String.h"
#include "tokens.h"
using namespace std;

int linenum = 1;
String lex_text;
int lex_int;
double lex_double;
%}
	
ID  [a-zA-Z_][_a-zA-Z0-9]*
NUM [0-9]

%s intype 
%%

^#.*\n		         { linenum++; }


[+-]?{NUM}+		{ lex_int = atoi(yytext); return TOK_INT; } 
[+-]?{NUM}+[eE][+-]?{NUM}+ { lex_double = atof(yytext); return TOK_DOUBLE; } 
[+-]?{NUM}*"."{NUM}+[eE][+-]?{NUM} { lex_double = atof(yytext); return TOK_DOUBLE; } 
[+-]?{NUM}+"."{NUM}*[eE][+-]?{NUM} { lex_double = atof(yytext); return TOK_DOUBLE; } 
[+-]?{NUM}+"."{NUM}* 	{ lex_double = atof(yytext); return TOK_DOUBLE; } 
[+-]?{NUM}*"."{NUM}+ 	{ lex_double = atof(yytext); return TOK_DOUBLE; } 
{ID}	 		{ lex_text = yytext; return TOK_ID; }

"\n"  	   linenum++;

[ \t]+	   ;

.	return yytext[0];
%%

