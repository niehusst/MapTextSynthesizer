/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * map_text_synthesizer.cpp holds definitions for non-virtual methods of the  *
 * MapTextSynthesizer class.                                                  *
 *                                                                            *
 * Copyright (C) 2018                                                         *
 *                                                                            *
 * Written by Ziwen Chen <chenziwe@grinnell.edu>                              * 
 * and Liam Niehus-Staab <niehusst@grinnell.edu>                              *
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

#include <fstream>
#include <string>
#include <iostream>

#include "mtsynth/map_text_synthesizer.hpp"
#include "mts_implementation.hpp"

using std::string;
using cv::Mat;
using cv::Ptr;

MapTextSynthesizer::MapTextSynthesizer(){}

Ptr<MapTextSynthesizer> MapTextSynthesizer::create(string config_file){
    Ptr<MapTextSynthesizer> mts(new MTSImplementation(config_file));
    return mts;
}
