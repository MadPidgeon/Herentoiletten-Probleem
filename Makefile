FLAGS = -Wall -Wextra -Wpedantic -O3 -std=c++11

all: bruteforce lookup_table_win lookup_table_lose

bruteforce: bruteforce.cc
	g++ bruteforce.cc -o bruteforce $(FLAGS)

lookup_table_win: lookup_table.cc
	g++ lookup_table.cc -o lookup_table_win $(FLAGS)

lookup_table_win_data: lookup_table.cc
	g++ lookup_table.cc -o lookup_table_win_data $(FLAGS) -DDATAOUTPUT

lookup_table_lose: lookup_table.cc
	g++ lookup_table.cc -o lookup_table_lose $(FLAGS) -DFORCE_LOSE

lookup_table_lose_data: lookup_table.cc
	g++ lookup_table.cc -o lookup_table_lose_data $(FLAGS) -DFORCE_LOSE -DDATAOUTPUT