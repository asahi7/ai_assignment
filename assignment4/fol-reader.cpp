
/* 
 * Author: Tsz-Chiu Au
 *
 * Created on May 9, 2013, 12:00 PM
 */

#include "fol-reader.hpp"
#include "fol-parser.tab.hpp"
#include "fol.hpp"

fol_reader::fol_reader()
  : trace_scanning(false), trace_parsing(false), predicate(NULL)
{
}

fol_reader::~fol_reader() {
}

Predicate *fol_reader::read() {
    return read_from_file("-");
}

Predicate *fol_reader::read_from_file(const string &f) {
    filename = f;
    scan_begin();
    yy::fol_parser parser(*this);
    parser.set_debug_level(trace_parsing);
    int res = parser.parse();
    scan_end();
    if (res == 0)
        return predicate;
    else
        return NULL;
}

void fol_reader::error(const yy::location& l, const string& m) {
    cerr << l << ": " << m << endl;
}

void fol_reader::error(const string& m) {
    cerr << m << endl;
}
