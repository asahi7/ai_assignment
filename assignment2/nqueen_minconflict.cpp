
// Aibek Smagulov, 20142028, smagulovaybek@gmail.com


#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cmath>

using namespace std;

/*
 * The n-queens problem solver
 * 
 * size      - the number of queens
 * isVerbose - whether the intermediate nodes are displayed
 */

int n, a[5555], b[5555], ans, calc, l[111111], r[111111];
double t;

int f()
{
	int ans = 0;
	for(int i = 1;i <= n; i++){
		ans += (++r[i + b[i]]) > 1;
		ans += (++l[i - b[i] + n]) > 1;
    }
	for(int i = 1; i <= n; i++)
		r[i + b[i]] = l[i - b[i] + n] = 0;
	return ans;
}

void solve_nqueen(int size, bool isVerbose) {
	n = size;
    for(int i = 1; i <= n; i++)
		a[i] = b[i] = i;
	ans = f();
    cout << ans << endl;
	srand(time(0));
	t = 20;
	while(ans > 0){
		for(int i = 1; i <= n; i++)
			b[i] = a[i];
		for(int i = 1; i <= t + 1; i++)
			swap(b[rand() % n + 1], b[rand() % n + 1]);
		calc = f();
		if(ans > calc || exp((ans - calc) / t) >= (rand() % 1001) / 1000.0){
			ans = calc;
			for(int i = 1; i <= n; i++)
				a[i] = b[i];
		}
		t *= 0.99;
	}
	for(int i = 1; i <= n; i++){
		for(int j = 1; j <= n; j++){
			if(a[i] == j)
                cout << 'Q';
			else
				cout << '*';
        }
		cout << endl;
	}

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
