%option never-interactive
%option nounistd
%option noyywrap

%{
#include <string>
#include "OpenInfraPlatform/IfcPL/node.h"
#include "parser.hpp"

#define SAVE_TOKEN  yylval.string = new std::string(yytext, yyleng); /* std::cout << yytext << std::endl; */
#define TOKEN(t)    (yylval.token = t)

int line_num = 1;
%}

%option noyywrap
%x IN_COMMENT
%%

"ref"					return TOKEN(REF);
"base"					return TOKEN(BASE);
"break"					return TOKEN(BREAK);
"case"					return TOKEN(CASE);
"class"					return TOKEN(CLASS);
"const"					return TOKEN(CONST);
"continue"				return TOKEN(CONTINUE);
"default"				return TOKEN(DEFAULT);
"do"					return TOKEN(DO);
"else"					return TOKEN(ELSE);
"enum"					return TOKEN(ENUM);
"for"					return TOKEN(FOR);
"if"					return TOKEN(IF);
"import"				return TOKEN(IMPORT);
"interface"				return TOKEN(INTERFACE);
"module"				return TOKEN(MODULE);
"new"					return TOKEN(TNEW);
"null"					return TOKEN(TNULL);
"override"				return TOKEN(OVERRIDE);
"public"|"private"|"protected"		SAVE_TOKEN; return ACCESS_MODIFIER;
"static"				return TOKEN(TSTATIC);
"switch"				return TOKEN(SWITCH);
"this"					return TOKEN(THIS);
"throw"					return TOKEN(THROW);
"while"					return TOKEN(WHILE);
"virtual"				return TOKEN(VIRTUAL);
"cast"					return TOKEN(CAST);
"instanceof"			return TOKEN(INSTANCEOF);
"List"					return TOKEN(LIST);
"Set"					return TOKEN(SET);
"Bag"					return TOKEN(BAG);
"Array"					return TOKEN(ARRAY);

[ \t]                   ;
\n						{ ++line_num; }
"return"                return TOKEN(RETURN);
[a-zA-Z_][a-zA-Z0-9_]*  SAVE_TOKEN; return TIDENIFIER;
[0-9]+\.[0-9]*			SAVE_TOKEN; return DOUBLE_LITERAL;
[0-9]+\.[0-9]*f			SAVE_TOKEN; return FLOAT_LITERAL;
[0-9]+					SAVE_TOKEN; return INTEGER_LITERAL;


"//"[^\r\n]* 		{} // line comment

<INITIAL>
{
    "/*"				BEGIN(IN_COMMENT);
}

<IN_COMMENT>
{
     "*/"				BEGIN(INITIAL);
     [^*\n]+			// eat comment in chunks
     "*"				// eat the lone star
     \n					line_num++;
}

"="						{ return '='; }
"<"						{ return '<'; }
">"						{ return '>'; }
"?"						{ return '?'; }

"=="                    return TOKEN(EQ);
"!="                    return TOKEN(NOTEQ);
"<="                    return TOKEN(LE);
">="                    return TOKEN(GE);

"["						{ return '['; }
"]"						{ return ']'; }
"{"						{ return '{'; }
"}"						{ return '}'; }
"("						{ return '('; }
")"						{ return ')'; }

";"                     { return ';'; }
"."						{ return '.'; }
":"						{ return ':'; }
","						{ return ','; }

"+"                     { return '+'; }
"-"                     { return '-'; }
"*"                     { return '*'; }
"/"                     { return '/'; }
"%"                     { return '%'; }

"+="                    { return ADDEQ; }
"-="                    { return SUBEQ; }
"*="                    { return MULEQ; }
"/="                    { return DIVEQ; }
"!="                    { return NOTEQ; }
"%="                    { return MODEQ; }

"!"                     { return '!'; }

"++"                    { return PLUSPLUS; }
"--"                    { return MINUSMINUS; }

'([^'\\\n]|\\.)'	    SAVE_TOKEN; return CHRLIT;


\"(\\.|[^"])*\"	        SAVE_TOKEN; return TSTRING;

.                       printf("Unknown token!\n"); yyterminate();

%%
