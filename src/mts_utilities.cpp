#include <unordered_map>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <memory>
#include <unistd.h>

#include "text/mts_utilities.hpp"


using namespace std;

namespace cv{ 
    namespace text{

        shared_ptr<unordered_map<string, double> > MTS_Utilities::params = make_shared<unordered_map<string, double> >();


        MTS_Utilities::MTS_Utilities(){
            if (params->size() == 0) {
                parse(params, "config.txt");
            }
        }


         void MTS_Utilities::parse(shared_ptr<unordered_map<string, double> >parameter_map, string filename) {
            cout << "in parse" << endl;

            string delimiter = "=";

            // open file
            ifstream infile(filename);
            CV_Assert(infile.is_open());

            string line, key, value;
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
                CV_Assert(pos != line.npos);
                
                key = line.substr(0, pos);
                value = line.substr(pos+1, line.npos-pos);
                char * err_flag;
                val = strtod(value.c_str(), &err_flag); 
                
                // check if strtod produced error in casting
                if (value.c_str() == err_flag && val == 0) { 
                  // tell user there was an error at this point and exit failure
                  cout << "An unparseable value was encountered for variable "
                       << key <<".\nPlease enter a valid number.\n";
                  exit(1);
                }
                
                parameter_map->insert(pair<string, double>(key, val));
            }   
            // close file
            infile.close();
        } 

    }  //namespace text
}  //namespace cv

