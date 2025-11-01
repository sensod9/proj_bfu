#include "classLib.hpp"

Product::Product(string name, double size, vector<string> consist, uint32_t price)
	: name(name), size(size), consist(consist), price(price)
{}

Store::Store(string name, Address address, double capacity, unordered_map<uint32_t, vector<Items>> sellers_items)
	: name(name), address(address), capacity(capacity), sellers_items(sellers_items)
{}
// TODO ADDRESS
Seller::Seller(string name, unordered_map<uint32_t, vector<Items>*> items)
	: name(name), items(items) 
{}

Items::Items(Product* product, uint32_t quantity)
	: product(product), quantity(quantity)
{}