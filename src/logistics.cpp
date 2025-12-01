#include <iostream>
#include <set>

#include "../include/classLib.hpp"
#include "../include/utils.hpp"
#include "../include/login.hpp"
#include "../include/SyncAPI.hpp"
#include "../include/LogisticsCore.hpp"

using namespace std;

void printStores(map<uint32_t, Store>& stores, map<uint32_t, Seller> sellers) // rewrite maybe? 
{
	for (auto& [id, store] : stores) {
		cout << endl << " ------------- " << endl;
		cout << "Name: " << store.name << endl;
		cout << "Address: " << store.address.index << ", " << store.address.city << ", " << store.address.street << ", " << store.address.house_number << endl;
		cout << "Capacity: " << store.size << '/' << store.capacity << endl;
		
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

int storeSelection(map<uint32_t, Store>& stores, Seller& seller, bool allStores = true)
{
	uint32_t i = 1, store_index;
	if (allStores) {
		for (auto& [id, store] : stores) {
			cout << i << ". " << store.name << endl;
			++i;
		}
	}
	else {
		for (auto& [id, items] : seller.store_items) {
			Store& store = stores.at(id);
			cout << i << ". " << store.name << endl;
			++i;
		}
	}
	cout << " : ";

	cin >> store_index;
	if (!store_index || store_index >= i) return -1;
	return store_index - 1;
}

void depositItems(
	map<uint32_t, Store>& stores,
	map<uint32_t, Seller>& sellers,
	map<uint32_t, Product>& products,
	map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers,
	uint32_t seller_id)
{ // выбор из имеющихся айтемов его
	Seller& seller = sellers.at(seller_id);
	
	cout << endl << "--- Store selection ---" << endl << "0. Cancel" << endl;
	int store_index = storeSelection(stores, seller, true);
	if (store_index < 0) return;

	map<uint32_t, pair<Items, set<uint32_t>>>* seller_items_ptr = &(items_by_sellers.at(seller_id));

	auto& store_pair = *next(stores.begin(), store_index);
	uint32_t store_id = store_pair.first;
	Store& store = store_pair.second;

	cout << endl << "--- Item selection ---" << endl << "0. Cancel" << endl;
	
	uint32_t i = 1;
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
			product_ptr = &(products.at(id));
		}
		else {
			cout << endl << "How much? : ";
			cin >> increase;
			product_ptr = next(seller_items_ptr->begin(), item_index)->second.first.product;
		}

		if (LogisticsCore::deposit(product_ptr, increase, seller, store, items_by_sellers, newCreated)) {
			cout << "There is not enough space. " << store.name << '(' << store.size << '/' << store.capacity << ')' << endl;
			return;
		}

		cout << endl << "Done. " << store.name << '(' << store.size << '/' << store.capacity << ')' << endl;
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
	int store_index = storeSelection(stores, seller, false);
	if (store_index < 0) return;

	uint32_t store_id = next(seller.store_items.begin(), store_index)->first;
	Store& store = stores.at(store_id);

	cout << endl << "--- Item selection ---" << endl << "0. Cancel" << endl;
	vector<Items>& items_in_store = store.sellers_items[seller_id];

	uint32_t i = 1;
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
	LogisticsCore::takeOut(product_ptr, decrease, seller, store, items_by_sellers);
	
	cout << "Done. " << store.name << '(' << store.size << '/' << store.capacity << ')' << endl;
}

void moveItems(
	map<uint32_t, Store>& stores,
	map<uint32_t, Seller>& sellers,
	map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers,
	uint32_t seller_id)
{
	Seller& seller = sellers.at(seller_id);
	
	if (seller.store_items.empty()) {
		cout << "You don't have any items in stores. Deposit some first";
		return;
	}

	cout << endl << "--- Store selection (FROM) ---" << endl << "0. Cancel" << endl;
	int store_index_from = storeSelection(stores, seller, false);
	if (store_index_from < 0) return;

	cout << endl << "--- Store selection (TO) ---" << endl << "0. Cancel" << endl;
	int store_index_to = storeSelection(stores, seller, true);
	if (store_index_to < 0 || store_index_from == store_index_to) return;

	int32_t store_from_id = next(seller.store_items.begin(), store_index_from)->first;
	Store& store_from = stores.at(store_from_id);
	uint32_t store_to_id = next(seller.store_items.begin(), store_index_to)->first;
	Store& store_to = stores.at(store_to_id);

	cout << endl << "--- Item selection ---" << endl << "0. Cancel" << endl;
	vector<Items>& items_in_store = store_from.sellers_items[seller_id];

	uint32_t i = 1;
	for (auto& items : items_in_store) {
		cout << i << ". " << items.product->name << ", " << items.quantity << endl;
		++i;
	}
	cout << " : ";

	uint32_t item_index;
	cin >> item_index;
	if (!item_index || item_index >= i) return;
	--item_index;

	uint32_t count;
	cout << endl << "How much? : ";
	cin >> count;

	// та же i схема + пользуемся вектором [] и у селлера и на складе одни индексы у айтемов => профит даже без мапы, хотя убого, но массив указателей должен быть по тз

	Product* product_ptr = items_in_store[item_index].product;
	LogisticsCore::deposit(product_ptr, count, seller, store_to, items_by_sellers);
	if (LogisticsCore::deposit(product_ptr, count, seller, store_to, items_by_sellers)) {
		cout << "There is not enough space. " << store_to.name << '(' << store_to.size << '/' << store_to.capacity << ')' << endl;
		return;
	}
	LogisticsCore::takeOut(product_ptr, count, seller, store_from, items_by_sellers);
	// сначала deposit, тк проверяем поместиться ли предмет

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
				moveItems(stores, sellers, items_by_sellers, seller_id);
				SyncAPI::saveStores(stores);
				SyncAPI::saveSellers(sellers);
				break;
			case 4:
				depositItems(stores, sellers, products, items_by_sellers, seller_id);
				SyncAPI::saveProducts(products);
				SyncAPI::saveStores(stores);
				SyncAPI::saveSellers(sellers);
				break;
			case 5:
				takeOutItems(stores, sellers, items_by_sellers, seller_id);
				SyncAPI::saveStores(stores);
				SyncAPI::saveSellers(sellers);
				break;
			case 6:
				return 1;
		}
	}
	return 0;
}

int main()
{
	map<uint32_t, Product> products;
	map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>> items_by_sellers;
	// (айди продавца - (айди продакта - айтемс(указатель на продакт, кол-во), айди складов))
	map<uint32_t, Store> stores;
	map<uint32_t, Seller> sellers;

	SyncAPI::loadProducts(products, items_by_sellers);
	SyncAPI::loadStores(stores, products, items_by_sellers);
	SyncAPI::loadSellers(sellers, stores);

	// удалить эту секцию, если не используются файлы для тестов
	SyncAPI::saveProducts(products);
	SyncAPI::saveStores(stores);
	SyncAPI::saveSellers(sellers);

	uint32_t seller_id = 0;

	for (;;)
		if (enterMenu(seller_id, products, items_by_sellers, stores, sellers)) return 0;
}