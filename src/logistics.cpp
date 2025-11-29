#include <iostream>
#include <functional>
#include <fstream>

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

void deposit(map<uint32_t, Store>& stores, map<uint32_t, Seller>& sellers, map<uint32_t, Product>& products, map<uint32_t, vector<Product*>> products_by_sellers, uint32_t seller_id)
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
	// мы уже нашли стор поэтому не важно как добыть айтемы
	// set<pair<uint32_t, string>> seller_items;
	vector<Product*>* seller_products_ptr;
	auto seller_products_it = products_by_sellers.find(seller_id);
	if (seller_products_it != products_by_sellers.end())
		seller_products_ptr = &seller_products_it->second;

	Store& store = next(stores.begin(), store_index)->second;
	vector<Items>& items_in_store = store.sellers_items.at(seller_id);
	cout << endl << "--- Item selection ---" << endl << "0. Cancel" << endl;
	
	i = 1;
	for (auto& product_ptr : *seller_products_ptr)
	{
		cout << i << ". " << product_ptr->name << endl;
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
		uint32_t increase = 2;
		if (item_index == i) {
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

			Product product = Product(id, name, size, consist, seller_id, price);
			products.insert({id, product});

			auto seller_products_it = products_by_sellers.find(seller_id);
			if (seller_products_it != products_by_sellers.end()) {
				seller_products_it->second.push_back(&products.at(product.id));
			}
			else {
				products_by_sellers.insert({product.seller_id, vector<Product*>(1, &products.at(product.id))});
			}
			newCreated = true;
		}
		product_ptr = *next(seller_products_ptr->begin(), item_index);
		
		if (!newCreated) {
			for (auto& item : items_in_store)
			{
				if (item.product == product_ptr) {
					item.quantity += increase;
					break;
				}
			}
		}
		else {
			items_in_store.push_back(Items(product_ptr, increase));
		}
		cout << "Done." << endl;
	}
}

void takeOut(map<uint32_t, Store>& stores, map<uint32_t, Seller>& sellers, map<uint32_t, Product>& products, uint32_t seller_id)
{ // убрать айтем из products если его нет больше нигде
	Seller& seller = sellers.at(seller_id);

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
	//*(next(seller.store_items.begin(), store_index)->second);
	Store& store = stores.at(next(seller.store_items.begin(), store_index)->first);
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
	else if (decrease < items_in_store[item_index].quantity)
	{
		items_in_store[item_index].quantity -= decrease;
	}
	else {
		Product* product_ptr = items_in_store[item_index].product;
		if (items_in_store.size() <= 1) {
			seller.store_items.erase(next(seller.store_items.begin(), store_index));
			store.sellers_items.erase(seller_id);
		}
		else {
			items_in_store.erase(items_in_store.begin() + item_index);
		}
		
		bool isThereMore = false;
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
		if (!isThereMore) products.erase(product_ptr->id); // почему бы и нет
		// вроде норм всё хз
	}
	
	cout << "Done." << endl;
}

uint32_t enterMenu(uint32_t& seller_id,
	map<uint32_t, Product>& products,
	map<uint32_t, vector<Product*>> products_by_sellers,
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
				deposit(stores, sellers, products, products_by_sellers, seller_id);
				break;
			case 5:
				takeOut(stores, sellers, products, seller_id); // УДАЛИ АЙТЕМ ИЗ ПРОДАКТОВ!!!!!!!!
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
	map<uint32_t, vector<Product*>> products_by_sellers;
	map<uint32_t, Store> stores;
	map<uint32_t, Seller> sellers;
	loadProducts(products, products_by_sellers);
	loadStores(stores, products);
	loadSellers(sellers, stores);
	saveProducts(products); // удалить потом!!!!!!!!!!!!!!
	saveStores(stores);
	saveSellers(sellers);

	uint32_t seller_id = 0;

	for (;;)
		if (enterMenu(seller_id, products, products_by_sellers, stores, sellers)) return 0;
}