#ifndef MTS_CONFIG_HPP
#define MTS_CONFIG_HPP

#include <unordered_map>
#include <memory>
#include <vector>

using namespace std;

class MTSConfig {
    private://----------------------- PRIVATE METHODS --------------------------

        /*
         * Parses a text file for variable names and values, using '='
         * as the delimeter, and places the data into parameter_map.
         *
         * filename - the name of the file to parse for values
         */
        std::unordered_map<std::string,std::string>
        parseConfig(std::string filename);

        std::unordered_map<std::string, std::string> params;
        std::unordered_map<std::string, int> paramsInt;
        std::unordered_map<std::string, double> paramsDouble;

    public://----------------------- PUBLIC METHODS --------------------------

  /*
   * Constructor for this class. Takes the file name to read user 
   * configured parameters from.
   *
   * filename - the name of the file from which to parse parameters from
   */
        MTSConfig(std::string filename);

  /*
   * Finds if the user configured parameter is in the params map.
   * Returns true if it is, otherwise false.
   *
   * key - the string name of the parameter to find
   */
        bool findParam(std::string key);

  /*
   * Get a parameter from the params map that is named by key. Returns
   * the pair value of the key.
   *
   * key - the string name of a parameter in params map
   */
        std::string getParam(std::string key);

  /*
   * Get a parameter from the paramsInt map that is named by key. Returns
   * the pair value of the key.
   *
   * key - the string name of a parameter in paramsInt map
   */
        int getParamInt(std::string key);

  /*
   * Get a parameter from the paramsDouble map that is named by key. Returns
   * the pair value of the key.
   *
   * key - the string name of a parameter in paramsDouble map
   */
        double getParamDouble(std::string key);
};

#endif
