#include <iostream>
#include <fstream>
#include <sstream>
#include "classLib.hpp"

using namespace std;

void loadStores(vector<Store>& stores)
{
	ifstream in("storeId.txt");

	string line;
	while (getline(in, line))
	{
		stringstream ss(line);
		vector<string> params;
		string temp;
		for (int i = 0; !ss.eof(); ++i) {
			getline(ss, temp, '|');
			params.push_back(temp);
		}

		stores.push_back(Store(params[1], params[2], stod(params[3])));
	}
	
	in.close();
}

void loadSellers(vector<Seller>& sellers)
{
	ifstream in("storeId.txt");

	string line;
	while (getline(in, line))
	{
		/*
		stringstream ss(line);
		vector<string> params;
		string temp;
		for (int i = 0; !ss.eof(); ++i) {
			getline(ss, temp, '|');
			params.push_back(temp);
		}

		stores.push_back(Store(params[1], params[2], stod(params[3])));
		*/
	}
	
	in.close();
}

void printMenu(size_t sellerId)
{
	std::cout << std::endl << "--- Menu ---" << std::endl;
	std::cout << "1. View stores" << endl;
	if (!sellerId) {
		std::cout << "2. Login as seller";
	}
	else {
		std::cout << "Logged as " << sellerId;
	}
}

int main()
{
	vector<Store> stores;
	loadStores(stores);

	size_t sellerId = 0;
	printMenu(sellerId);
	return 0;
}