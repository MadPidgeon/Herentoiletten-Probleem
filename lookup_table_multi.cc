#include <iostream>
#include <algorithm>
#include <functional>
#include <fstream>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

#define PILE_LENGTH_MAX 40

// Datastructure in which the state of the game is saved
struct Board {
	int*  subboard;
	void print( ostream& );
	int  cut( int i );
	int  find( int i );
	int  add( int i );
	void serialize();
	void init();
	void deinit();
};

// Removes a subboard with index i from the board
// Preserves canonical representation
int Board::cut( int i ) {
	int temp = subboard[i];
	while( subboard[i] != 0 ) {
		subboard[i] = subboard[i+1];
		i++;
	}
	return temp;
}

// Finds the index of a subboard of given size s
int Board::find( int s ) {
	int j = 0;
	while( subboard[j] > s ) j++;
	if( subboard[j] == s )
		return j;
	else
		return -1;
}

// Adds a subboard of size s to the board
// Preserves canonical representation
int Board::add( int s ) {
	int j = 0;
	while( subboard[j] != 0 ) j++;
	while( subboard[j] < s && j >= 0 ) {
		subboard[j] = subboard[j-1];
		j--;
	}
	subboard[j+1] = s;
	return j+1;
}

// Converts the current board to its canonical representation
// Canonical representation is descending order of subboard sizes
void Board::serialize() {
	sort( subboard, subboard+PILE_LENGTH_MAX, greater<int>() );
}

// Prints the board to the output stream os
// e.g.: print(std::cout)
void Board::print( ostream& os ) {
	os << "[" << subboard[0];
	for( int i = 1; i < PILE_LENGTH_MAX && subboard[i] > 0; i++ )
		os << "," << subboard[i];
	os << "]";
}

// Constructor
void Board::init() {
	subboard = new int[PILE_LENGTH_MAX];
	for( int i = 0; i < PILE_LENGTH_MAX; i++ )
		subboard[i] = 0;
}

// Destructor
void Board::deinit() {
	delete [] subboard;
}

mutex treewrite_mt;

// Datastructure in which the results of known games is stored
class Tree {
	struct Node {
		int win;
		int child_count;
		struct Child {
			int n;
			Node* ch;
		};
		Child* child;

		int findChild( int i );
		int addChild( int i );
		int find( Board, int );
		void add( Board, int, int );
		void print( ostream&, int );
		Node();
	};

	Node head;
public:
	int find( Board );
	void add( Board, int );
	void print( ostream& );
};

// Add a child of value v to the node
int Tree::Node::addChild( int v ) {
	int j;
	int k;
	for( int i = 0; i < child_count; i++ )
		if( child[i].n == v )
			return i;
	Child* temp = child;
	Child* childX = new Child[ child_count + 1 ];
	for( j = 0; j < child_count; j++ )
		if( temp[j].n < v )
			childX[j] = temp[j];
		else 
			break;
	childX[j].n = v;
	childX[j].ch = new Node;
	k = j;
	for(; j < child_count; j++ )
		childX[j+1] = temp[j];
	delete [] temp;
	child_count++;
	child = childX;
	return k;
}

// Find the index of the child with value v
int Tree::Node::findChild( int v ) {
	int j;
	for( j = 0; j < child_count; j++ )
		if( child[j].n >= v )
			break;
	if( j == child_count || child[j].n != v )
		return -1;
	else
		return j;
}

// Find the the result of the game board in the tree recursively
int Tree::Node::find( Board board, int depth ) {
	if( board.subboard[depth] == 0 ) {
		return win;
	} else {
		int c = findChild( board.subboard[depth] );
		if( c == -1 )
			return 0;
		else
			return child[c].ch->find( board, depth + 1 );
	}
}

// Add a game board with result w to the tree
void Tree::Node::add( Board board, int w, int depth ) {
	if( board.subboard[ depth ] == 0 )
		win = w;
	else {
		int c = findChild( board.subboard[ depth ] );
		if( c == -1 ) {
			treewrite_mt.lock();
			c = addChild( board.subboard[ depth ] );
			treewrite_mt.unlock();
		}
		child[ c ].ch->add( board, w, depth + 1 );
	}
}

// Print a node to the output stream os
void Tree::Node::print( ostream& os, int depth ) {
	os << "(" << win << ")";
	for( int i = 0; i < child_count; i++ ) {
		os << "{" << child[i].n << ":";
		child[i].ch->print( os, depth+1 );
		os << "}";
	}
}

// Constructor
Tree::Node::Node() {
	child_count = 0;
	child = nullptr;
	win = 0;
}

// Find the result of game board
int Tree::find( Board board ) {
	return head.find( board, 0 );
}

// Add the result w of a game board to the tree
void Tree::add( Board board, int w ) {
	head.add( board, w, 0 );
}

// Print the tree to the output stream os
void Tree::print( ostream& os ) {
	head.print( os, 0 );
	os << endl;
}

// Global lookup tree, because me lazy
// srsly, wadja gonna do bout it
Tree lookup[2];


#ifdef EXT_RULE
// Calculate the number of unique moves on a subboard of size s
int getSubmoveCount( int s ) {
	return ( s - 1 )/2 + 1;
}

// Converts a submove number to its resulting subboards a and b
void getMoveResult( int s, int submove, int& a, int& b ) {
	a = max( 0, submove - 2 );
	b = s - min( submove + 3, s );
}
#else
// Calculate the number of unique moves on a subboard of size s
int getSubmoveCount( int s ) {
	return ( s - 1 )/2 + 1;
}

// Converts a submove number to its resulting subboards a and b
void getMoveResult( int s, int submove, int& a, int& b ) {
	a = max( 0, submove - 1 );
	b = s - min( submove + 2, s );
}
#endif


// Recursively determine whether the board is winning
// Returns 1 for winning and -1 for losing 
template< bool force_lose = false >
int recursive( Board board, int& moveA, int& moveB ) {
	// check if result of board is known
	int result = lookup[force_lose].find( board );
	if( result != 0 )	
		return result;

	if( force_lose )
		result = 1;
	else
		result = -1;

	for( int subboard = 0; subboard < PILE_LENGTH_MAX; subboard++ ) {
		if( board.subboard[subboard] == 0 )
			break;
		if( subboard == 0 || board.subboard[subboard] != board.subboard[subboard-1] ) {
			// do move
			int subboard_size = board.cut( subboard );

			// calculate submoves
			int submove_count = getSubmoveCount( subboard_size );
			for( int submove = 0; submove < submove_count; submove++ ) {
				int a = 0;
				int b = 0;

				// do submove
				getMoveResult( subboard_size, submove, a, b );
				board.add(a);
				board.add(b);

				// play recursively
				result = -recursive< force_lose >( board, moveA, moveB );

				// undo submove
				board.cut( board.find(a) );
				board.cut( board.find(b) );

				if( result > 0 ) {
					// if player can win, return a win
					moveA = subboard_size;
					moveB = submove;
					board.add( subboard_size );
					lookup[force_lose].add( board, 1 );
					return result;
				}
			}

			// undo move
			board.add( subboard_size );
		}
	}
	// player can't win, return a loss
	lookup[force_lose].add( board, result );
	return result;
}

mutex mt_dataout;
condition_variable cv_threadpool;
mutex mt_cv_threadpool;
ofstream dataout;
const int THREAD_MAX = 10;
int THREAD_COUNT = 0;

void crunch( int i, int j ) {
	int a = -1, b = -1, result;
	Board board;
	board.init();

	getMoveResult( i, j, board.subboard[0], board.subboard[1] );
	board.serialize();

	result = -recursive( board, a, b );

	board.deinit();

	mt_dataout.lock();
	dataout << i << " " << j << " " << result << " " << a << " " << b << endl;
	THREAD_COUNT--;
	cv_threadpool.notify_all();
	mt_dataout.unlock();
}

int main( ) {
	// Setup
	dataout.open( "outputX.csv", fstream::trunc );

	// Generate data
	for( int i = 1; i < 200; i++ ) {
		cout << i << endl;
		for( int j = 0; j < i; j++ ) {
			unique_lock<mutex> lck( mt_cv_threadpool );
			while( THREAD_COUNT >= THREAD_MAX )
				cv_threadpool.wait( lck );

			thread t( crunch, i, j );
			t.detach();

			mt_dataout.lock();
			THREAD_COUNT++;
			mt_dataout.unlock();
		}
	}

	// wait until finished
	unique_lock<mutex> lck( mt_cv_threadpool );
	while( THREAD_COUNT > 0 )
		cv_threadpool.wait( lck );

	return 0;
}