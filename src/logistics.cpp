#include <iostream>
#include <functional>
#include <fstream>
#include <set>

#include "../include/classLib.hpp"
#include "../include/utils.hpp"
#include "../include/login.hpp"

using namespace std;

void printStores(map<uint32_t, Store>& stores, map<uint32_t, Seller> sellers) // пересмотреть?
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

void deposit(map<uint32_t, Store>& stores, map<uint32_t, Seller>& sellers, map<uint32_t, Product>& products, map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers, uint32_t seller_id)
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
	vector<Items>& items_in_store = store.sellers_items.at(seller_id);
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

			pair<uint32_t, pair<Items, set<uint32_t>>> new_items_entry{product.id, {Items(&products.at(product.id), increase), {store_id}}};
			if (seller_items_it != items_by_sellers.end()) {
				seller_items_it->second.insert(new_items_entry);
			}
			else {
				items_by_sellers.insert({product.seller_id, {new_items_entry}});
			}
			newCreated = true;
		}
		product_ptr = next(seller_items_ptr->begin(), item_index)->second.first.product;
		// итератор на продакт в мапе -> пара (items, set) . items . указатель на продакт
		
		if (!newCreated) {
			cout << endl << "How much? : ";
			cin >> increase;
			seller_items_ptr->at(product_ptr->id).first.quantity += increase;
			// указатель на айтемы продавца -> пара (items, set) . items . количество
			bool isThereAny = false;
			for (auto& item : items_in_store)
			{
				if (item.product == product_ptr) {
					item.quantity += increase;
					isThereAny = true;
					break;
				}
			}
			if (!isThereAny) {
				items_in_store.push_back(Items(product_ptr, increase));
			}
		}
		else {
			items_in_store.push_back(Items(product_ptr, increase));
		}
		cout << "Done." << endl;
	}
}

void takeOut(
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

	if (!decrease) return;
	Items& selected_items = items_in_store[item_index];
	if (decrease < selected_items.quantity)
	{
		selected_items.quantity -= decrease;
		items_by_sellers.at(seller_id).at(selected_items.product->id).first.quantity -= decrease;
	}
	else {
		Product* product_ptr = selected_items.product;
		items_by_sellers.at(seller_id).at(product_ptr->id).first.quantity -= selected_items.quantity;
		items_by_sellers.at(seller_id).at(product_ptr->id).second.erase(store_id);
		if (items_in_store.size() <= 1) {
			seller.store_items.erase(next(seller.store_items.begin(), store_index));
			store.sellers_items.erase(seller_id);
		}
		else {
			items_in_store.erase(items_in_store.begin() + item_index);
		}
		
		/* bool isThereMore = false;
		for (auto& [store_id, store] : stores) {
			for (auto& [seller_id, seller_items] : store.sellers_items) {
				for (auto& items : seller_items) {
					if (product_ptr == items.product) {
						isThereMore = true;
						break;
					}
				}
			}
		}
		if (!isThereMore) products.erase(product_ptr->id);
		*/
	}
	
	cout << "Done." << endl;
}

uint32_t enterMenu(uint32_t& seller_id,
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
				// депозит через создание айтема (прям конструктором новый айтем) количество далее запихиваем на склад и если там не было айтемов продавца то закидываем указатель на массив к продавцу И В ГЛОБАЛЬНЫЙ ПРОДАКТС ЗАКИДЫВАЕМ АЙТЕМ
			case 4:
				deposit(stores, sellers, products, items_by_sellers, seller_id);
				saveProducts(products);
				saveStores(stores);
				saveSellers(sellers);
				break;
			case 5:
				takeOut(stores, sellers, items_by_sellers, seller_id);
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