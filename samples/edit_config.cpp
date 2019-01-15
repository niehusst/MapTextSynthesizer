#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

/*
reads through config.txt writing contents line by line to a new file
until finding seed parameter. Then it writes a different line to the new file
and then copies the rest of the original config into the new file.

g++ edit_config.cpp -o edit_config
*/
int main(int argc, char ** argv) {
	if(argc < 1) {
		cout << "Error, no command line argument" << endl;
		return 1;
	}

	int newSeed = atoi(argv[1]);
	
	string seedParam = "seed=";
	//open config file for reading
	filebuf in, out;
	in.open("config.txt", ios::in);
	out.open("newConfig.txt", ios::out);
	istream readFile(&in);
	ostream writeFile(&out);
	string line;

	while(getline(readFile, line)) {
		if(line == seedParam+to_string(newSeed-1)) {
			//write different line to writeFile
			writeFile << (seedParam + to_string(newSeed)) << endl;
		} else {
			//copy existing line into writeFile
			writeFile << line << endl;
		}
	}
	
	return 0;
}
