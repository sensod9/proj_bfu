#include "../include/LogisticsCore.hpp"

#include "../include/classLib.hpp"
#include "../include/StoresAPI.hpp"
#include <set>

namespace LogisticsCore
{
	int deposit(Product* product_ptr, uint32_t increase, Seller& seller, Store& store, map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers, bool newCreated)
	{
		bool canBeAdded = (store.capacity - store.size >= (product_ptr->size * increase)) ? true : false;
		
		uint32_t seller_id = product_ptr->seller_id;
		if (newCreated) {
			if (!canBeAdded)
				increase = 0; // добавляем в список продавца, но на склад не отгружаем
			pair<uint32_t, pair<Items, set<uint32_t>>> new_items_entry{product_ptr->id, {Items(product_ptr, increase), {store.id}}};
			auto seller_items_it = items_by_sellers.find(seller_id);
			if (seller_items_it != items_by_sellers.end()) {
				seller_items_it->second.insert(new_items_entry);
			}
			else {
				items_by_sellers.insert({seller_id, {new_items_entry}});
			}
			if (!canBeAdded) return -1;
		}
		else {
			if (!canBeAdded) return -1; 
			items_by_sellers.at(seller_id).at(product_ptr->id).first.quantity += increase;
			items_by_sellers.at(seller_id).at(product_ptr->id).second.insert(store.id);
			// next(items_by_sellers[seller_id].begin(), item_index)->second.first.quantity += increase;
		}
		
		auto seller_store_items_it = seller.store_items.find(store.id);
		if (seller_store_items_it == seller.store_items.end())
			seller.store_items.insert({store.id, &(store.sellers_items[seller_id])});			

		StoresAPI::add(store, product_ptr, increase);
		return 0;
	}

	int takeOut(Product* product_ptr, uint32_t decrease, Seller& seller, Store& store, map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers)
	{
		if (!decrease) return -1;
		uint32_t seller_id = product_ptr->seller_id;

		vector<Items>& items_in_store = store.sellers_items[seller_id];

		uint32_t item_index = 0;
		for (auto& item : items_in_store) {
			if (item.product == product_ptr) {
				break;
			}
			++item_index;
		}
		uint32_t full_quantity = items_in_store[item_index].quantity;

		if (decrease < full_quantity)
		{
			items_by_sellers.at(seller_id).at(product_ptr->id).first.quantity -= decrease;
		}
		else {
			items_by_sellers.at(seller_id).at(product_ptr->id).first.quantity -= full_quantity;
			items_by_sellers.at(seller_id).at(product_ptr->id).second.erase(store.id);
			if (items_in_store.size() <= 1) {
				seller.store_items.erase(store.id);
			}
		}
		
		StoresAPI::subtract(store, product_ptr, decrease);
		return 0;
	}
}