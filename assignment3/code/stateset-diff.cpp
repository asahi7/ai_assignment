/* 
 * Author: Tsz-Chiu Au
 *
 * Created on April 2, 2015, 2:15 AM
 */

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>

#include <stack>
#include <queue>
#include <utility>
#include <cassert>
#include <ctime>
#include <inttypes.h>
#include <unordered_map>

#define INT_BIT_NUM 32
#define COMPLETE_BIT_MASK 0xffffffff

typedef uint32_t bcode;
typedef uint64_t bstate;

using namespace std;


// -------------------------------------------------------------------------------------

class GameSpec {
    
    int size;
    int sizeX, sizeY;
    bool *board;  //  board[i] is true if the position i is not blocked.
    vector<vector<int> > winseq;

public:
    GameSpec(char *specfilename);
    ~GameSpec() { delete [] board; }
    
    const int getIndex(int x, int y) const { return x + sizeX * y; }
    const int getSize() const { return size; }
    const int getSizeX() const { return sizeX; }
    const int getSizeY() const { return sizeY; }
    const bool isBlocked(int i) const { return !board[i]; }
    const bool isBlocked(int x, int y) const { return !board[x + y * sizeX]; }
    const vector<vector<int> >& getWinSeq() const { return winseq; }
    const bool isValidMove(int move) { return 0 <= move && move < size && board[move]; }
    
    void print();
};

GameSpec::GameSpec(char *specfilename) {
    ifstream f(specfilename);

    if (f.is_open()) {

        f >> sizeX >> sizeY;

        if (f.bad()) throw runtime_error("Failed to read from the specification file.");
        if (sizeX < 1 || sizeY < 1) throw runtime_error("Invalid board size.");

        size = sizeX * sizeY;
        
        board = new bool[size];
        
        for (int y=0; y<sizeY; y++) {
            for (int x=0; x<sizeX; x++) {
                int i = x + y * sizeX;
                int v;
                f >> v;
                if (v >= 0) {
                    if (v != i) throw runtime_error("Incorrect board entry.");
                    board[i] = true;
                } else {
                    board[i] = false;
                }
            }
        }

        while(true) {
            if (f.bad()) throw runtime_error("Unable to read winning sequences.");
            int n;
            f >> n;
            if (n < 0) break;  // no more winning sequences
            winseq.push_back(vector<int>());
            for(int i=0; i<n; i++) {
                int v;
                f >> v;
                if (!board[v]) throw runtime_error("Winning sequences cannot include blocked locations");
                for(int i=0; i<winseq.back().size(); i++) {
                    if (winseq.back()[i] == v)
                        throw runtime_error("Duplicated locations in a winning sequences.");
                }
                winseq.back().push_back(v);
            }
        }

        f.close();
    } else {
        throw runtime_error("Cannot open the specification file.");
    }
}

void GameSpec::print() {
    cout << "Board:"<< endl;
    for (int y=0; y<sizeY; y++) {
        for (int x=0; x<sizeX; x++) {
            if (isBlocked(x, y)) {
                cout << "*";
            } else {
                cout << "#";
            }
        }
        cout << endl;
    }
    cout << "Winning sequences:"<< endl;
    for (int i=0; i<winseq.size(); i++) {
        for(int j=0; j<winseq[i].size(); j++) {
            int k = winseq[i][j];
            cout << "(" << k % sizeX << "," << k / sizeX << ")" << " ";
        }
        cout << endl;
    }
}

// -------------------------------------------------------------------------------------

const bcode bit_table[INT_BIT_NUM] = {
    0x00000001,
    0x00000002,
    0x00000004,
    0x00000008,
    0x00000010,
    0x00000020,
    0x00000040,
    0x00000080,
    0x00000100,
    0x00000200,
    0x00000400,
    0x00000800,
    0x00001000,
    0x00002000,
    0x00004000,
    0x00008000,
    0x00010000,
    0x00020000,
    0x00040000,
    0x00080000,
    0x00100000,
    0x00200000,
    0x00400000,
    0x00800000,
    0x01000000,
    0x02000000,
    0x04000000,
    0x08000000,
    0x10000000,
    0x20000000,
    0x40000000,
    0x80000000
};

int getBitId(bcode bit) {
    int i=-1;
    do {
        bit >>= 1;
        i++;
    } while (bit != 0);
    return i;
}

bcode getPos1(bstate state) { return (COMPLETE_BIT_MASK & state); }

bcode getPos2(bstate state) { return (state >> INT_BIT_NUM); }

bstate getState(bcode pos1, bcode pos2) {
    return static_cast<bstate>(pos1) | (static_cast<bstate>(pos2) << INT_BIT_NUM);
}

bool isFirstPlayerMove(bstate state) {
    bcode code1 = getPos1(state);
    bcode code2 = getPos2(state);
    
    int count1 = 0;
    while(code1>0) {
        count1 += (code1 % 2);
        code1 >>= 1;
    }
    
    int count2 = 0;
    while(code2>0) {
        count2 += (code2 % 2);
        code2 >>= 1;
    }
    
    return (count1 == count2);
}

class WinSeqCode {

    int csize;
    
    int *original2compact;
    int *compact2original;

    int ws_code_num;
    bcode *ws_code;
    
    bcode terminal_mask;
    
public:
    WinSeqCode(GameSpec *spec) {
        csize=0;
        original2compact = new int[spec->getSize()];
        compact2original = new int[spec->getSize()];  // slightly larger that needed
        for(int i=0; i<spec->getSize(); i++) {
            if (!spec->isBlocked(i)) {
                original2compact[i] = csize;
                compact2original[csize] = i;
                csize++;
            } else {
                original2compact[i] = -1;
            }
        }
        
        if (csize > INT_BIT_NUM) throw runtime_error("Too many empty spaces.");
        
        ws_code_num = spec->getWinSeq().size();
        ws_code = new bcode[ws_code_num];
        
        for(int i=0; i<ws_code_num; i++) {
            ws_code[i] = 0;
            for(int j=0; j<spec->getWinSeq()[i].size(); j++) {
                ws_code[i] |= bit_table[original2compact[spec->getWinSeq()[i][j]]];
            }
        }
        
        terminal_mask = 0;
        for(int i=0; i<csize; i++) {
            terminal_mask |= bit_table[i];
        }
    }

    ~WinSeqCode() {
        delete [] compact2original;
        delete [] original2compact;
        delete [] ws_code;
    }
    
    const int getCompactedSize() const { return csize; }
    const int getWinSeqCodeNum() const { return ws_code_num; }
    const bcode getWinSeqCode(int i) const { return ws_code[i]; }
    const int toCompactIndex(int i) const { return original2compact[i]; }
    const int toOriginalIndex(int i) const { return compact2original[i]; }
    const bcode getTerminalMask() const { return terminal_mask; }
    
};

// -------------------------------------------------------------------------------------

class StateSet {
    
    GameSpec *spec;
    WinSeqCode *coding;
    int csize;
    unordered_map<bstate,int> payoff_table;

public:
    
    StateSet(GameSpec *_spec, char *statefilename);
    ~StateSet();

    void printDivider();
    void printState(bstate state);

    int setminus(StateSet &sset, bool isCheckPayoff);
};

StateSet::StateSet(GameSpec *_spec, char *statefilename) : spec(_spec) {
    assert (sizeof(uint32_t) == 4 && "This program only works in system in which the size of uint32_t is 4 bytes.");
    assert (sizeof(uint64_t) == 8 && "This program only works in system in which the size of uint64_t is 8 bytes.");

    coding = new WinSeqCode(spec);
    csize = coding->getCompactedSize();
    
    ifstream f(statefilename);

    string bar = "";
    
    for(int x=0; x < spec->getSizeX(); x++) { bar += "-"; }
   
    if (!f.is_open()) {
        throw runtime_error("Cannot open the specification file.");
    }
        
    while(!f.bad()) {

        char c;
        string s;
        string payoff_string;
        int payoff;
        bool isPruned = false;

        f >> s >> payoff_string;

        if (s == "") break; // end of file

        if (s != bar) {
            throw runtime_error("Incorrect state file format.");
        }

        if (('0' <= payoff_string[0] && payoff_string[0] <= '9') || payoff_string[0] == '-') {
            payoff = atoi(payoff_string.c_str());
        } else {
            isPruned = true;
        }

        getline(f, s); // skip to the end of the line

        int id = 1;
        bcode bcode1 = 0, bcode2 = 0;
        for(int y=0; y < spec->getSizeY(); y++) {
            for(int x=0; x < spec->getSizeX(); x++) {
                f >> c;
                if (c != '#') {
                    if (c == 'O') {
                        bcode1 |= id;
                    } else if (c == 'X') {
                        bcode2 |= id;
                    } else if (c == '*') {
                        // do nothing
                    } else {
                        throw runtime_error("Incorrect state file format.");
                    }
                    id <<= 1;
                }
            }
            getline(f, s); // skip to the end of the line
        }

        bstate state = getState(bcode1, bcode2);

        if (!isPruned) {
            payoff_table[state] = payoff;
        }
    }

    f.close();
}

StateSet::~StateSet() {
    delete coding;
}

void StateSet::printDivider() {
    for(int x=0; x<spec->getSizeX(); x++) cout << "-";
}

void StateSet::printState(bstate state) {
    bcode pos1 = getPos1(state);
    bcode pos2 = getPos2(state);
    for(int y=0; y<spec->getSizeY(); y++) {
        for(int x=0; x<spec->getSizeX(); x++) {
            int i = spec->getIndex(x, y);
            int j = coding->toCompactIndex(i);
            if (j >= 0) {
                if ((pos1 & bit_table[j]) != 0) {
                    cout << "O";
                } else if ((pos2 & bit_table[j]) != 0) {
                    cout << "X";
                } else {
                    cout << "*";
                }
            } else {
                cout << "#";
            }
        }
        cout << endl;
    }
}

int StateSet::setminus(StateSet &sset, bool isCheckPayoff) {
    int diff_count = 0;
    
    for(auto kv : payoff_table) {
        bstate state = kv.first;
        int payoff = kv.second;

        bool isFound = false;
        
        if (sset.payoff_table.count(state) == 0) {
            printDivider();
            cout << " " << payoff << " // state does not exist" << endl;
            printState(state);
            isFound = true;
            
        } else if (isCheckPayoff) {
            int payoff2 = sset.payoff_table[state];
            if (payoff != payoff2) {
                printDivider();
                cout << " " << payoff << " // minimax value is different " << endl;
                printState(state);
                isFound = true;
            }
        }
        
        if (isFound) {
            diff_count++;
        }
    }
    
    return diff_count;
}

// -------------------------------------------------------------------------------------


/*
 * The main function.
 */
int main(int argc, char** argv) {

    bool isCheckPayoff = true;
    
    char *specfilename;
    char *statefile1name;
    char *statefile2name;
    
    if (argc==4) {
        specfilename = argv[1];
        statefile1name = argv[2];
        statefile2name = argv[3];
    } else if (argc==5 && strcmp(argv[1], "-ip")==0) {
        isCheckPayoff = false;
        specfilename = argv[2];
        statefile1name = argv[3];
        statefile2name = argv[4];
    } else {
        cerr << "Error in command line arguments." << endl;
        cerr << endl;
        cerr << "Usage: " << endl;
        cerr << endl;
        cerr << "       " << argv[0] << " [-ip] spec.txt stateset1.txt stateset2.txt" << endl;
        cerr << endl;
        cerr << "where -ip is an option to ignore the payoff when checking the equality of states." << endl;
        cerr << "      spec.txt is the specification of a generalized Tic-Tac-Toe game." << endl;
        cerr << "      stateset1.txt and stateset2.txt are the state files." << endl;
        return 1;
    }

    GameSpec *spec;
    
    try {
       spec = new GameSpec(specfilename);
    } catch(exception const& e) {
        cerr << e.what() << endl;
        cerr << "Invalid specification file." << endl;
        return 2;
    }

    StateSet sset1(spec, statefile1name);
    StateSet sset2(spec, statefile2name);

    int diff_count = sset2.setminus(sset1, isCheckPayoff);

    if (diff_count == 0) {
        cout << "There is no state that exists in the second set but not in the first set";
        if (isCheckPayoff) {
            cout << "." << endl;
        } else {
            cout << " (payoff ignored)." << endl;
        }
    } else {
        cout << endl;
        if (diff_count == 1) {
            cout << "There is one state that exists ";
        } else {
            cout << "There are " << diff_count << " states that exist ";
        }
        cout << "in the second set but not in the first set";
        if (isCheckPayoff) {
            cout << "." << endl;
        } else {
            cout << " (payoff ignored)." << endl;
        }
    }
    
    delete spec;
    
    return 0;
}
