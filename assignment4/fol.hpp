
/* 
 * Author: Tsz-Chiu Au
 *
 * Created on May 9, 2013, 12:00 PM
 */

#ifndef FOL_HPP
#define FOL_HPP

#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum FOL_TYPE {
    TERM,
    VARIABLE,
    CONSTANT,
    FUNCTION,
    PREDICATE
};

class Term {
public:
    virtual ~Term() = 0;

    virtual string getName() const = 0;
    virtual void print(ostream&) const = 0;
    virtual bool equals(Term &) const = 0;
    virtual FOL_TYPE getType() const { return TERM; }

    friend ostream& operator<<(ostream &out, Term &p);
};

inline Term::~Term() { }


class Variable : public Term {
    string name;
public:
    Variable(string _name) : name(_name) { }

    string getName() const { return name; }
    void print(ostream& out) const { out << name; }

    bool equals(Term &term) const;
    FOL_TYPE getType() const { return VARIABLE; }

    friend ostream& operator<<(ostream &out, Variable &p);
};


class Constant : public Term {
    string name;
public:
    Constant(string _name) : name(_name) { }

    string getName() const { return name; }
    void print(ostream& out) const { out << name; }

    bool equals(Term &term) const;
    FOL_TYPE getType() const { return CONSTANT; }

    friend ostream& operator<<(ostream &out, Constant &p);
};


class Function : public Term {
    string name;
    vector<Term*> terms;
public:
    Function(string _name, vector<Term*> &ts);
    ~Function();

    string getName() const { return name; }
    int getTermSize() const { return terms.size(); }
    Term& getTerm(int i) const { return *terms[i]; }
    void print(ostream& out) const;

    bool equals(Term &term) const;
    FOL_TYPE getType() const { return FUNCTION; }

    friend ostream& operator<<(ostream &out, Function &p);
};


class Predicate {
    string name;
    vector<Term*> terms;
public:
    Predicate(string _name, vector<Term*> &ts);
    ~Predicate();

    string getName() const { return name; }
    int getTermSize() const { return terms.size(); }
    Term& getTerm(int i) const { return *terms[i]; }
    void print(ostream& out) const;

    bool equals(Predicate &p) const;
    FOL_TYPE getType() const { return PREDICATE; }

    friend ostream& operator<<(ostream &out, Predicate &p);
};


#endif // FOL_HPP
