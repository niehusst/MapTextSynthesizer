#include <vector>
#include <stdlib.h>
#include <iostream> 
#include <fstream> 
#include <unordered_map>
#include <memory>

#include "mts_config.hpp"

using namespace std;

// SEE mts_config.hpp FOR ALL DOCUMENTATION

//strip the spaces in the front and end of the string
string strip(string str) {
    int i,j;
    for (i=0;i<str.length();i++) {
        if (str[i] != ' '){
            break;
        }
    }   
    for (j=str.length()-1;j>=0;j--) {
        if (str[j] != ' '){
            break;
        }
    }   
    if (i>j) {
        return ""; 
    } else {
        return str.substr(i,j-i+1);
    }   
}

unordered_map<string,string> 
MTSConfig::parseConfig(string filename) {

    unordered_map<string, string> params = unordered_map<string, string>();

    string delimiter = "=";

    // open file
    ifstream infile(filename);
    if (! infile.is_open()) {
        cerr << "config file cannot be openned!" << endl;
        exit(1);
    }

    string line, key, value;

    // parse file line by line
    while (getline(infile, line)) {
        // if line is empty or is a comment, erase it
        size_t com_pos = line.find("//");
        if (com_pos != line.npos) {
            line.erase(com_pos);
        }
        if (line.length()==0) {
            continue;
        }
        size_t pos = line.find(delimiter);
        if (pos == line.npos) {
cerr << "A line does not contain delimiter in config file!" << endl;
            exit(1);
        }

        key = strip(line.substr(0, pos));
        value = strip(line.substr(pos+1, line.npos-pos));

        /*
        char * err_flag;
        val = strtod(value.c_str(), &err_flag);

        // check if strtod produced error in casting
        if (value.c_str() == err_flag && val == 0) {
            // tell user there was an error at this point and exit failure
            cout << "An unparseable value was encountered for variable "
                << key <<".\nPlease enter a valid number.\n";
            exit(1);
        }
        */
        params.insert(pair<string, string>(key, value));

    }
    // close file
    infile.close();

    return params;
}


MTSConfig::MTSConfig(string filename){
    params = parseConfig(filename);
    paramsInt = unordered_map<string, int>();
    paramsDouble = unordered_map<string, double>();
}

string
MTSConfig::getParam(string key) {
    if (params.find(key) != params.end()) {
        return params.find(key)->second;
    } else {
        cerr << "key " << key << " does not exist in config file!" << endl;
        exit(1);
    }
}

int
MTSConfig::getParamInt(string key) {
    if (paramsInt.find(key) != paramsInt.end()) {
        return paramsInt.find(key)->second;
    } else {
        string value = getParam(key);
        char *endptr;
        int val = strtol(value.c_str(), &endptr, 10);
        if (endptr[0] != '\0') {
            cerr << key << " must be an integer!" << endl;
            exit(1);
        }
        paramsInt.insert(pair<string, int>(key, val));
        return val;
    }
}

double
MTSConfig::getParamDouble(string key) {
    if (paramsDouble.find(key) != paramsDouble.end()) {
        return paramsDouble.find(key)->second;
    } else {
        string value = getParam(key);
        char *endptr;
        double val = strtod(value.c_str(), &endptr);
        if (endptr[0] != '\0') {
            cerr << key << " must be a double!" << endl;
            exit(1);
        }
        paramsDouble.insert(pair<string, double>(key, val));
        return val;
    }
}
