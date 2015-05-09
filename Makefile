FLAGS = -Wall -Wextra -Wpedantic -O3 -std=c++11

all: bruteforce lookup_table

bruteforce: bruteforce.cc
	g++ bruteforce.cc -o bruteforce $(FLAGS)

lookup_table: lookup_table.cc
	g++ lookup_table.cc -o lookup_table $(FLAGS)
