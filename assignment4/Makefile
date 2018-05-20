
all: unify

fol-parser.tab.cpp fol-parser.tab.hpp location.hh position.hh stack.hh: fol-parser.ypp fol.hpp
	bison fol-parser.ypp

fol-scanner.cpp: fol-scanner.ll
	flex -o fol-scanner.cpp fol-scanner.ll

unify: fol-parser.tab.cpp fol-scanner.cpp fol-reader.cpp fol.hpp fol.cpp unify.cpp
	g++ -O3 fol-parser.tab.cpp fol-scanner.cpp fol-reader.cpp fol.cpp unify.cpp -o unify

clean:
	rm -f fol-parser.tab.cpp fol-parser.tab.hpp location.hh position.hh stack.hh fol-scanner.cpp unify *.o
