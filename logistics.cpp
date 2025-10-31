#include <iostream>
#include <fstream>
#include <sstream>
#include "classLib.hpp"

using namespace std;

vector<string> splitToVector(string s, char delim = ',')
{
	string temp;
	vector<string> v;
	stringstream ss(s);
	for (int i = 0; !ss.eof(); ++i) {
		getline(ss, temp, delim);
		v.push_back(temp);
	}
	return v;
}

void loadProducts(vector<Product>& products)
{
	ifstream in("products.csv");
	in.ignore(1024, '\n'); // Id;Name;Size;Consist

	string line;
	while (getline(in, line))
	{
		vector<string> params = splitToVector(line, ';');

		products.push_back(Product(stol(params[0]), params[1], stod(params[2]), splitToVector(params[3], ','), stol(params[4])));
	}
	
	in.close();
}

void loadStores(vector<Store>& stores)
{
	ifstream in("storeId.txt");

	string line;
	while (getline(in, line))
	{
		vector<string> params = splitToVector(line, '|');
		vector<string> address = splitToVector(params[2]);
		unordered_map<uint32_t, vector<Items>> sellers_items;

		vector<string> str_sellers = splitToVector(params[4], ';');
		for (auto& e : str_sellers) {
			vector<string> temp = splitToVector(e, ':');
			vector<Items> items;

			vector<string> str_items = splitToVector(temp[1], '&');
			for (auto& k : str_items) {
				items.push_back(Items())
			}
			sellers_items.insert(stol(temp[0]), )			
		}
		
		
		stores.push_back(Store(stol(params[0]), params[1], Address{static_cast<uint32_t>(stol(address[0])), address[1], address[2], static_cast<uint32_t>(stol(address[3]))}, stod(params[3])));
			// unordered_map<uint32_t, vector<Items>> sellers_items; // id,Items
	}
	
	in.close();
}

void loadSellers(vector<Seller>& sellers, vector<Product>& products, vector<Store>& stores)
{
	ifstream in("sellerId.txt");

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
	vector<Product> products;
	vector<Store> stores;
	vector<Seller> sellers;
	loadProducts(products);
	loadStores(stores);
	loadSellers(sellers, products, stores);

	size_t sellerId = 0;
	printMenu(sellerId);
	return 0;
}