
%{ /* -*- C++ -*- */
#include <cstdlib>
#include <cerrno>
#include <climits>
#include <string>
#include "fol-reader.hpp"
#include "fol-parser.tab.hpp"

/* Work around an incompatibility in flex (at least versions
   2.5.31 through 2.5.33): it generates code that does
   not conform to C89.  See Debian bug 333231
   <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.  */
#undef yywrap
#define yywrap() 1

/* By default yylex returns int, we use token_type.
   Unfortunately yyterminate by default returns 0, which is
   not of token_type.  */
#define yyterminate() return token::END
%}

%option noyywrap nounput debug


constant [A-Z][a-zA-Z0-9_]*
variable [a-z0-9][a-zA-Z0-9_]*
blank [ \t]


%{
# define YY_USER_ACTION  yylloc->columns (yyleng);
%}

%%
%{
  yylloc->step ();
%}

{blank}+   yylloc->step ();

%{
  typedef yy::fol_parser::token token;
%}

\n      { yylloc->lines (yyleng); yylloc->step (); return token::END; }

         /* Convert ints to the actual type of tokens.  */
[\(\)\,]   return yy::fol_parser::token_type(yytext[0]);

{constant} {
           yylval->sval = new string(yytext);
           return token::CONSTANT;
         }

{variable} {
           yylval->sval = new string(yytext);
           return token::VARIABLE;
         }

.        reader.error(*yylloc, "invalid character");


%%


void fol_reader::scan_begin() {
  yy_flex_debug = trace_scanning;
  if (filename.empty() || filename == "-")
    yyin = stdin;
  else if (!(yyin = fopen(filename.c_str(), "r")))
    {
      error("cannot open " + filename + ": " + strerror(errno));
      exit(EXIT_FAILURE);
    }
}

void fol_reader::scan_end() {
  if (! (filename.empty() || filename == "-")) fclose(yyin);
}
