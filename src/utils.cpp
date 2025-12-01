#include "../include/utils.hpp"

#include <string>
#include <vector>
#include <sstream>

using namespace std;

vector<string> splitToVector(string s, char delim)
{
	vector<string> v;
	string temp;
	stringstream ss(s);
	for (int i = 0; !ss.eof(); ++i) {
		getline(ss, temp, delim);
		v.push_back(temp);
	}
	return v;
}