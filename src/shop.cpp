#include <iostream>
#include <set>
#include <stack>

#include "../include/classLib.hpp"
#include "../include/utils.hpp"
#include "../include/login.hpp"
#include "../include/SyncAPI.hpp"
#include "../include/LogisticsCore.hpp"

using namespace std;

void showItemsDetails(
	pair<Items, set<uint32_t>>* items_pair_ptr,
	map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers,
	map<uint32_t, Seller>& sellers,
	map<uint32_t, Store>& stores,
	stack<pair<Items, Store&>>& cart)
{
	uint32_t seller_id = items_pair_ptr->first.product->seller_id;
	Seller& seller = sellers.at(seller_id);

	cout << "----------------";
	cout << endl << items_pair_ptr->first.product->name << ": " << items_pair_ptr->first.quantity << "pcs" << endl;
	cout << "Price: " << items_pair_ptr->first.product->price << endl;
	cout << "Seller: " << seller.name << endl;
	cout << "Stored in: ";
	
	uint32_t i = 1;
	for (auto& store_id : items_pair_ptr->second) {
		cout << endl;
		Store& store = stores.at(store_id);
		cout << i << ". " << store.name << " (" << store.sellers_items.at(seller_id).at(items_pair_ptr->first.product->id).quantity << ')' << endl;
		cout << "Address: " << store.address.index << ", " << store.address.city << ", " << store.address.street << ", " << store.address.house_number << endl;
		++i;
	}
	cout << "0. Cancel" << endl;
	cout << " : ";

	uint32_t store_index;
	cin >> store_index;
	if (!store_index || store_index >= i) return;
	--store_index;
	
	uint32_t store_id = next(seller.store_items.begin(), store_index)->first;
	Store& store = stores.at(store_id);

	uint32_t decrease;
	cout << endl << "How much? : ";
	cin >> decrease;
	if (!decrease) return;
	
	if (decrease > store.sellers_items.at(seller_id).at(items_pair_ptr->first.product->id).quantity) {
		cout << "There is not enough items in this store";
		return;
	}
	
	cart.push({Items(items_pair_ptr->first.product, decrease), store});
	
	// искать в стеке не получится, поэтому отгрузим без сохранения (сохраним при оформлении заказа, при очищении корзины заново депозитнем)
	Product* product_ptr = items_pair_ptr->first.product;
	LogisticsCore::takeOut(product_ptr, decrease, seller, store, items_by_sellers);
	
	cout << items_pair_ptr->first.product->name << " (" << decrease << ") was added to your cart";
}

void showItems(
	map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers,
	map<uint32_t, Store>& stores,
	uint32_t seller_id)
{
	for (auto& [product_id, item_pair] : items_by_sellers.at(seller_id)) {
		cout << endl << item_pair.first.product->name << ": " << item_pair.first.quantity << "pcs" << endl;
		cout << "Stored in: ";
		for (auto& store_id : item_pair.second) {
			cout << stores.at(store_id).name << ' ';
		}
		cout << "Price: " << item_pair.first.product->price;
		cout << endl << endl;
	}
}

void searchItemsByName( 
	string query,
	map<uint32_t, Product>& products,
	map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers,
	map<uint32_t, Seller>& sellers,
	map<uint32_t, Store>& stores,
	stack<pair<Items, Store&>>& cart)
{
	vector<pair<Items, set<uint32_t>>*> items_ptrs;
	for (auto& [seller_id, seller_items] : items_by_sellers) {
		for (auto& [product_id, items_pair] : seller_items) {
			if (items_pair.first.product->name.find(query) != string::npos) {
				items_ptrs.push_back({&items_pair});
			}
		}
	}
	
	if (items_ptrs.empty()) {
		cout << "There is no items found, come check later" << endl;
		return;
	}
	
	int j = 0;
	for (int i = 1; i < items_ptrs.size(); ++i) {
		j = i - 1;
		while (j >= 0 && items_ptrs[j + 1]->first.product->price <  items_ptrs[j]->first.product->price) {
			swap(items_ptrs[j + 1], items_ptrs[j]);
			--j;
		}
	} // ascending

	cout << endl << "---- Search results ----" << endl;
	cout << "0. Cancel";
	uint32_t i = 1;
	for (auto& items_ptr : items_ptrs) {
		cout << endl;
		cout << i << ". " << items_ptr->first.product->name << endl;
		cout << "Price: " << items_ptr->first.product->price << "kekov" << endl;
		cout << "Seller: " << sellers.at(items_ptr->first.product->seller_id).name << endl;
		cout << "Quantity: " << items_ptr->first.quantity << endl;
		cout << "Stored in: ";
		for (auto& store_id : items_ptr->second) {
			cout << stores.at(store_id).name << ' ';
		}
		cout << endl;

		++i;
	}
	cout << " : ";

	uint32_t product_index;
	cin >> product_index;
	if (!product_index || product_index >= i) return;
	--product_index;
	
	showItemsDetails(items_ptrs[product_index], items_by_sellers, sellers, stores, cart);
}

void showAllItems(
	map<uint32_t, Product>& products,
	map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers,
	map<uint32_t, Seller>& sellers,
	map<uint32_t, Store>& stores,
	stack<pair<Items, Store&>>& cart)
{
	map<string, pair<vector<Product*>, uint32_t>> items_by_name; // нужно общее кол-во
	for (auto& [seller_id, seller_items] : items_by_sellers) {
		for (auto& [product_id, items_pair] : seller_items) {
			string product_name = items_pair.first.product->name;
			uint32_t items_quantity = items_pair.first.quantity;
			
			auto items_by_name_it = items_by_name.find(product_name);
			if (items_by_name_it != items_by_name.end()) {
				items_by_name_it->second.first.push_back(items_pair.first.product);
				items_by_name_it->second.second += items_quantity;
			}
			else {
				items_by_name.insert({product_name, {{items_pair.first.product}, items_quantity}});
			}
		}
	}
	
	cout << endl << "---- All items ----" << endl;
	cout << "0. Cancel";
	uint32_t i = 1;
	for (auto& [product_name, items_pair] : items_by_name) {
		cout << endl;
		uint32_t avg_price = 0;
		for (auto& product_ptr : items_pair.first) {
			avg_price += product_ptr->price;
		}
		avg_price /= items_pair.first.size();
		cout << i << ". " << product_name << endl;
		cout << "Average price: " << avg_price << "kekov" << endl;
		cout << "Quantity: " << items_pair.second << endl;
		++i;
	}
	cout << " : ";

	uint32_t product_index;
	cin >> product_index;
	if (!product_index || product_index >= i) return;
	--product_index;
	
	searchItemsByName(next(items_by_name.begin(), product_index)->first, products, items_by_sellers, sellers, stores, cart); // можно и лучше, но зачем
}

void addNewItem(
	map<uint32_t, Product>& products,
	map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers,
	uint32_t seller_id)
{
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
	Product* product_ptr = &(products.at(id));

	pair<uint32_t, pair<Items, set<uint32_t>>> new_items_entry{product_ptr->id, {Items(product_ptr, 0), {}}};
	auto seller_items_it = items_by_sellers.find(seller_id);
	if (seller_items_it != items_by_sellers.end()) {
		seller_items_it->second.insert(new_items_entry);
	}
	else {
		items_by_sellers.insert({seller_id, {new_items_entry}});
	}
	cout << "Done." << endl;
}

void changePrice(
	map<uint32_t, Product>& products,
	map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers,
	uint32_t seller_id)
{
	cout << endl << "--- Item selection ---" << endl << "0. Cancel" << endl;
	
	map<uint32_t, pair<Items, set<uint32_t>>>* seller_items_ptr = &(items_by_sellers.at(seller_id));
	uint32_t i = 1;
	for (auto& [product_id, pair] : *seller_items_ptr)
	{
		cout << i << ". " << pair.first.product->name << ": " << pair.first.product->price << "kekov" << endl;
		++i;
	}

	uint32_t item_index;
	cin >> item_index;
	if (!item_index || item_index > i - 1) return;
	else {
		--item_index;
		uint32_t new_price;
		cout << endl << "New price: ";
		cin >> new_price;
		Product* product_ptr = next(seller_items_ptr->begin(), item_index)->second.first.product;
		product_ptr->price = new_price;

		cout << "Done." << endl;
	}
}

void enterCart(stack<pair<Items, Store&>>& cart,
	map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers,
	map<uint32_t, Seller>& sellers,
	map<uint32_t, Store>& stores)
{
	for (;;) {
		cout << endl << "--- Cart ---" << endl;
		stack<pair<Items, Store&>> cart_dump = cart;

		while (!cart.empty()) {
			pair<Items, Store&> items_pair = cart.top();
			cout << endl << items_pair.first.product->name << ": " << items_pair.first.quantity << "pcs" << endl;
			cout << "Price: " << items_pair.first.product->price << endl;
			cout << "Seller: " << sellers.at(items_pair.first.product->seller_id).name << endl;
			Store& store = items_pair.second;
			cout << "Stored in: " << store.name;
			cout << "Address: " << store.address.index << ", " << store.address.city << ", " << store.address.street << ", " << store.address.house_number << endl;
			cout << endl;
			cart.pop();
		}

		cart = cart_dump;
		uint32_t flag;
		pair<Items, Store&> items_pair = cart.top();
		cout << "0. Cancel" << endl;
		cout << "1. Remove last items entry" << endl;
		cout << "2. Purchase" << endl << " : ";
		cin >> flag;
		switch (flag) {
			case 0:
				return;
			case 1:
				LogisticsCore::deposit(items_pair.first.product, items_pair.first.quantity, sellers.at(items_pair.first.product->seller_id), items_pair.second, items_by_sellers, false);
				cart.pop();
				break;
			case 2:
				SyncAPI::saveStores(stores);
				SyncAPI::saveSellers(sellers);
				cart = stack<pair<Items, Store&>>{};
				cout << "You've successfully purchased the items";
				return;
		}
	}
}

int enterShop(
	map<uint32_t, Product>& products,
	map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers,
	map<uint32_t, Store>& stores,
	map<uint32_t, Seller>& sellers)
{
	stack<pair<Items, Store&>> cart;
	uint32_t flag;
	string query;
	
	for (;;) {
		cout << endl << "--- Shop ---" << endl;
		cout << "1. View all items" << endl;
		cout << "2. Search item by name" << endl;
		cout << "3. Search seller by name" << endl;
		cout << "4. Enter the cart" << endl;
		cout << "5. Exit shop" << endl << " : ";
		cin >> flag;
		switch (flag) {
			case 1:
				showAllItems(products, items_by_sellers, sellers, stores, cart);
				break;
			case 2:
				cout << endl << "Enter your query: ";
				cin >> query;
				searchItemsByName(query, products, items_by_sellers, sellers, stores, cart);
				break;
			case 3:
			// показ товаров продавца то же самое что showItems(sellerId), но учитывать индекс надо
				break;
			case 4:
				enterCart(cart, items_by_sellers, sellers, stores);
			// не забудь, что чек = файл со всяким (прям в папку где exe создавай и норм)
				break;
			case 5:
				return 1;
		}
	}
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
		cout << "1. Enter the shop" << endl;
		cout << "2. Login as seller" << endl;
		cout << "3. Exit app" << endl << " : ";

		cin >> flag;
		switch (flag) {
			case 1:
				enterShop(products, items_by_sellers, stores, sellers);
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
		cout << "1. View items" << endl;
		cout << "2. Log out" << endl;
		cout << "3. Add new item" << endl;
		cout << "4. Change the price" << endl;
		cout << "5. Exit app" << endl << " : ";
		cin >> flag;
		switch (flag) {
			case 1:
				showItems(items_by_sellers, stores, seller_id);
				break;
			case 2:
				seller_id = 0;
				break;
			case 3:
				addNewItem(products, items_by_sellers, seller_id);
				SyncAPI::saveProducts(products);
				break;
			case 4:
				changePrice(products, items_by_sellers, seller_id);
				SyncAPI::saveProducts(products);
				break;
			case 5:
				return 1;
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