#include "classLib.hpp"

Product::Product(uint32_t id, string name, double size, vector<string> consist, uint32_t price)
	: id(id), name(name), size(size), consist(consist), price(price)
{}

Store::Store(string name, Address address, double capacity, map<uint32_t, vector<Items>> sellers_items)
	: name(name), address(address), capacity(capacity), sellers_items(sellers_items)
{}

Seller::Seller(string name, map<uint32_t, vector<Items>*> store_items)
	: name(name), store_items(store_items) 
{}

Items::Items(Product* product, uint32_t quantity)
	: product(product), quantity(quantity)
{}