
/* 
 * Author: Tsz-Chiu Au
 *
 * Created on May 9, 2013, 12:00 PM
 */

#ifndef FOL_READER_HPP
#define FOL_READER_HPP

#include <string>
#include <map>
#include "fol-parser.tab.hpp"

#include "fol.hpp"

using namespace std;

// Tell Flex the lexer's prototype ...
#define YY_DECL                                        \
  yy::fol_parser::token_type                         \
  yylex (yy::fol_parser::semantic_type* yylval,      \
         yy::fol_parser::location_type* yylloc,      \
         fol_reader& reader)

// ... and declare it for the parser's sake.
YY_DECL;

class fol_reader {

    string filename;
    bool trace_scanning;
    bool trace_parsing;

    Predicate *predicate;

public:

    fol_reader();

    virtual ~fol_reader();

    // Get/set the predicate

    void setPredicate(Predicate *p) { predicate = p; }
    Predicate &getPredicate() { return *predicate; }

    // Get file name
    string& getFilename() { return filename; }

    // Handling the scanner.
    void scan_begin();
    void scan_end();

    // Run the parser.  Return NULL on failure
    Predicate *read();
    Predicate *read_from_file(const string& f);

    // Error handling.
    void error(const yy::location& l, const string& m);
    void error(const string& m);
};

#endif // ! FOL_READER_HPP
