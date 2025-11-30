#include "../include/StoresAPI.hpp"

#include "../include/classLib.hpp"

namespace StoresAPI
{
	int add(Store& store, Product* product_ptr, uint32_t increase, bool newCreated)
	{
		uint32_t seller_id = product_ptr->seller_id;
		auto items_in_store_it = store.sellers_items.find(seller_id);
		if (items_in_store_it != store.sellers_items.end()) {
			vector<Items>& items_in_store = items_in_store_it->second;
			if (!newCreated) {
				bool isThereAny = false;
				for (auto& item : items_in_store) {
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
		}
		else {
			store.sellers_items.insert({seller_id, {Items(product_ptr, increase)}});
		}

		return 0;
	}
	
	int subtract(Store& store, Product* product_ptr, uint32_t decrease)
	{
		vector<Items>& items_in_store = store.sellers_items[product_ptr->seller_id];

		uint32_t item_index = 0;
		for (auto& item : items_in_store) {
			if (item.product == product_ptr) {
				break;
			}
			++item_index;
		}

		Items& selected_items = items_in_store[item_index];
		if (decrease < selected_items.quantity) {
			selected_items.quantity -= decrease;
		}
		else {
			if (items_in_store.size() <= 1) {
				store.sellers_items.erase(selected_items.product->seller_id);
			}
			else {
				items_in_store.erase(items_in_store.begin() + item_index);
			}
		}
		return 0;
	}
}