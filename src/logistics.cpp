#include <iostream>
#include <set>

#include "../include/classLib.hpp"
#include "../include/utils.hpp"
#include "../include/login.hpp"
#include "../include/LogisticsCore.hpp"

using namespace std;

void printStores(map<uint32_t, Store>& stores, map<uint32_t, Seller> sellers) // rewrite maybe? 
{
	for (auto& [id, store] : stores) {
		cout << endl << " ------------- " << endl;
		cout << "Name: " << store.name << endl;
		cout << "Address: " << store.address.index << ", " << store.address.city << ", " << store.address.street << ", " << store.address.house_number << endl;
		cout << "Capacity: " << store.capacity << endl;
		
		cout << endl << "- Sellers + items -";
		for (auto& [seller_id, items] : store.sellers_items) {
			Seller& seller = sellers.at(seller_id);
			cout << endl << "Seller name: " << seller.name << endl;
			for (auto& item : items) {
				cout << "Item name: "<< item.product->name << ", size: " << item.product->size << ", quantity: " << item.quantity << endl;
			}
		}
	}
}

void depositItems(map<uint32_t, Store>& stores, map<uint32_t, Seller>& sellers, map<uint32_t, Product>& products, map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers, uint32_t seller_id)
{ // выбор из имеющихся айтемов его
	Seller& seller = sellers.at(seller_id);
	
	cout << endl << "--- Store selection ---" << endl << "0. Cancel" << endl;
	uint32_t i = 1, store_index;
	for (auto& [id, store] : stores) {
		cout << i << ". " << store.name << endl;
		++i;
	}
	cout << " : ";

	cin >> store_index;
	if (!store_index || store_index >= i) return;
	--store_index;

	map<uint32_t, pair<Items, set<uint32_t>>>* seller_items_ptr;
	auto seller_items_it = items_by_sellers.find(seller_id);
	if (seller_items_it != items_by_sellers.end())
		seller_items_ptr = &(seller_items_it->second);

	auto& store_pair = *next(stores.begin(), store_index);
	uint32_t store_id = store_pair.first;
	Store& store = store_pair.second;
	cout << endl << "--- Item selection ---" << endl << "0. Cancel" << endl;
	
	i = 1;
	for (auto& [product_id, pair] : *seller_items_ptr)
	{
		cout << i << ". " << pair.first.product->name << endl;
		++i;
	}
	cout << i << ". Add new item" << endl << " : ";

	uint32_t item_index;
	cin >> item_index;
	if (!item_index || item_index > i) return;
	else {
		--item_index;
		Product* product_ptr;
		
		bool newCreated = false;
		uint32_t increase;
		if (item_index == i - 1) {
			cout << endl << "--- New item ---" << endl;
			uint32_t price, id = products.rbegin()->first + 1;
			string name, consist_str;
			double size;
			vector<string> consist;

			cout << "Name: ";
			cin >> name;
			cout << "Size: ";
			cin >> size;
			cout << "Consists of (comma-separated): ";
			cin >> consist_str;
			consist = splitToVector(consist_str, ',');
			cout << "Price: ";
			cin >> price;
			cout << "How much?: ";
			cin >> increase;

			Product product = Product(id, name, size, consist, seller_id, price);
			products.insert({id, product});
			newCreated = true;
		}
		else {
			cout << endl << "How much? : ";
			cin >> increase;
		}

		product_ptr = next(seller_items_ptr->begin(), item_index)->second.first.product;
		
		LogisticsCore::depositItems(product_ptr, increase, seller, store, items_by_sellers);

		cout << "Done." << endl;
	}
}

void takeOutItems(
	map<uint32_t, Store>& stores,
	map<uint32_t, Seller>& sellers,
	map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers,
	uint32_t seller_id)
{ //  НЕ НАДО: убрать айтем из products если его нет больше нигде
	Seller& seller = sellers.at(seller_id);
	
	if (seller.store_items.empty()) {
		cout << "You don't have any items in stores. Deposit some first";
		return;
	}

	cout << endl << "--- Store selection ---" << endl << "0. Cancel" << endl;
	uint32_t i = 1, store_index;
	for (auto& [id, items] : seller.store_items) {
		Store& store = stores.at(id);
		cout << i << ". " << store.name << endl;
		++i;
	}
	cout << " : ";

	cin >> store_index;
	if (!store_index || store_index >= i) return;
	--store_index;

	cout << endl << "--- Item selection ---" << endl << "0. Cancel" << endl;
	uint32_t store_id = next(seller.store_items.begin(), store_index)->first;
	Store& store = stores.at(store_id);
	vector<Items>& items_in_store = store.sellers_items[seller_id];

	i = 1;
	for (auto& items : items_in_store) {
		cout << i << ". " << items.product->name << ", " << items.quantity << endl;
		++i;
	}
	cout << " : ";

	uint32_t item_index;
	cin >> item_index;
	if (!item_index || item_index >= i) return;
	--item_index;

	uint32_t decrease;
	cout << endl << "How much? : ";
	cin >> decrease;

	// та же i схема + пользуемся вектором [] и у селлера и на складе одни индексы у айтемов => профит даже без мапы, хотя убого, но массив указателей должен быть по тз

	Product* product_ptr = items_in_store[item_index].product;
	LogisticsCore::takeOutItems(product_ptr, decrease, seller, store, items_by_sellers);
	
	cout << "Done." << endl;
}

int enterMenu(uint32_t& seller_id,
	map<uint32_t, Product>& products,
	map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers,
	map<uint32_t, Store>& stores,
	map<uint32_t, Seller>& sellers)
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
				seller_id = loginSellerProc();
				break;
			case 3:
				return 1;
		}
	}
	else {
		cout << "Logged as " << sellers.at(seller_id).name << endl;
		cout << "1. View stores" << endl;
		cout << "2. Log out" << endl;
		cout << "3. Move items" << endl;
		cout << "4. Deposit items" << endl;
		cout << "5. Take out items" << endl;
		cout << "6. Exit app" << endl << " : ";
		cin >> flag;
		switch (flag) {
			case 1:
				printStores(stores, sellers);
				break;
			case 2:
				seller_id = 0;
				break;
			case 3:
				//moveItems();
				break;
			case 4:
				depositItems(stores, sellers, products, items_by_sellers, seller_id);
				saveProducts(products);
				saveStores(stores);
				saveSellers(sellers);
				break;
			case 5:
				takeOutItems(stores, sellers, items_by_sellers, seller_id);
				saveStores(stores);
				saveSellers(sellers);
				break;
			case 6:
				return true;
		}
	}
	return 0;
}

int main()
{
	map<uint32_t, Product> products;
	map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>> items_by_sellers;
	// (айди продавца - (айди продакта - (указатель на продакт, кол-во), айди складов))
	map<uint32_t, Store> stores;
	map<uint32_t, Seller> sellers;
	loadProducts(products, items_by_sellers);
	loadStores(stores, products, items_by_sellers);
	loadSellers(sellers, stores);
	saveProducts(products); // удалить потом!!!!!!!!!!!!!!
	saveStores(stores);
	saveSellers(sellers);

	uint32_t seller_id = 0;

	for (;;)
		if (enterMenu(seller_id, products, items_by_sellers, stores, sellers)) return 0;
}