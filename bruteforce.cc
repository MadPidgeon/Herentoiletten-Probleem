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

int recursive( int A[], int n, int& move) {
	int result;
	for( int i = 0; i < n; i++ ) {
		if( A[i] == 0 ) {
			doMove( i, A, n );
			result = -recursive( A, n, move );
			undoMove( i, A, n ); 
			if( result > 0 ) {
				move = i;
				return result;
			}
		}
	}
	return -1;
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
		result = recursive( A, i, move ) > 0;
		cout << i << " : " << result << "(" << move << ")" << endl;
	}
}