#ifndef MTS_UTILITIES_HPP
#define MTS_UTILITIES_HPP

#include <unordered_map>
#include <memory>


class MTS_Utilities{

public:

  // a map from user configured variable names to their values
  static std::shared_ptr<std::unordered_map<std::string, double> > params;


  /*
   * Parses a text file for variable names and values, using '='
   * as the delimeter, and places the data into parameter_map.
   *
   * parameter_map - the destination of the data. Map from 
   *                 variable name to variable value
   * filename - the name of the file to parse for values
   */
  static void parse(std::shared_ptr<std::unordered_map<std::string, double> > parameter_map, std::string filename);


  //constructor
  MTS_Utilities();

};

#endif // MTS_UTILITIES_HPP
