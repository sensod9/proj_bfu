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
	Seller(string name, string login, string password);
	string name;
	vector<Product*> products;
private:
	size_t id;
	string login;
};