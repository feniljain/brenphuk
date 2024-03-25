#include <iostream>
#include <string>
#include <unistd.h>

int main() {
	std::string name;
	std::cout << "> enter your name: ";
	std::getline(std::cin, name);
	std::string greeting = "Namastey, "+name+"!\n";
	
	write(1, greeting.c_str(), greeting.size());
}
