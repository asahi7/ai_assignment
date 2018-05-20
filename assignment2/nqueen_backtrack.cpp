// Aibek Smagulov, 20142028, smagulovaybek@gmail.com


#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

/*
 * The n-queens problem solver
 * 
 * size      - the number of queens
 * isVerbose - whether the intermediate nodes are displayed
 */

void printMatrix(bool a[][100], int size){
    for(int i = 1; i <= size; i++){
        for(int j = 1; j <= size; j++){
            cout << (a[i][j] == true ? 'Q' : '*');
        }
        cout << endl;
    }
}

int cnt = 0;

bool go(int k, bool a[][100], bool row[], bool col[], bool dm[], bool dp[], int size, bool isVerbose){
    if(k == size){
    /*    for(int i = 1; i <= size; i++){
            for(int j = 1; j <= size; j++){
                cout << (a[i][j] == true ? 'Q' : '*');
            }
            cout << endl;
        }*/
        return true;
    }

    if(isVerbose == true && k > 0){
        for(int i = 1; i <= size; i++) cout << '-';
        cout << " " << cnt << endl; 
        for(int i = 1; i <= size; i++){
            for(int j = 1; j <= size; j++){
                 cout << (a[i][j] == true ? 'Q' : '*');
            }
            cout << endl;
         }
        cnt++;
    }
    
    for(int i = 1; i <= size; i++){
        if(row[i] == true) continue;
        for(int j = 1; j <= size; j++){
            if(a[i][j] == true || col[j] == true || dp[i + j] == true || dm[i - j + size + 1] == true) continue;
            row[i] = true;
            col[j] = true;
            dp[i + j] = dm[i - j + size + 1] = true;
            a[i][j] = true;
            bool ok = go(k + 1, a, row, col, dm, dp, size, isVerbose);
            if(ok == true) return true;
            row[i] = col[j] = false;
            dp[i + j] = dm[i - j + size + 1] = false;
            a[i][j] = false;
        }
    }
    return false;
}

void solve_nqueen(int size, bool isVerbose) {

    bool a[size + 1][100];
    bool row[size + 1], col[size + 1];
    bool dm[size * 2 + 1], dp[size * 2 + 1];
    bool trace[size + 1][100];
    
    for(int i = 1; i <= size; i++) row[i] = col[i] = false;
    for(int i = 1; i <= size * 2; i++) dm[i] = dp[i] = false;
    for(int i = 1; i <= size; i++){
        for(int j = 1; j <= size; j++){
            a[i][j] = trace[i][j] = false;
        }
    }

    bool sol = go(0, a, row, col, dm, dp, size, isVerbose);
    if(sol == true){
        if(isVerbose == true){
            for(int i = 1; i <= size; i++){
                cout << '-';
            }
            cout << " Solution" << endl;
        }
        printMatrix(a, size);
        /*else{
            for(int r = 1; r <= size; r++){
               for(int j = 1; j <= size; j++){
                    cout << '-';
                    trace[r][j] = a[r][j];
               } 
               cout << " " << r - 1;
               printMatrix(trace, size);
            }
        }*/
    }
    else{
        cout << "No solution." << endl;
    }
    //cout << sol << endl;
}

/*
 * The main function.
 */
int main(int argc, char** argv) {

    int size;
    bool isVerbose = false, isArgValid = true;

    if (argc==2) {
        size = atoi(argv[1]);
        isArgValid = (size>0);
    } else if (argc==3) {
        if (strcmp(argv[1], "-v")==0) {
            isVerbose = true;
            size = atoi(argv[2]);
            isArgValid = (size>0);
        } else {
            isArgValid = false;
        }
    } else {
        isArgValid = false;
    }

    if (!isArgValid) {
        cout << "Error in command line arguments." << endl;
        cout << "Usage: " << argv[0] << " [-v] n" << endl;
        cout << "where n is the number of queens and the size of the board (nxn)." << endl;
        cout << "The option -v enables the output of the intermediate states of" << endl;
        cout << "the search procedure." << endl;
        return 1;
    }

    solve_nqueen(size, isVerbose);
    
    return 0;
}
