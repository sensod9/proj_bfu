#include <map>
#include <vector>
#include <string>

using namespace std;

class Product
{
public:
	Product(uint32_t id, string name, double size, vector<string> consist, uint32_t price);
	uint32_t id;
	string name;
	double size;
	vector<string> consist;
	uint32_t price;
};

struct Address {
	string index;
	string city;
	string street;
	uint32_t house_number;
};

struct Items {
	Items(Product* product, uint32_t quantity);
	Product* product;
	uint32_t quantity;
};

class Store
{
public:
	Store(string name, Address address, double capacity, map<uint32_t, vector<Items>> sellers_items);
	string name;
	Address address;
	double capacity;
	map<uint32_t, vector<Items>> sellers_items; // id,Items
};

class Seller
{
public:
	Seller(string name, map<uint32_t, vector<Items>*> items);
	string name;
	map<uint32_t, vector<Items>*> store_items; // store id, Items
	string login;
};