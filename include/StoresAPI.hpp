#pragma once

#include "../include/classLib.hpp"

namespace StoresAPI {
	int add(Store& store, Product* product_ptr, uint32_t increase, bool newCreated = 0);

	int subtract(Store& store, Product* product_ptr, uint32_t decrease);
}