#pragma once

#include "classLib.hpp"
#include <set>

using namespace std;

namespace SyncAPI
{
	void loadProducts(map<uint32_t, Product>& products, map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers, string path = "data/products.csv");
	void loadStores(map<uint32_t, Store>& stores, map<uint32_t, Product>& products, map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers, string path = "data/storeId.txt");
	void loadSellers(map<uint32_t, Seller>& sellers, map<uint32_t, Store>& stores, string path = "data/sellerId.txt");

	void saveProducts(map<uint32_t, Product> products, string path = "data/products1.csv");
	void saveStores(map<uint32_t, Store> stores, string path = "data/storeId1.txt");
	void saveSellers(map<uint32_t, Seller> sellers, string path = "data/sellerId1.txt");
}