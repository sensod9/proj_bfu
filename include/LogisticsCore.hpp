#pragma once

#include "../include/classLib.hpp"
#include <set>

namespace LogisticsCore
{
	int depositItems(
		Product* product_ptr,
		uint32_t increase,
		Seller& seller,
		Store& store,
		map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers,
		bool newCreated = 0);

	int takeOutItems(
			Product* product_ptr,
			uint32_t decrease,
			Seller& seller,
			Store& store,
			map<uint32_t, map<uint32_t, pair<Items, set<uint32_t>>>>& items_by_sellers);
}