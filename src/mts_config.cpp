#include <vector>
#include <stdlib.h>
#include <iostream> 
#include <fstream> 
#include <unordered_map>
#include <memory>

#include "mts_config.hpp"
#include "mts_basehelper.hpp"

using namespace std;

// SEE mts_config.hpp FOR ALL DOCUMENTATION


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
        if (MTS_BaseHelper::strip(line).length()==0) {
            continue;
        }
        size_t pos = line.find(delimiter);
        if (pos == line.npos) {
cerr << "A line does not contain delimiter in config file!" << endl;
            exit(1);
        }

        key = MTS_BaseHelper::strip(line.substr(0, pos));
        value = MTS_BaseHelper::strip(line.substr(pos+1, line.npos-pos));

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

bool
MTSConfig::findParam(string key) {
    return params.find(key) != params.end();
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
