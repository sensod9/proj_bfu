#include "classLib.hpp"

Product::Product(uint32_t id, string name, double size, vector<string> consist, uint32_t price)
	: name(name), size(size), consist(consist), price(price)
{}

Store::Store(uint32_t id, string name, Address address, double capacity)
	: name(name), address(address), capacity(capacity)
{}
// TODO ADDRESS
Seller::Seller(uint32_t id, string name, vector<Items>* items, string login)
	: id(id), name(name), items(items), login(login) 
{}