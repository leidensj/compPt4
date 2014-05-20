%{

#include "hash.h"
#include "astree.h"
#include "y.tab.h"

int lineNumber = 1;
int running = 1;

void initMe()
{
     hashInit();
}

%}

%x COMM 

digit [0-9]
letter [A-Za-z_]
special [,;:\(\)\[\]\{\}\+\-\*\/<>=!&\$]

%%

"word"                             return KW_WORD;
"bool"                             return KW_BOOL;
"byte"                             return KW_BYTE;
"if"                               return KW_IF;
"then"                             return KW_THEN;
"else"                             return KW_ELSE;
"loop"                             return KW_LOOP;
"input"                            return KW_INPUT;
"output"                           return KW_OUTPUT;
"return"                           return KW_RETURN;

"<="                               return OPERATOR_LE; 
">="                               return OPERATOR_GE;
"=="                               return OPERATOR_EQ;
"!="                               return OPERATOR_NE;
"&&"                               return OPERATOR_AND;
"||"                               return OPERATOR_OR;

"false"                            { yylval.symbol = hashInsert(yytext, LIT_FALSE); return LIT_FALSE; }
"true"                             { yylval.symbol = hashInsert(yytext, LIT_TRUE); return LIT_TRUE; }

{special}                          { return yytext[0]; }
{digit}+                           { yylval.symbol = hashInsert(yytext, LIT_INTEGER); return LIT_INTEGER; }
\"([^\n\"])*\"                     { yylval.symbol = hashInsert(yytext, LIT_STRING); return LIT_STRING; }
\'.\'                              { yylval.symbol = hashInsert(yytext, LIT_CHAR); return LIT_CHAR; }
{letter}({letter}|{digit})*        { yylval.symbol = hashInsert(yytext, TK_IDENTIFIER); return TK_IDENTIFIER; }
{digit}({letter}|{digit})+         { yylval.symbol = hashInsert(yytext, TK_IDENTIFIER); return TK_IDENTIFIER; }

[ \t]
.                                  { return TOKEN_ERROR; }
"\n"                               { ++ lineNumber; }

"//".*	

"/*"                               { BEGIN(COMM); }
<COMM>"*/"                         { BEGIN(INITIAL); }
<COMM>"\n"                         { ++lineNumber; }
<COMM>.

%%

int getLineNumber(void)
{
     return lineNumber;
}

int yywrap()
{
     running = 0;
     return 1;
}
