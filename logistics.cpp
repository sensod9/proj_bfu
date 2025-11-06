#include <iostream>
#include <functional>
			
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
		if (line.empty()) continue;
		vector<string> params = splitToVector(line, ';');

		products.insert({stoul(params[0]), Product(stoul(params[0]), params[1], stod(params[2]), splitToVector(params[3], ','), stoul(params[4]))});
	}
	
	in.close();
}

void loadStores(unordered_map<uint32_t, Store>& stores, unordered_map<uint32_t, Product>& products)
{
	ifstream in("storeId.txt");

	string line;
	while (getline(in, line))
	{
		if (line.empty()) continue;
		vector<string> params = splitToVector(line, '|');
		vector<string> address = splitToVector(params[2], ',');
		unordered_map<uint32_t, vector<Items>> sellers_items;

		vector<Items> items;
		vector<string> str_sellers = splitToVector(params[4], '&');
		for (auto& e : str_sellers) {
			items.clear();
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

void loadSellers(unordered_map<uint32_t, Seller>& sellers, unordered_map<uint32_t, Store>& stores)
{
	ifstream in("sellerId.txt");

	string line;
	while (getline(in, line))
	{
		if (line.empty()) continue;
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
		
		sellers.insert({seller_id, Seller(params[1], items)});
	}
	
	in.close();
}

void saveProducts(unordered_map<uint32_t, Product> products)
{
	ofstream out("products1.csv");
	out << "Id;Name;Size;Consist;Price\n";
	for (auto& [id, product] : products) {
		out << id << ';' << product.name << ';' << product.size << ';';
		for (uint32_t i = 0; i < product.consist.size() - 1; ++i) {
			out << product.consist[i] << ','; 
		}
		out << product.consist[product.consist.size() - 1] << ';' << product.price << '\n';
	}
	out.close();
}

void saveStores(unordered_map<uint32_t, Store> stores)
{
	ofstream out("storeId1.txt");
	for (auto& [id, store] : stores) {
		out << id << '|' << store.name << '|' << 
			store.address.index << ',' << store.address.city << ',' << store.address.street << ',' << store.address.house_number << '|' <<
			store.capacity << '|';
		uint32_t cnt = 0;
		for (auto& [id, items] : store.sellers_items) {
			if (!cnt++) out << '&';
			out << id << ':';
			for (uint32_t i = 0; i < items.size() - 1; ++i) {
				out << items[i].product->id << ',' << items[i].quantity << ';';
			}
			out << items[items.size() - 1].product->id;
		}
		out << '\n';
	}
	out.close();
}

void saveSellers(unordered_map<uint32_t, Seller> sellers, unordered_map<uint32_t, Store> stores)
{
	ofstream out("sellerId1.txt");
	for (auto& [id, seller] : sellers) {
		out << id << '|' << seller.name << '|';
		uint32_t cnt = 0;
		for (auto& [store_id, store] : stores) {
			if (stores.find(id) != stores.end()) out << (!cnt++ ? to_string(store_id) : "," + to_string(store_id));
		}
		out << '\n';
	}
	out.close();
}

void printStores(unordered_map<uint32_t, Store>& stores, unordered_map<uint32_t, Seller> sellers)
{
	for (auto& [id, store] : stores) {
		cout << endl << " ------------- " << endl;
		cout << "Name: " << store.name << endl;
		cout << "Address: " << store.address.index << ", " << store.address.city << ", " << store.address.street << ", " << store.address.house_number << endl;
		cout << "Capacity: " << store.capacity << endl;
		
		cout << endl << "- Sellers + items -";
		for (auto& [seller_id, items] : store.sellers_items) {
			auto seller_it = sellers.find(seller_id);
			if (seller_it != sellers.end()) {
				cout << endl << "Seller name: " << seller_it->second.name << endl;
				for (auto& item : items) {
					cout << "Item name: "<< item.product->name << ", size: " << item.product->size << ", quantity: " << item.quantity << endl;
				}
			}
		}
	}
}

uint32_t loginProc() {
	ifstream in("db.txt");
	vector<vector<string>> sellers; 
	string line;
	while (getline(in, line))
	{
		sellers.push_back(splitToVector(line, '|'));
	}
	in.close();

	hash<string> hasher;
	for (;;) {
		string login, password;

		cout << endl << "Login: "; 
		cin >> login;
		if (login == "0") return 0;
		cout << "Password: "; 
		cin >> password;
		uint64_t h_login = hasher(login);
		uint64_t h_password = hasher(password);
		for (auto& seller : sellers) {
			if (h_login == stoull(seller[0]) && h_password == stoull(seller[1])) {
				return stoull(seller[2]);				
			}
		}
		cout << "Incorrect. Try Again" << endl;
	}
}

uint32_t enterMenu(uint32_t& seller_id,
	unordered_map<uint32_t, Product>& products,
	unordered_map<uint32_t, Store>& stores,
	unordered_map<uint32_t, Seller>& sellers)
{
	uint32_t flag;
	cout << endl << "--- Menu ---" << endl;
	if (!seller_id) {
		cout << "1. View stores" << endl;
		cout << "2. Login as seller" << endl;
		cout << "3. Exit app" << endl << " : ";

		cin >> flag;
		switch (flag) {
			case 1:
				printStores(stores, sellers);
				break;
			case 2:
				seller_id = loginProc();
				break;
			case 3:
				return 1;
		}
	}
	else {
		auto seller_it = sellers.find(seller_id);
		if (seller_it == sellers.end()) return 1;

		cout << "Logged as " << seller_it->second.name << endl;
		cout << "1. View stores" << endl;
		cout << "2. Log out" << endl;
		cout << "3. Move items" << endl;
		cout << "3. Deposit items" << endl;
		cout << "5. Take out items" << endl;
		cout << "6. Exit app" << endl << " : ";
		cin >> flag;
		switch (flag) {
			case 1:
				printStores(stores, sellers);
				break;
			case 2:
				seller_id = loginProc();
				break;
			case 3:
				return true;
		}
	}
	return 0;
}

int main()
{
	unordered_map<uint32_t, Product> products;
	unordered_map<uint32_t, Store> stores;
	unordered_map<uint32_t, Seller> sellers;
	loadProducts(products);
	loadStores(stores, products);
	loadSellers(sellers, stores);
	saveProducts(products);
	saveStores(stores);
	saveSellers(sellers, stores);

	uint32_t seller_id = 0;

	for (;;)
		if (enterMenu(seller_id, products, stores, sellers)) return 0;
}