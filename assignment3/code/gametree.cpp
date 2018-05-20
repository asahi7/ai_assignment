// Aibek Smagulov, 20142028, smagulovaybek@gmail.com

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>

using namespace std;

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

class GameTree {
    
    GameSpec *spec;
    const int inf = 1000000000;

public:
    GameTree(GameSpec *spec);
    ~GameTree();
    void playAsPlayer1();
    void playAsPlayer2();
    int getRootMiniMaxValue();
    void print();
    void printState(int pos[], bool pruned, int player);
    pair<int, int> getMinimaxTurn(int pos[], int player, int alpha, int beta, bool isVerbose);
    int goalTest(int pos[]);
};

int GameTree::goalTest(int pos[]){
    vector<vector<int> > winseq = spec->getWinSeq();
    for(int i = 0; i < winseq.size(); i++){
        int winner = pos[winseq[i].back()];
        bool bad = 0;
        for(int j = 0; j < winseq[i].size(); j++){
           int k = winseq[i][j];
           if(pos[k] != winner){
                bad = 1;
                break;
           }
        }
        if(bad == 0){
            return winner;
        }
    }
    bool full = 1;
    for(int i = 0; i < spec->getSize(); i++) 
        if(! spec->isBlocked(i) && pos[i] == 0) full = 0;
    return (full == 1 ? 10 : 0);
}

void GameTree::printState(int pos[], bool pruned, int player){
    for(int i = 0; i < spec->getSizeX(); i++) cout << "-";
    if(pruned){
        cout << " pruned" << endl;
    }
    else {
        int pos2[spec->getSize()];
        for(int i = 0; i < spec->getSize(); i++) pos2[i] = pos[i];
        pair<int, int> res = getMinimaxTurn(pos2, player, -inf, inf, false);
        cout << " " << res.first;
        int terminal = goalTest(pos);
        if(terminal == 1) cout << " WIN";
        else if(terminal == -1) cout << " LOSE";
        else if(terminal == 10) cout << " DRAW";
        cout << endl;
    }
    for(int i = 0; i < spec->getSizeY(); i++){
         for(int j = 0; j < spec->getSizeX(); j++){
             if(spec->isBlocked(i, j) == false){
                 //cout << (pos[i * spec->getSizeY() + j] == 1 ? 'O' : 'X');
                int e = pos[i * spec->getSizeY() + j];
                if(e == 1){
                    cout << 'O';
                }
                else if(e == -1){
                    cout << 'X';
                }
                else cout << "*";
             }
             else{
                 cout << '#';
             }
         }
         cout << endl;
    }
} 

pair<int, int> GameTree::getMinimaxTurn(int pos[], int player, int alpha, int beta, bool isVerbose = false){
    int terminal = goalTest(pos);
    if(isVerbose == true) printState(pos, 0, player);
    if(terminal != 0){
        if(terminal == 1) return make_pair(1, -1);
        else if(terminal == -1) {
            return make_pair(-1, -1);
        }
        else if(terminal == 10) return make_pair(0, -1);
    }
    if(player == 1){
        pair<int, int> best = make_pair(-inf, -1);
        for(int i = 0; i < spec->getSize(); i++){
            if(! spec->isBlocked(i) && pos[i] == 0){
                pos[i] = 1;
                pair<int, int> res = getMinimaxTurn(pos, 2, alpha, beta, isVerbose);
                if(res.first > best.first){
                    best.first = res.first;
                    best.second = i;
                }
                if(best.first >= beta) {
                    if(isVerbose == true) printState(pos, 1, player);
                    pos[i] = 0;
                    return best;
                }
                pos[i] = 0;
                alpha = max(alpha, best.first);
            }
        }
        return best;
    }
    else if(player == 2){
        pair<int, int> best = make_pair(inf, -1);
        for(int i = 0; i < spec->getSize(); i++){
            if(! spec->isBlocked(i) && pos[i] == 0){
                pos[i] = -1;
                pair<int, int> res = getMinimaxTurn(pos, 1, alpha, beta, isVerbose);
                if(res.first < best.first){
                    best.first = res.first;
                    best.second = i;
                }
                if(best.first <= alpha) {
                    if(isVerbose == true) printState(pos, 1, player);
                    pos[i] = 0;
                    return best;
                }
                pos[i] = 0;
                beta = min(beta, best.first);
            }
        }
        return best;
    }
    return make_pair(0, -1);
}

GameTree::GameTree(GameSpec *_spec) : spec(_spec) {
}

GameTree::~GameTree() {
}

void GameTree::playAsPlayer1() {
    int terminal;
    int pos[spec->getSize()];
    for(int i = 0; i < spec->getSize(); i++) pos[i] = 0;
    while(true){
         pair<int, int> res = getMinimaxTurn(pos, 1, -inf, inf);
         pos[res.second] = 1;
         cout << res.second << endl;
         int player2;
         cin >> player2;
         pos[player2] = -1;
         if(goalTest(pos) != 0) break;
    }
}

void GameTree::playAsPlayer2() {
    int terminal;
    int pos[spec->getSize()];
    for(int i = 0; i < spec->getSize(); i++) pos[i] = 0;
    while(true){
         int player1;
         cin >> player1;
         pos[player1] = 1;
         pair<int, int> res = getMinimaxTurn(pos, 2, -inf, inf);
         pos[res.second] = -1;
         cout << res.second << endl;
         if(goalTest(pos) != 0) break;
   }
}

int GameTree::getRootMiniMaxValue() {
    int pos[spec->getSize()]; 
    for(int i = 0; i < spec->getSize(); i++) pos[i] = 0;
    pair<int, int> res = getMinimaxTurn(pos, 1, -inf, inf);
    if(res.first == inf) return 0;
    else if(res.first == -inf) return 0;
    else return res.first;
}

void GameTree::print(){
    int pos[spec->getSize()];
    for(int i = 0; i < spec->getSize(); i++) pos[i] = 0;
    getMinimaxTurn(pos, 1, -inf, inf, true);
}

/*
 * The main function.
 */
int main(int argc, char** argv) {

    char *specfilename;
    bool isVerbose = false, isArgValid = true;
    bool isPlaying = false, isFirstPlayer = false;

    if (argc==2) {
        specfilename = argv[1];
    } else if (argc==3) {
        if (strcmp(argv[1], "-v")==0) {
            isVerbose = true;
        } else if (strcmp(argv[1], "-p1")==0) {
            isPlaying = true;
            isFirstPlayer = true;
        } else if (strcmp(argv[1], "-p2")==0) {
            isPlaying = true;
            isFirstPlayer = false;
        } else {
            isArgValid = false;
        }
        specfilename = argv[2];
    } else {
        isArgValid = false;
    }
    
    if (!isArgValid) {
        cerr << "Error in command line arguments." << endl;
        cerr << endl;
        cerr << "Usage: " << endl;
        cerr << endl;
        cerr << "       " << argv[0] << " [-v|-p1|-p2] spec.txt" << endl;
        cerr << endl;
        cerr << "where spec.txt is the specification of a generalized Tic-Tac-Toe game." << endl;
        cerr << "The option -v enables the output of the intermediate states of the" << endl;
        cerr << "game tree search procedure." << endl;
        cerr << "The option -p1 causes the program to start playing as the first player." << endl;
        cerr << "The option -p2 causes the program to start playing as the second player." << endl;
        return 1;
    }

    GameSpec *spec;
    try {
       spec = new GameSpec(specfilename);
       // spec.print();
    } catch(exception const& e) {
        cerr << e.what() << endl;
        cerr << "Invalid specification file." << endl;
        return 2;
    }
    
    GameTree *game_tree;
    try {
        game_tree = new GameTree(spec);
    } catch(exception const& e) {
        cerr << e.what() << endl;
        cerr << "Failed to create the game tree." << endl;
        return 3;
    }
    
    if (isVerbose) {
        game_tree->print();
    } else if (isPlaying) {
        if (isFirstPlayer) {
            game_tree->playAsPlayer1();
        } else {
            game_tree->playAsPlayer2();
        }
    } else {
        int v = game_tree->getRootMiniMaxValue();
        if (v == 0) {
            cout << "This is a balanced game." << endl;
        } else if (v > 0) {
            cout << "This game favors the first player." << endl;
        } else {  // v < 0
            cout << "This game favors the second player." << endl;
        }
    }
    
    delete spec;
    delete game_tree;
    
    return 0;
}
