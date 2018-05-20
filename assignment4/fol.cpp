
/* 
 * Author: Tsz-Chiu Au
 *
 * Created on May 9, 2013, 12:00 PM
 */

#include <iostream>
#include <string>
#include <vector>
#include <typeinfo>

#include "fol.hpp"

Function::Function(string _name, vector<Term*> &ts) : name(_name) {
    for(int i=0; i<ts.size(); i++) {
        terms.push_back(ts[i]);
    }
}

Function::~Function() {
    for(int i=0; i<terms.size(); i++) {
        delete terms[i];
    }
}

void Function::print(ostream& out) const {
    out << name << "(";
    for(int i=0; i<terms.size(); i++) {
        if (i != 0) out << ", ";
        terms[i]->print(out);
    }
    out << ")";
}

Predicate::Predicate(string _name, vector<Term*> &ts) : name(_name) {
    for(int i=0; i<ts.size(); i++) {
        terms.push_back(ts[i]);
    }
}

Predicate::~Predicate() {
    for(int i=0; i<terms.size(); i++) {
        delete terms[i];
    }
}

void Predicate::print(ostream& out) const {
    out << name << "(";
    for(int i=0; i<terms.size(); i++) {
        if (i != 0) out << ", ";
        terms[i]->print(out);
    }
    out << ")";
}

bool Variable::equals(Term &term) const {
    try {
        Variable &v = dynamic_cast<Variable &>(term);
        return name == v.name;
    } catch (bad_cast) {
        return false;
    }
}

bool Constant::equals(Term &term) const {
    try {
        Constant &c = dynamic_cast<Constant &>(term);
        return name == c.name;
    } catch (bad_cast) {
        return false;
    }
}


bool Function::equals(Term &term) const {
    try {
        Function &f = dynamic_cast<Function &>(term);

        if (name != f.name) return false;

        for(int i=0; i<terms.size(); i++) {
            if ( !terms[i]->equals(*f.terms[i]) ) return false;
        }

        return true;
    } catch (bad_cast) {
        return false;
    }
    
}

bool Predicate::equals(Predicate &p) const {
    if (name != p.name) return false;

    for(int i=0; i<terms.size(); i++) {
        if ( !terms[i]->equals(*p.terms[i]) ) return false;
    }

    return true;
}

ostream& operator<< (ostream &out, Term &p) {
    p.print(out);
    return out;
}

ostream& operator<< (ostream &out, Variable &v) {
    v.print(out);
    return out;
}

ostream& operator<< (ostream &out, Constant &c) {
    c.print(out);
    return out;
}

ostream& operator<< (ostream &out, Function &f) {
    f.print(out);
    return out;
}

ostream& operator<< (ostream &out, Predicate &p) {
    p.print(out);
    return out;
}
