// algo2__1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "RationalNum.h"
#include <vector>
#include "perfect_hash.h"
using namespace std;

int main()
{

	
	vector<string> concatenated ;
	concatenated.push_back(concat(RationalNum (1, 45)));
	concatenated.push_back(concat(RationalNum(-5, 6)));
	concatenated.push_back(concat(RationalNum(11, 103)));
	concatenated.push_back(concat(RationalNum(223, 456)));
	concatenated.push_back(concat(RationalNum(97, 401)));
	concatenated.push_back(concat(RationalNum(9, 2222)));
	concatenated.push_back(concat(RationalNum(6, 5459)));
	concatenated.push_back(concat(RationalNum(1, 10000)));
	concatenated.push_back(concat(RationalNum(78, 9876543)));
	concatenated.push_back(concat(RationalNum(2, 141)));
	
	

	perfect_hash_table<string> hash(concatenated, compute_hash);
	for (size_t i = 0; i < concatenated.size(); i++) {
		cout << compute_hash(concatenated[i]) << endl;
		cout << concatenated[i] << "......." << hash.get(compute_hash(concatenated[i]))<<endl;
	}

}