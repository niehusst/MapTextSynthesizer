#ifndef MTS_CONFIG_HPP
#define MTS_CONFIG_HPP

#include <unordered_map>
#include <memory>
#include <vector>

using namespace std;

class MTSConfig {
    private://----------------------- PRIVATE METHODS --------------------------

        unordered_map<string,string> parseConfig(string filename);

        unordered_map<string, string> params;
        unordered_map<string, int> paramsInt;
        unordered_map<string, double> paramsDouble;

    public://----------------------- PUBLIC METHODS --------------------------

        MTSConfig(string filename);

        string getParam(string key);

        int getParamInt(string key);

        double getParamDouble(string key);
};

#endif
