#include <iostream>
			
#include <fstream>
#include <sstream>
#include "classLib.hpp"

using namespace std;

vector<string> splitToVector(string s, char delim = ',')
{
	vector<string> v;
	string temp;
	stringstream ss(s);
	for (int i = 0; !ss.eof(); ++i) {
		getline(ss, temp, delim);
		v.push_back(temp);
	}
	return v;
}

void loadProducts(unordered_map<uint32_t, Product>& products)
{
	ifstream in("products.csv");
	in.ignore(1024, '\n'); // Id;Name;Size;Consist

	string line;
	while (getline(in, line))
	{
		vector<string> params = splitToVector(line, ';');

		products.insert({stoul(params[0]), Product(params[1], stod(params[2]), splitToVector(params[3], ','), stoul(params[4]))});
	}
	
	in.close();
}

void loadStores(unordered_map<uint32_t, Store>& stores, unordered_map<uint32_t, Product>& products)
{
	ifstream in("storeId.txt");

	string line;
	while (getline(in, line))
	{
		vector<string> params = splitToVector(line, '|');
		vector<string> address = splitToVector(params[2], ',');
		unordered_map<uint32_t, vector<Items>> sellers_items;

		vector<Items> items;
		vector<string> str_sellers = splitToVector(params[4], '&');
		for (auto& e : str_sellers) {
			vector<string> temp = splitToVector(e, ':');

			vector<string> str_items = splitToVector(temp[1], ';');
			for (auto& str_item : str_items) {
				vector<string> temp2 = splitToVector(str_item, ',');
				auto product_it = products.find(stoul(temp2[0]));
				if (product_it != products.end()) {
					items.push_back(Items(&(product_it->second), stoul(temp2[1])));
				}
				else {
					cerr << "loadStores, product_it == product.end()" << endl;
				}
			}
			sellers_items.insert({stoul(temp[0]), items});
		}
		
		stores.insert({stoul(params[0]), Store(params[1], Address{static_cast<uint32_t>(stoul(address[0])), address[1], address[2], static_cast<uint32_t>(stoul(address[3]))}, stod(params[3]), sellers_items)});
			// unordered_map<uint32_t, vector<Items>> sellers_items; // id,Items
	}
	
	in.close();
}

// САММАРИ 1 НОВ - мапа сверху селлер айтемы. в принципе все айдишки можно так и сделать, при лоде селлеров короче
// намутить либо общий список айтемов (vector<Items> там и так указатели), 
// либо массив указателей на sellers_items (сверху коммент). определённо через парам с номерами складов
// будем итерировать по складам (поэтому и передаю stores), но в самом классе такое не нужно, тк список предметов будет хз
// (тогда действительно лучше массив указателей на sellers items) бб
void loadSellers(vector<Seller>& sellers, unordered_map<uint32_t, Store>& stores)
{
	ifstream in("sellerId.txt");

	string line;
	while (getline(in, line))
	{
		vector<string> params = splitToVector(line, '|');
		uint32_t seller_id = stoul(params[0]);

		unordered_map<uint32_t, vector<Items>*> items;
		for (auto& store_id : splitToVector(params[2], ','))
		{
			auto store_it = stores.find(stoul(store_id));	
			if (store_it != stores.end()) {
				auto seller_items_it = (store_it->second).sellers_items.find(seller_id);	
				if (seller_items_it != (store_it->second).sellers_items.end()) {
					items.insert({stoul(store_id), &(seller_items_it->second)});
				}
				else {
					cerr << "loadSellers, 2it == .end()" << endl;
				}
			}
			else {
				cerr << "loadSellers, it == .end()" << endl;
			}
		}
		
		sellers.push_back(Seller(seller_id, params[1], items));
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
	unordered_map<uint32_t, Product> products;
	unordered_map<uint32_t, Store> stores;
	vector<Seller> sellers;
	loadProducts(products);
	loadStores(stores, products);
	loadSellers(sellers, stores);

	size_t sellerId = 0;
	printMenu(sellerId);
	return 0;
}