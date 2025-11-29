#include <iostream>
#include <fstream>

#include "../include/login.hpp"
#include "../include/utils.hpp"

uint32_t loginSellerProc(string path) {
	ifstream in(path);
	vector<vector<string>> sellers; 
	string line;
	while (getline(in, line))
	{
		sellers.push_back(splitToVector(line, '|'));
	}
	in.close();

	hash<string> hasher;
	for (;;) {
		string login, password, salt = "lololol999";

		cout << endl << "Login: "; 
		cin >> login;
		if (login == "0") return 0;
		cout << "Password: "; 
		cin >> password;
		uint64_t h_login = hasher(login + salt);
		uint64_t h_password = hasher(password + salt);
		for (auto& seller : sellers) {
			if (h_login == stoull(seller[0]) && h_password == stoull(seller[1])) {
				return stoull(seller[2]);				
			}
		}
		cout << "Incorrect. Try Again" << endl;
	}
}