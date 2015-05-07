#include <iostream>
#include <climits>

using namespace std;

int doMove( int m, int A[], int n ) {
	A[m-1]++;
	A[m]++;
	A[m+1]++;
}

int undoMove( int m, int A[], int n ) {
	A[m-1]--;
	A[m]--;
	A[m+1]--;
}

int recursiveWin( int A[], int n, int& move) {
	int result = -1;
	for( int i = 0; i < n; i++ ) {
		if( A[i] == 0 ) {
			doMove( i, A, n );
			result = -recursiveWin( A, n, move );
			undoMove( i, A, n ); 
			if( result > 0 ) {
				move = i;
				return result;
			}
		}
	}
	return result;
}

int recursiveLose( int A[], int n, int& move) {
	int result = 1;
	for( int i = 0; i < n; i++ ) {
		if( A[i] == 0 ) {
			doMove( i, A, n );
			result = -recursiveLose( A, n, move );
			undoMove( i, A, n ); 
			if( result > 0 ) {
				move = i;
				return result;
			}
		}
	}
	return result;
}

int main() {
	int n = 100;
	int B[n+2];
	int* A = B+1;
	int result = 0;
	int move = 0;
	for( int i = 0; i < n; i++ )
		A[i] = 0;

	for( int i = 0; i < n; i++ ) {
		cout << i << ":\n  win: " << recursiveWin( A, i, move );
		cout << "(" << move << ")" << endl;
		cout << "  lose: " << recursiveLose( A, i, move );
		cout << "(" << move << ")" << endl;
	}
}