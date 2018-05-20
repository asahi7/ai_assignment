
/* 
 * Author: Tsz-Chiu Au
 *
 * Created on March 23, 2013, 2:38 AM
 */

#include <iostream>
#include <string>

using namespace std;

#define MAX_SIZE 1000000

/*
 * The main function.
 */
int main(int argc, char** argv) {

    string str;
    
    cin >> str;
    
    int size = str.length();
    
    cout << "This is the " << size << "-queens problem." << endl;
   
    int *num_x = new int[size];
    int *num_y = new int[size];
    int *num_xy = new int[size * 2 - 1];  // (x+y)
    int *num_yx = new int[size * 2 - 1];  // (y-x) + (size-1)
    
    for(int i=0; i<size; i++) {
        num_x[i] = 0;
        num_y[i] = 0;
    }
    
    for(int i=0; i < 2*size-1; i++) {
        num_xy[i] = 0;
        num_yx[i] = 0;
    }
    
    int y = 0;
    while(true) {
        for(int x=0; x<size; x++) {
            if (str[x] == 'Q') {
                
                int xy = x + y;
                int yx = (y-x) + (size-1);
                
                if (num_x[x] == 0) {
                    num_x[x] = 1;
                } else {
                    cout << "The queen at " << "(" << x << "," << y << ")";
                    cout << " is in conflict with another queen in the vertical direction." << endl;
                    return 1;
                }
                if (num_y[y] == 0) {
                    num_y[y] = 1;
                } else {
                    cout << "The queen at " << "(" << x << "," << y << ")";
                    cout << " is in conflict with another queen in the horizontal direction." << endl;
                    return 2;
                }
                if (num_xy[xy] == 0) {
                    num_xy[xy] = 1;
                } else {
                    cout << "The queen at " << "(" << x << "," << y << ")";
                    cout << " is in conflict with another queen in the negative diagonal direction." << endl;
                    return 3;
                }
                if (num_yx[yx] == 0) {
                    num_yx[yx] = 1;
                } else {
                    cout << "The queen at " << "(" << x << "," << y << ")";
                    cout << " is in conflict with another queen in the positive diagonal direction." << endl;
                    return 4;
                }
            }
        }
        
        if (y == size-1) break;
        
        cin >> str; // get next line
        y++;
    }

    for(int i=0; i<size; i++) {
        if (num_x[i] == 0) {
            cout << "There is no queen on column " << i << "." << endl;
            return 5;
        }
        if (num_y[i] == 0) {
            cout << "There is no queen on row " << i << "." << endl;
            return 6;
        }
    }

    delete [] num_x;
    delete [] num_y;
    delete [] num_xy;
    delete [] num_yx;
    
    cout << "The solution is correct." << endl;
    return 0;
}
