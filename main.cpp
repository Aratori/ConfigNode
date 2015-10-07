#include <fstream>
#include "ConfigNodeJson_impl.h"

using namespace std;

int main() {
	ConfigNodeJson	config;
	
	config.load("jsonExample.json");
	
	cout<<config.toString()<<endl;
}
