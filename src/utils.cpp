#include <iostream>
#include <fstream>
#include <set>
#include <sstream>

#include "../include/utils.hpp"

vector<string> splitToVector(string s, char delim)
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

void loadProducts(map<uint32_t, Product>& products, map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers, string path)
{
	ifstream in(path);
	in.ignore(1024, '\n'); // Id;Name;Size;Consist

	string line;
	while (getline(in, line))
	{
		if (line.empty()) continue;
		vector<string> params = splitToVector(line, ';');

		Product product = Product(stoul(params[0]), params[1], stod(params[2]), splitToVector(params[3], ','), stoul(params[4]), stoul(params[5]));
		products.insert({stoul(params[0]), product});

		auto seller_products_it = items_by_sellers.find(product.seller_id);
		if (seller_products_it != items_by_sellers.end()) {
			seller_products_it->second.insert({product.id, {Items(&products.at(product.id), 0), {} }});
		}
		else {
			items_by_sellers.insert({product.seller_id, 
				map<uint32_t, pair<Items, set<uint32_t>>>{{product.id,
					{Items(&products.at(product.id), 0), {} }}}});
		}
		// инициализировали все айтемы, количество посчитаем в loadStores, там же занесём в сет айди складов
	}
	
	in.close();
}

void loadStores(map<uint32_t, Store>& stores, map<uint32_t, Product>& products, map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers, string path)
{
	ifstream in(path);

	string line;
	while (getline(in, line))
	{
		if (line.empty()) continue;
		vector<string> params = splitToVector(line, '|');
		uint32_t store_id = stoul(params[0]);
		vector<string> address = splitToVector(params[2], ',');
		map<uint32_t, vector<Items>> sellers_items;

		vector<string> str_items = splitToVector(params[4], ';');
		for (auto& str_item : str_items) {
			vector<string> temp = splitToVector(str_item, ',');
			Product& product = products.at(stoul(temp[0]));
			uint32_t quantity = stoul(temp[1]);
			Items items = Items(&product, quantity);

			auto seller_items_it = sellers_items.find(product.seller_id);
			if (seller_items_it != sellers_items.end()) {
				seller_items_it->second.push_back(items);
			}
			else {
				sellers_items.insert({product.seller_id, vector<Items>{items}});
			}
			
			items_by_sellers.at(product.seller_id).at(product.id).second.insert(store_id);
			items_by_sellers.at(product.seller_id).at(product.id).first.quantity += quantity;
		}
		/*
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
		*/
		
		stores.insert({store_id, Store(stoul(params[0]), params[1], Address{address[0], address[1], address[2], stoul(address[3])}, stod(params[3]), sellers_items)});
			// map<uint32_t, vector<Items>> sellers_items; // id,Items
	}
	
	in.close();
}

void loadSellers(map<uint32_t, Seller>& sellers, map<uint32_t, Store>& stores, string path)
{
	ifstream in(path);

	string line;
	while (getline(in, line))
	{
		if (line.empty()) continue;
		vector<string> params = splitToVector(line, '|');
		uint32_t seller_id = stoul(params[0]);

		map<uint32_t, vector<Items>*> items;
		for (auto& store_id : splitToVector(params[2], ','))
		{
			Store& store = stores.at(stoul(store_id));	
			vector<Items>& seller_items = store.sellers_items.at(seller_id);	
			items.insert({stoul(store_id), &(seller_items)});
		}
		
		sellers.insert({seller_id, Seller(params[1], items)});
	}
	
	in.close();
}

void saveProducts(map<uint32_t, Product> products, string path)
{
	ofstream out(path);
	out << "Id;Name;Size;Consist;SellerId;Price\n";
	for (auto& [id, product] : products) {
		out << id << ';' << product.name << ';' << product.size << ';';
		for (uint32_t i = 0; i < product.consist.size() - 1; ++i) {
			out << product.consist[i] << ','; 
		}
		out << product.consist[product.consist.size() - 1] << ';' << product.seller_id << ';' << product.price << '\n';
	}
	out.close();
}

void saveStores(map<uint32_t, Store> stores, string path)
{
	ofstream out(path);
	for (auto& [id, store] : stores) {
		out << id << '|' << store.name << '|' << 
			store.address.index << ',' << store.address.city << ',' << store.address.street << ',' << store.address.house_number << '|' <<
			store.capacity << '|';
		uint32_t cnt = 0;
		for (auto& [seller_id, items] : store.sellers_items) {
			if (cnt++) out << ';';
			for (uint32_t i = 0; i < items.size() - 1; ++i) {
				out << items[i].product->id << ',' << items[i].quantity << ';';
			}
			out << items[items.size() - 1].product->id << ',' << items[items.size() - 1].quantity;
		}
		out << '\n';
	}
	out.close();
}

void saveSellers(map<uint32_t, Seller> sellers, string path)
{
	ofstream out(path);
	for (auto& [id, seller] : sellers) {
		out << id << '|' << seller.name << '|';
		uint32_t cnt = 0;
		for (auto& [store_id, items] : seller.store_items) {
			out << (!cnt++ ? to_string(store_id) : "," + to_string(store_id));
		}
		out << '\n';
	}
	out.close();
}