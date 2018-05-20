/**
   Assignment 4: Unification
   Name: Aibek Smagulov
   Student_id: 20142028
   Email: smagulovaybek@gmail.com
*/


#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <map>
#include <set>
#include <utility>
#include <stdexcept>

#include "fol.hpp"
#include "fol-reader.hpp"

using namespace std;

struct Variable_Ptr_Comparer {
  bool operator() (const Variable* x, const Variable* y) const {
      return x->getName().compare(y->getName()) < 0;
  }
};

typedef map<Variable*,Term*,Variable_Ptr_Comparer> Substitute;
typedef map<Variable*, int, Variable_Ptr_Comparer> CntMap;

/**
  Algorithm for deriving or finding value for the variable var, does occur checking,
  finds existing variable from the set theta or puts new value for the variable

  @param theta is a set for containing variables and their respective values
  @param var is variable
  @param x is value or term
*/
bool unify_var(Substitute &theta, Variable& var, Term &x);
/**
  Unification algorithm described as in textbook

  @param theta is a set for containing variables and their respective values
  @param x is first term
  @param y is second term
*/
bool unify_term(Substitute &theta, Term& x, Term& y);
/**
  Adds variable var with value x to the set theta
  
  @param theta is a set for containing variables and their respective values
  @param var is variable 
  @param x is value
*/
void addToTheta(Substitute& theta, Variable& var, Term& x);
/**
  Checks recursive cycles of type x := S(x) in the set

  @param theta is a set for containing variables and their respective values
  @param var is variable
  @param x is value
*/
bool occurCheck(Substitute& theta, Variable& var, Term& x);
/**
  Given two predicates, evaluates each of the terms

  @param theta is a set for containing variables and their respective values
  @param x is first predicatee
  @param x is second predicate
*/
void unify(Substitute& theta, Predicate& x, Predicate& y);
/**
  Counts number of variables with same names and stores count in the map

  @param mp - map for storing variables names with respective counterss
  @param x is term
*/
void count_variables(CntMap& mp, Term& x);
/**
  Replaces variables making their names unique by appending '_n', where n is 
  counter

  @param mp - map for storing variables names with respective counters
  @param x is term
*/
Term& replace_vars(CntMap& mp, Term& x);
/**
  Given two predicates, implements standardizing algorithm   

  @param p1, first predicatee
  @param p2, second predicate
*/
pair<Predicate*,Predicate*> standandizing_apart(Predicate *p1, Predicate *p2);
 

void addToTheta(Substitute& theta, Variable& var, Term& x){
    theta[&var] = &x;
}

bool occurCheck(Substitute& theta, Variable& var, Term& x){
  try{  
    if(var.equals(x) == true) return true;
    else if(x.getType() == FUNCTION){
        Function &f = dynamic_cast<Function&>(x);
        for(int i = 0; i < f.getTermSize(); i++){
            if(occurCheck(theta, var, f.getTerm(i)) == true) return true;
        }
    }
    else if(x.getType() == VARIABLE && theta.count(dynamic_cast<Variable*>(&x)) > 0){  
        return occurCheck(theta, var, *(theta[dynamic_cast<Variable*>(&x)]));
    }
  } catch(bad_cast& bc){ 
    cout << bc.what() << endl;
    return false;
  }
  return false;
}

bool unify_var(Substitute &theta, Variable &var, Term &x){
 try{
    if(theta.count(&var) > 0){
        return unify_term(theta, *(theta[&var]), x);
    }
    else if(x.getType() == VARIABLE && theta.count((dynamic_cast<Variable*>(&x))) > 0){ 
        return unify_term(theta, var, *(theta[dynamic_cast<Variable*>(&x)])); 
    }
    else if(occurCheck(theta, var, x)){
        throw runtime_error("Occur check failed.");
    }
    else{
        addToTheta(theta, var, x); 
    }
 } catch(bad_cast& bc){
    cout << bc.what() << endl;
 }
 return true;
}

bool unify_term(Substitute& theta, Term &x, Term &y) {
    if(x.equals(y) == true) return true; 
    else if(x.getType() == VARIABLE){
        return unify_var(theta, dynamic_cast<Variable&>(x), y);
    }
    else if(y.getType() == VARIABLE){
        return unify_var(theta, dynamic_cast<Variable&>(y), x);
    }
    else if((x.getType() == FUNCTION && y.getType() == FUNCTION) || (x.getType() == PREDICATE && y.getType() == PREDICATE)){
        Function &fx = dynamic_cast<Function&>(x);
        Function &fy = dynamic_cast<Function&>(y);
        if(fx.equals(fy) == true){
            return true;
        }
        if(fx.getTermSize() != fy.getTermSize()){ 
            throw runtime_error("Different function arguments' lengths!");
        }
        else if(x.getName() != y.getName()){
            throw runtime_error("Different function names.");
        }
        else{
            for(int i = 0; i < fx.getTermSize(); i++){
                if(fx.getTerm(i).equals(fy.getTerm(i)) == false){ 
                    if(unify_term(theta, fx.getTerm(i), fy.getTerm(i)) == false){
                        throw runtime_error("Unable to unify terms.");
                    }
                }
            }
        }
    }
    else{
        throw runtime_error("Unable to unify terms.");
    }
    return true;
}

void unify(Substitute& theta, Predicate& x, Predicate& y){
    if(x.getTermSize() != y.getTermSize()){
        throw runtime_error("Predicate argument number mismatched.");  
    }
    if(x.getName() != y.getName()){
        throw runtime_error("Different predicate names."); 
    }
    for(int i = 0; i < x.getTermSize(); i++){
        if(x.equals(y) == false){
            if(unify_term(theta, x.getTerm(i), y.getTerm(i)) == false) throw runtime_error("Unable to unify terms.");
        }
    }
}

void count_variables(CntMap& mp, Term& x){
    if(x.getType() == FUNCTION){
        Function &f = dynamic_cast<Function&>(x);
        for(int i = 0; i < f.getTermSize(); i++){
            count_variables(mp, f.getTerm(i));
        }
    }
    else if(x.getType() == VARIABLE){
        Variable &var = dynamic_cast<Variable&>(x);
        mp[&var]++;
    }
}

Term& replace_vars(CntMap& mp, Term& x){
     if(x.getType() == FUNCTION){
        Function &f = dynamic_cast<Function&>(x);
        Function* nf;
        vector<Term*> t;
        for(int i = 0; i < f.getTermSize(); i++){
            t.push_back(&(replace_vars(mp, f.getTerm(i))));
        }
        nf = new Function(f.getName(), t);
        return *nf;
    }
    else if(x.getType() == VARIABLE){
        Variable &var = dynamic_cast<Variable&>(x);
        Variable* nv;
        if(mp[&var] - 1 > 0){
            stringstream ss;
            ss << (mp[&var] - 1);
            nv = new Variable(x.getName() + "_" + ss.str());
            mp[&var]--;
        }
        else nv = new Variable(x.getName());
        return *nv;
    }  
    else if(x.getType() == CONSTANT){
        Constant* nc;
        nc = new Constant(x.getName());
        return *nc;
    }
}

pair<Predicate*,Predicate*> standandizing_apart(Predicate *p1, Predicate *p2) {
    CntMap mp;
    for(int i = 0; i < p1->getTermSize(); i++){
        count_variables(mp, p1->getTerm(i));
    }
    for(int i = 0; i < p2->getTermSize(); i++){
        count_variables(mp, p2->getTerm(i));
    }
    Predicate *n1, *n2;
    vector<Term*> ts1, ts2;
    for(int i = 0; i < p1->getTermSize(); i++){
        Term &t = replace_vars(mp, p1->getTerm(i));
        ts1.push_back(&t);
    }
    n1 = new Predicate(p1->getName(), ts1); 
    for(int i = 0; i < p2->getTermSize(); i++){
        Term &t = replace_vars(mp, p2->getTerm(i));
        ts2.push_back(&t);
    }
    n2 = new Predicate(p2->getName(), ts2);
    return make_pair(n1, n2);
}


/*
 * Printing
 */
void print(Term &x, Substitute &theta) {
    if (x.getType() == VARIABLE && theta.count(dynamic_cast<Variable*>(&x)) > 0) {
        print(*theta[dynamic_cast<Variable*>(&x)], theta);
    } else if (x.getType() == FUNCTION) {
        Function& f = dynamic_cast<Function&>(x);
        cout << f.getName() << "(";
        for(int i=0; i<f.getTermSize(); i++) {
            if (i != 0) cout << ", ";
            print(f.getTerm(i), theta);
        }
        cout << ")";
    } else {
        cout << x;
    }
}

/*
 * The main function.
 */
int main(int argc, char **argv) {

    bool isArgValid = true;
    bool isStandardizingApart = false;

    if (argc==2) {
        if (strcmp(argv[1], "-s")==0) {
           isStandardizingApart = true;
        } else {
            isArgValid = false;
        }
    } else if (argc > 2) {
        isArgValid = false;
    }

    if (!isArgValid) {
        cerr << "Error in command line arguments." << endl;
        cerr << endl;
        cerr << "Usage: " << endl;
        cerr << "       " << argv[0] << " [-s]" << endl;
        cerr << endl;
        cerr << "The option -s enables standardizing apart." << endl;
        return 1;
    }

    fol_reader reader;

    cout << "Please enter the first predicate: ";
    Predicate *p1 = reader.read();
    if (!p1) { 
        cerr << "Error when reading from STDIN." << endl;
        return 2;
    }

    cout << "Please enter the second predicate: ";
    Predicate *p2 = reader.read();
    if (!p1) { 
        cerr << "Error when reading from STDIN." << endl;
        return 3;
    }

    if (isStandardizingApart) {
        pair<Predicate*,Predicate*> pair = standandizing_apart(p1, p2);
        delete p1;
        delete p2;
        p1 = pair.first;
        p2 = pair.second;
        cout << "After standardizing apart, the predicates are:" << endl;
        cout << "    " << *p1 << endl;
        cout << "    " << *p2 << endl;
    }

    Substitute theta;

    try {
        unify(theta, *p1, *p2);
    } catch(const runtime_error& e) {
        cout << "Unification failed: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }

    cout << "The substitution is:" << endl;
    for(Substitute::iterator iter = theta.begin(); iter != theta.end(); ++iter) {
        cout << "    " << *(iter->first) << " := ";
        print(*iter->second, theta);
        cout << endl;
    }

    delete p1;
    delete p2;
}
