/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * map_text_synthesizer.cpp holds definitions for non-virtual methods of the  *
 * MapTextSynthesizer class, which is wrapped for use in python.              *
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

#include <memory>

#include "map_text_synthesizer.hpp"
#include "mts_implementation.hpp"

/*
namespace cv{
    namespace text{
*/
MapTextSynthesizer::MapTextSynthesizer(){}

std::shared_ptr<MapTextSynthesizer> MapTextSynthesizer::create(){
  std::shared_ptr<MapTextSynthesizer> mts(new MTSImplementation());
  return mts;
}
/*
    }  //namespace text
}  //namespace cv
*/

