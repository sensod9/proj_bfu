#include "classLib.hpp"

Product::Product(string name, double size, vector<string> consist)
	: name(name), size(size), consist(consist)
{}

Store::Store(string name, string address, double capacity)
	: name(name), address(address), capacity(capacity)
{}

Seller::Seller(string name, string login, string password)
	: name(name)
{
	

}