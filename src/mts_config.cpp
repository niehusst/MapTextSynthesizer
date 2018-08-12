#include <vector>
#include <stdlib.h>
#include <iostream> 
#include <fstream> 
#include <unordered_map>
#include <memory>

#include "mts_config.hpp"
#include "mts_basehelper.hpp"


// SEE mts_config.hpp FOR ALL DOCUMENTATION


std::unordered_map<std::string,std::string> 
MTSConfig::parseConfig(std::string filename) {

    std::unordered_map<std::string, std::string> params = std::unordered_map<std::string, std::string>();

    std::string delimiter = "=";

    // open file
    ifstream infile(filename);
    if (! infile.is_open()) {
        std::cerr << "The input config file could not be opened!" << std::endl;
        exit(1);
    }

    std::string line, key, value;
    int line_number;
    // parse file line by line
    while (getline(infile, line)) {
        line_number++;
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
            std::cerr << "Line " << line_number
                      << " in config file does not contain delimiter!\n";
            exit(1);
        }

        key = MTS_BaseHelper::strip(line.substr(0, pos));
        value = MTS_BaseHelper::strip(line.substr(pos+1, line.npos-pos));

        params.insert(std::pair<std::string, std::string>(key, value));
    }
    // close file
    infile.close();

    return params;
}


MTSConfig::MTSConfig(std::string filename){
    params = parseConfig(filename);
    paramsInt = std::unordered_map<std::string, int>();
    paramsDouble = std::unordered_map<std::string, double>();
}

bool
MTSConfig::findParam(std::string key) {
    return params.find(key) != params.end();
}

std::string
MTSConfig::getParam(std::string key) {
    if (params.find(key) != params.end()) {
        return params.find(key)->second;
    } else {
        std::cerr << "Parameter " << key
                  << " does not exist in config file!" << std::endl;
        exit(1);
    }
}

int
MTSConfig::getParamInt(std::string key) {
    if (paramsInt.find(key) != paramsInt.end()) {
        return paramsInt.find(key)->second;
    } else {
        std::string value = getParam(key);
        char *endptr;
        int val = strtol(value.c_str(), &endptr, 10);
        if (endptr[0] != '\0') {
            std::cerr << "Config file parameter " << key
                      << " must be an integer!" << std::endl;
            exit(1);
        }
        paramsInt.insert(std::pair<std::string, int>(key, val));
        return val;
    }
}

double
MTSConfig::getParamDouble(std::string key) {
    if (paramsDouble.find(key) != paramsDouble.end()) {
        return paramsDouble.find(key)->second;
    } else {
        std::string value = getParam(key);
        char *endptr;
        double val = strtod(value.c_str(), &endptr);
        if (endptr[0] != '\0') {
            std::cerr << "Config file parameter " << key << " must be a double!"
                      << std::endl;
            exit(1);
        }
        paramsDouble.insert(std::pair<std::string, double>(key, val));
        return val;
    }
}
