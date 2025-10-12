#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

class Product
{
public:
	Product(string name, double size, vector<string> consist);
	string name;
	double size;
	vector<string> consist;
private:
	size_t id;
};

class Store
{
public:
	Store(string name, string address, double capacity);
	string name;
	string address;
	double capacity;
private:
	size_t id;
};

class Seller
{
public:
	Seller(string login);
	string name;
	unordered_map<Product*, pair<int, int>> products; // <цена, кол-во>
	size_t getId();
private:
	size_t id;
	string login;
};