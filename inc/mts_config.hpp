#ifndef MTS_CONFIG_HPP
#define MTS_CONFIG_HPP

#include <unordered_map>
#include <memory>
#include <vector>

using std::string;
using std::vector;
using std::unordered_map;

class MTSConfig {
    private://----------------------- PRIVATE METHODS --------------------------

        /*
         * Parses a text file for variable names and values, using '='
         * as the delimeter, and places the data into parameter_map.
         *
         * filename - the name of the file to parse for values
         */
        unordered_map<string,string> parseConfig(string filename);

        unordered_map<string, string> params;
        unordered_map<string, int> paramsInt;
        unordered_map<string, double> paramsDouble;

    public://----------------------- PUBLIC METHODS --------------------------

        MTSConfig(string filename);

        bool findParam(string key);

        string getParam(string key);

        int getParamInt(string key);

        double getParamDouble(string key);
};

#endif
