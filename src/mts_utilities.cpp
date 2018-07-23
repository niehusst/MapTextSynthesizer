/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * mts_utilites.cpp contains class definitions for the MTS_Utilities class,   *
 * which is used to parse a text file that holds the user configurable        *
 * parameters.                                                                *
 * Copyright (C) 2018, Liam Niehus-Staab and Ziwen Chen                       *
 *                                                                            *
 * This program is free software: you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 3 of the License, or          *
 * (at your option) any later version.                                        * 
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

#include <unordered_map>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <memory>
#include <unistd.h>

#include "mts_utilities.hpp"


std::shared_ptr<std::unordered_map<std::string, double> > MTS_Utilities::params = std::make_shared<std::unordered_map<std::string, double> >();

MTS_Utilities::MTS_Utilities(){
  if (params->size() == 0) {
    parse(params, "config.txt");
  }
}


void MTS_Utilities::parse(std::shared_ptr<std::unordered_map<std::string, double> > parameter_map, std::string filename) {

  std::string delimiter = "=";

  // open file
  std::ifstream infile(filename);
  // if file failed to open, report error and exit
  if(!infile.is_open()) {
    std::cout << "The file " << filename << " could not be opened.\n";
    exit(1);
  }

  std::string line, key, value;
  double val;

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
    //CV_Assert(pos != line.npos);
                
    key = line.substr(0, pos);
    value = line.substr(pos+1, line.npos-pos);
    char * err_flag;
    val = strtod(value.c_str(), &err_flag); 
                
    // check if strtod produced error in casting
    if (value.c_str() == err_flag && val == 0) { 
      // tell user there was an error at this point and exit failure
      std::cout << "An unparseable value was encountered for variable "
                << key <<".\nPlease enter a valid number.\n";
      exit(1);
    }
                
    parameter_map->insert(std::pair<std::string, double>(key, val));
  }   
  // close file
  infile.close();
} 
