#include <fstream>
#include "ConfigNodeJson.cpp"
#include <string>

using namespace std;

void change(string& str);

int main() {
	ConfigNodeJson	config;
	std::string 	str = "hello";
	
	change(str);
	
	cout<<str<<endl;
	
	config.load("jsonExample.json");
	
	cout<<config.toString()<<endl;
}

void change(string& str)
{
	for(int i = 0; i < 10; i++)
		str.push_back('f');
}
