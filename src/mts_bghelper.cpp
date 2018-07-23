/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * mts_bghelper.cpp contains the class method definitions for the             *
 * MTS_BackgroundHelper class, which handles background synthesization.       *
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

#include <pango/pangocairo.h>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <string>

#include "mts_bghelper.hpp"
#include "mts_basehelper.hpp"

using boost::random::beta_distribution;
using boost::random::normal_distribution;
using boost::random::gamma_distribution;
using boost::random::variate_generator;


// SEE mts_bghelper.hpp FOR ALL DOCUMENTATION

double
MTS_BackgroundHelper::getParam(std::string key) {
  return helper->getParam(key);
}

MTS_BackgroundHelper::MTS_BackgroundHelper(std::shared_ptr<MTS_BaseHelper> h)
  :helper(h),  // initialize fields
   bias_var_dist(h->getParam(std::string("bias_std_alpha")),
                 h->getParam(std::string("bias_std_beta"))),
   bias_var_gen(h->rng2_, bias_var_dist),
   texture_distribution(h->getParam(std::string("texture_width_alpha")), 
                        h->getParam(std::string("texture_width_beta"))),
   texture_distrib_gen(h->rng2_, texture_distribution)
{}

void
MTS_BackgroundHelper::make_dash_pattern(double * pattern, int len) {
  double dash;

  //set pattern randomly
  for(int i = 0; i < len; i++) {
    dash = (helper->rng() % 10000) / 1000.0;
    pattern[i] = dash;
  }
}


void
MTS_BackgroundHelper::draw_boundary(cairo_t *cr, double linewidth, 
                                    double og_col, bool horizontal) {

  // get original dash code
  int dash_len = cairo_get_dash_count(cr);
  double dash[dash_len], *offset;
  cairo_get_dash(cr, dash, offset);

  // calculate a distance between lines
  int dist_min = getParam(std::string("boundary_distance_min"));
  int dist_max = getParam(std::string("boundary_distance_max"))+1 - dist_min;
  double distance = (dist_min + helper->rng() % dist_max) * linewidth;

  // set boundary line characteristics
  int width_min = getParam(std::string("boundary_linewidth_min"));
  int width_max = getParam(std::string("boundary_linewidth_max"))+1 - width_min;
  double new_linewidth = linewidth * (width_min + helper->rng() % width_max);
  cairo_set_line_width(cr, new_linewidth);
  cairo_set_dash(cr, dash, 0,0); //set dash pattern to none

  // set boundary line gray-scale color (lighter than original)
  int color_min = 100 * getParam(std::string("boundary_color_diff_min"));
  int color_max = 100 * getParam(std::string("boundary_color_diff_max"))+1 - color_min;
  double color_diff = (color_min + helper->rng() % color_max) / 100;
  double color = og_col + color_diff;
  cairo_set_source_rgb(cr, color, color, color);

  // stroke the boundary line
  cairo_stroke_preserve(cr);

  // translate distance so that main line is drawn off center of boundary
  draw_parallel(cr, horizontal, distance, false); //doesn't stroke new path

  // reset to color and line width of original line
  cairo_set_line_width(cr, linewidth);
  cairo_set_source_rgb(cr, og_col, og_col, og_col);
  cairo_set_dash(cr, dash, dash_len, 0);
}


void
MTS_BackgroundHelper::draw_hatched(cairo_t *cr, double linewidth) {

  //set width of hatches (in multiples of original linewidth)
  int width_min = getParam(std::string("railroad_cross_width_min"));
  int width_max = getParam(std::string("railroad_cross_width_max"))+1 - width_min;
  double wide = (width_min + (helper->rng() % width_max)) * linewidth;
  cairo_set_line_width(cr, wide);

  // set distance between hatches
  int dist_min = getParam(std::string("railroad_distance_between_crosses_min"));
  int dist_max = getParam(std::string("railroad_distance_between_crosses_max"))+1 - dist_min;

  // set apparent width of hatches
  double on_len = 3 * linewidth / (1 + (helper->rng() % 5)); 
  // set apparent distance between hatches
  double off_len = dist_min + (helper->rng() % (int) ceil(dist_max * on_len)); 

  //set dash pattern to be used
  const double pattern[] = { on_len, off_len};
  cairo_set_dash(cr, pattern, 2, 0);
  cairo_stroke_preserve(cr);

  // return to original settings
  cairo_set_line_width(cr, linewidth);
  cairo_set_dash(cr, pattern, 0, 0); 
}



void
MTS_BackgroundHelper::draw_parallel(cairo_t *cr, bool horizontal, 
                                    double distance, bool stroke) {

  cairo_path_t *path;
  cairo_path_data_t *data;
  double xtrans = 0, ytrans = 0;

  //copy current path
  path = cairo_copy_path(cr);

  if(horizontal) { // line horizontal, translate new path in y direction
    ytrans = distance;
    helper->manual_translate(cr, path, data, xtrans, ytrans); 

  } else { // line vertical, translate new path in x direction
    xtrans = distance; 
    helper->manual_translate(cr, path, data, xtrans, ytrans); 
  }

  //stroke new parallel line if stroke flag is true
  if(stroke) cairo_stroke(cr);

  //destroy used up path
  cairo_path_destroy(path);
}

void
MTS_BackgroundHelper::set_dash_pattern(cairo_t *cr) {
  //set length of pattern (1 - 6)
  int pattern_len = 1 + (helper->rng() % 6);
  double dash_pattern[pattern_len];

  //make and set pattern
  make_dash_pattern(dash_pattern, pattern_len);
  cairo_set_dash(cr, dash_pattern, pattern_len, 0);
}

coords
MTS_BackgroundHelper::orient_path(cairo_t *cr, bool horizontal, bool curved, 
                                  int width, int height) {
  double x,y,angle;
  int translation_x, translation_y;

  // randomly set translation and rotation based on orientation 
  if(horizontal) { //horizontal line orientation
    //make a starting point for straight lines
    x = 0;
    y = height - (helper->rng() % height); // y variation of 0-height along left side

    //make a starting translation and angle
    angle = ((helper->rng() % 7854) - (helper->rng() % 7854))/10000.0; //get angle +- PI/4
    translation_x = (helper->rng() % width) - (helper->rng() % width); // +- width
    if(curved) { translation_y = -fabs(y); } // translate points up from bottom
    else { translation_y = 0; }

    cairo_translate(cr, translation_x, translation_y);
    cairo_rotate(cr, angle); 

  } else { //vertical line orientation
    //make a starting point for straight lines
    y = 0;
    x = width - (helper->rng() % width); // x variation of 0-width along top side

    //make a starting translation and angle
    angle = -((3 * 7854) - 2 * (helper->rng() % 7854))/10000.0; // from PI/4 to 3PI/4

    /*translate to approximate curved line center point, rotate around it, 
      translate back */
    if(curved) {
      // try to keep xy translations in bounds of surface. finiky
      translation_y = -height/2.0 + (helper->rng() % (int) ceil(height/2.0)) 
        - (helper->rng() % (int) ceil(height/2.0));
      translation_x = (helper->rng() % (int) ceil(width/2.0)); 

      cairo_translate(cr, width/2.0, height);
      cairo_rotate(cr, angle); 
      cairo_translate(cr, translation_x, translation_y);

    } else { //if line is straight, just translate 
      translation_x = 0;
      translation_y = (helper->rng() % (int) ceil(height)) 
        - (helper->rng() % (int) ceil(height)); // +- height

      cairo_translate(cr, translation_x, translation_y);
    }
  }
  //set and return starting coordinats
  coords start(x,y);
  return start;
}

void
MTS_BackgroundHelper::generate_curve(cairo_t *cr, bool horizontal, int width, 
                                     int height,  double c_min, double c_max, double d_min, double d_max) {

  std::vector<coords> points;
  PangoLayout *layout;
  cairo_path_t *path;
  PangoLayoutLine *line;
  int num_points = 3 + (helper->rng() % 12); // make from 3 to 15 points 

  //get correct point vector based on line orientation
  if(horizontal) {
    points = helper->make_points_wave(width, height, num_points);

  } else {
    points = helper->make_points_wave(height, height, num_points);
  } 

  //curve the path and give extra optional parameter to stroke the path
  helper->create_curved_path(cr,path,line,layout,width,
                             height,0,0,points,c_min,c_max,d_min,d_max,true);
}


void
MTS_BackgroundHelper::addLines(cairo_t *cr, bool boundary, bool hatched, 
                               bool dashed, bool curved, bool doubleline, 
                               bool horizontal, int width, int height, 
                               double color, double c_min, double c_max, double d_min, double d_max){

  double magic_line_ratio, line_width;
  coords start_point;

  //set line color and width
  cairo_set_source_rgb(cr, color, color, color); // gray-scale
  // set ratio to keep line scaled for image size
  int ratio_min = (int)(getParam("line_width_scale_min") * 10000);
  int ratio_max = 1 + (int)(getParam("line_width_scale_max") * 10000) - ratio_min;
  //cout << ratio_min << " " << ratio_max << endl;
  magic_line_ratio = (ratio_min + helper->rng() % ratio_max) / 10000.0; 
  //cout << "line width ratio " << magic_line_ratio << endl;
  line_width = std::min(width, height) * magic_line_ratio;
  cairo_set_line_width(cr, line_width);

  //move to origin of surface
  cairo_move_to(cr, 0, 0);

  //orient the path for the line correctly
  start_point = orient_path(cr, horizontal, curved, width, height);

  // set path shape 
  if(curved) { 
    // draw a wiggly line
    generate_curve(cr, horizontal, width, height, c_min, c_max, d_min, d_max);

  } else { // draw a straight line
    // move to starting point
    cairo_move_to(cr, start_point.first, start_point.second); 
    if(horizontal) {
      // make a line to width and a random height
      cairo_line_to(cr, width, helper->rng() % height); 
    } else { //vertical 
      // make a line to height and a random width
      cairo_line_to(cr, helper->rng() % width, height); 
    }
  } 

  // set line style to dashed or not (default solid)
  if(dashed) { set_dash_pattern(cr); } 

  // set boundry or not
  if(boundary) { draw_boundary(cr, line_width, color, horizontal); }

  // set hatching or not
  if(hatched) { draw_hatched(cr, line_width); } 

  // draw parallel or not
  if(doubleline) { draw_parallel(cr, horizontal, 3*line_width); }

  //stroke
  cairo_stroke(cr);

  //set rotations and translations back to normal
  cairo_identity_matrix(cr); 
}


void
MTS_BackgroundHelper::diagonal_lines(cairo_t *cr, int x1, int y1, int x2, int y2,
                                     int spacing, int width, int height) {

  // increment point positions
  if (y1 >= height && x2 < width){
    x1 += spacing;
    x2 += spacing;
  } else if (x2 >= width && y1 >= height) {
    y2 += spacing;
    x1 += spacing;
  } else if (x2 >= width && y1 < height) {
    y1 += spacing;
    y2 += spacing;
  } else { 
    y1 += spacing;
    x2 += spacing;
  }

  // draw diagonal line
  cairo_move_to(cr, x1, y1);
  cairo_line_to(cr, x2, y2);

  //if both points have reached the corner opposite the origin, return
  if (x1 >= width && y2 >= height) return;  

  // otherwise recurse to draw another line
  diagonal_lines(cr, x1, y1, x2, y2, spacing, width, height);
}



void
MTS_BackgroundHelper::crossed_lines(cairo_t *cr, int x1, int y1, int x2, int y2, int spacing, int width, int height) {
  // draw lines 
  diagonal_lines(cr, x1, y1, x2, y2, spacing, width, height);

  // draw lines perpendicular to existing lines
  cairo_translate(cr, width, 0);
  cairo_rotate(cr, M_PI/2);
  // (width and height switched to account for rotation)
  diagonal_lines(cr, x1, y1, x2, y2, spacing, height, width);

  //return matrix transformations to normal
  cairo_identity_matrix(cr);
}



double
MTS_BackgroundHelper::get_edge_len(int num_sides, int radius) {
  double circumference = 2 * M_PI * radius;
  return circumference / num_sides;
}


void
MTS_BackgroundHelper::shape_helper(cairo_t *cr, double x, double y, double angle, double edge_len, int num_sides, int counter) {
  if (num_sides-1 == counter) {
    // close shape and end recursion
    cairo_close_path(cr);
    return;

  } else { // draw the next edge
    // polar coordinates conversion to calculate next point
    double increase = counter * angle; 
    x = edge_len * cos(increase) + x;
    y = edge_len * sin(increase) + y;
    cairo_line_to(cr, x, y);

    //recurse
    shape_helper(cr, x, y, angle, edge_len, num_sides, counter+1);
  }
}


void 
MTS_BackgroundHelper::draw_shape(cairo_t *cr, int x, int y, int num_sides, int radius) {

  if (num_sides < 3) { //shape is a circle
    // draw circle
    cairo_arc(cr, x, y, radius, 0, 2*M_PI);

  } else { //shape is not a circle
    // calculate the angle between edges and edge length of the shape
    double total = (num_sides - 2) * M_PI;
    double internal_angle =  total / num_sides;
    double outer_angle = M_PI - internal_angle;
    double edge_len = get_edge_len(num_sides, radius);

    // move to start coords and draw geometric shape
    cairo_move_to(cr, x, y);
    shape_helper(cr, x, y, outer_angle, edge_len, num_sides, 0);
  }
}


void
MTS_BackgroundHelper::shape_texture_helper(cairo_t *cr, int origin_x, int origin_y, int num_sides, int distance, bool even, int radius, int width, int height) {

  // if dot origin has reached opposite corner from start, return
  if (origin_x >= width && origin_y >= height) return;

  // draw shape
  draw_shape(cr, origin_x, origin_y, num_sides, radius);
  cairo_fill(cr);  

  // if origin has reached edge, set it back and start new row
  if(origin_x >= width) {
    even? origin_x = -distance/2 : origin_x = 0;
    origin_y += distance;
    even = !even;

  } else { // otherwise move origin along row, towards edge
    origin_x += distance;
  }

  //recurse
  shape_texture_helper(cr, origin_x, origin_y, num_sides, distance, even, radius, width, height);
}



void
MTS_BackgroundHelper::make_shape_texture(cairo_t *cr, int x, int y, int diameter, 
                                         int num_sides, int spacing, int width, int height) {
  // call recursive circle helper to draw texture
  shape_texture_helper(cr, x, y, num_sides, spacing, true, diameter/2, width, height);
}


void
MTS_BackgroundHelper::draw_texture(cairo_t *cr, int texture, double brightness, double linewidth,
                                   int diameter, int num_sides, int spacing, int width, int height) {

  //verify preconditions
  if (spacing < 1) spacing = 1;
  if (texture != 2) diameter = 0, num_sides = 0;

  //set drawing source color and line width of texture
  cairo_set_source_rgb(cr, brightness,brightness,brightness);        
  cairo_set_line_width(cr, linewidth);                 

  // a vector of texture drawing function pointers
  void (*texture_func)(cairo_t*, int, int, int, int, int, int, int);
  std::vector<void (*)(cairo_t*, int, int, int, int, int, int, int)> texture_vec;

  // populate vector with all texture drawing functions
  texture_vec.push_back(&diagonal_lines);
  texture_vec.push_back(&crossed_lines);
  texture_vec.push_back(&make_shape_texture);

  // draw texture 
  (*texture_vec[texture])(cr, 0, 0, diameter, num_sides, spacing, width, height);

  // apply texture to surface 
  cairo_stroke(cr);
}


void
MTS_BackgroundHelper::set_texture_source(cairo_t *cr, int texture, double brightness, double linewidth, int spacing, int width, int height) {
  cairo_t *cr_new;
  cairo_surface_t *surface_m;
  int diameter = 0, num_sides = 0;

  // if shapes texture is chosen, set shape related parameters
  if (texture == 2) {
    spacing *= 2;
    diameter = 2 + helper->rng() % 40;  // range 2 - 41
    num_sides = 2+ helper->rng() % 8;   // circles through nonagon 
    if(spacing < diameter) spacing = diameter; // verify preconditions
  }

  //create new surface and context to hold the texture for the source
  surface_m = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 
                                          width, height);
  cr_new = cairo_create(surface_m);

  // make texture on new surface
  draw_texture(cr_new, texture, brightness, linewidth, diameter, num_sides, spacing, width, height);

  // set texture as source
  cairo_set_source_surface(cr, surface_m, 0,0);

  // clean up
  cairo_destroy(cr_new);
  cairo_surface_destroy(surface_m);
}


void
MTS_BackgroundHelper::addTexture(cairo_t *cr, bool curved, bool horizontal, double brightness, int width, int height, double c_min, double c_max, double d_min, double d_max) {
  // set adequate spacing between lines in texture
  int spacing = std::max(4, width/100);
  spacing = spacing + helper->rng() % (2*spacing);  

  // linewidth range (1/3)width - (1/10)width
  int linewidth = (1.0 / (3 + texture_distrib_gen() * 8)) * width;
  int texture = helper->rng() % 3; // range 0-2
  coords start_point;

  // set source to correct texture and make line thick & rounded
  set_texture_source(cr, texture, brightness, 1, spacing, width, height); 
  cairo_set_line_width(cr, linewidth);
  cairo_set_line_cap  (cr, CAIRO_LINE_CAP_ROUND);

  //orient the path for the line correctly
  start_point = orient_path(cr, horizontal, curved, width, height);

  // set path shape 
  if(curved) { 
    // draw a wiggly line
    generate_curve(cr, horizontal, width, height, c_min,c_max,d_min,d_max);

  } else { // draw a straight line
    // move to starting point
    cairo_move_to(cr, start_point.first, start_point.second); 
    if(horizontal) {
      // make a line to width and a random height
      cairo_line_to(cr, width, helper->rng() % height); 
    } else { //vertical 
      // make a line to height and a random width
      cairo_line_to(cr, helper->rng() % width, height); 
    }
  } 

  // stroke lines to surface
  cairo_stroke(cr);

  // reset transformation matrix
  cairo_identity_matrix(cr);
}


void
MTS_BackgroundHelper::addBgBias(cairo_t *cr, int width, int height, int color){
  //cout << "start adding bias" << endl;
  cairo_pattern_t *pattern_vertical = cairo_pattern_create_linear(helper->rng()%width, 0,
                                                                  helper->rng()%width, height);
  cairo_pattern_t *pattern_horizontal = cairo_pattern_create_linear(0, helper->rng()%height,
                                                                    width, helper->rng()%height);

  // set the number of points
  int points_min = getParam(std::string("bias_vert_num_min"));
  int points_max = getParam(std::string("bias_vert_num_max"))+1 - points_min;
  int num_points_vertical = helper->rng()%points_max + points_min;
  int num_points_horizontal = helper->rng()%points_max + points_min;

  if (width > height) {
    // num points in horizontal direction is multiplied by ratio width/height
    num_points_horizontal = helper->rng()%((width/height)*points_max) + points_min; 
  }
  double offset_vertical = 1.0 / (num_points_vertical - 1);
  double offset_horizontal = 1.0 / (num_points_horizontal - 1);
                
  // get and set bias std variables
  double std_scale = getParam(std::string("bias_std_scale"));
  double std_shift = getParam(std::string("bias_std_shift"));
  double mean = getParam(std::string("bias_mean"));

  double bias_std = round((pow(1/(bias_var_gen() + 0.1), 0.5) 
                           * std_scale + std_shift) * 100) / 100;
  // set a normal distribution for the bias
  normal_distribution<> bias_dist(mean , bias_std);
  variate_generator<mt19937, normal_distribution<> > bias_gen(helper->rng2_, bias_dist);
  bias_gen.engine().seed(helper->rng());
  bias_gen.distribution().reset();

  int color_stop_val;
  double dcolor;
  // add color stops for each point along the vertical line
  for (int i = 0; i < num_points_vertical; i++){
    color_stop_val = color + (int)round(bias_gen());
    // bound the number between 0 and 255
    color_stop_val = std::min(color_stop_val, 255);
    color_stop_val = std::max(color_stop_val, 0);
    dcolor = color_stop_val / 255.0;

    cairo_pattern_add_color_stop_rgb(pattern_vertical, i*offset_vertical, dcolor,dcolor,dcolor);
  }
  // add color stops for each point along the horizontal line
  for (int i = 0; i < num_points_horizontal; i++){
    color_stop_val = color + (int)round(bias_gen());
    // bound the number between 0 and 255
    color_stop_val = std::min(color_stop_val, 255);
    color_stop_val = std::max(color_stop_val, 0);
    dcolor = color_stop_val / 255.0;

    cairo_pattern_add_color_stop_rgb(pattern_horizontal, i*offset_horizontal, dcolor,dcolor,dcolor);
  }

  cairo_set_source(cr, pattern_horizontal);
  cairo_paint_with_alpha(cr,0.3);
  cairo_set_source(cr, pattern_vertical);
  cairo_paint_with_alpha(cr,0.3);
  //cout << "finished bias" << endl;

}


void
MTS_BackgroundHelper::addBgPattern (cairo_t *cr, int width, int height, 
                                    bool even, bool grid, bool curved) {

  //randomly choose number of lines 
  int lines_min, lines_max;
  if (grid) { // correctly get number of lines to draw from user config
    lines_min = getParam(std::string("grid_num_min"));
    lines_max = getParam(std::string("grid_num_max"))+1 - lines_min;
  } else if (even) {
    lines_min = getParam(std::string("para_num_min"));
    lines_max = getParam(std::string("para_num_max"))+1 - lines_min;
  } else {
    lines_min = getParam(std::string("vpara_num_min"));
    lines_max = getParam(std::string("vpara_num_max"))+1 - lines_min;
  }
  int num = helper->rng()%lines_max + lines_min;

  //length of lines
  int length = std::max(width, height)*1.414;

  //average spacing
  int spacing = length / num;

  //randomly choose then apply rotation degree
  int deg = helper->rng()%360;
  double rad = (deg/180.0)*3.14;
  // translate to center of image to rotate there instead of from origin
  cairo_translate(cr, width/2.0, height/2.0);
  cairo_rotate(cr, rad);
  cairo_translate(cr, -width/2.0, -height/2.0); // translate back

  //initialize the vector of lines stored as xy coordinates
  std::vector<std::vector<coords> > lines;

  int top_y = -(length - height) / 2;
  int bottom_y = height + (length - height) / 2;

  std::vector<int> x_coords;
  std::vector<int> y_coords;
  int rand_num;
  // create points to be used in curving lines
  if(curved) {
    //get curving params
    rand_num = helper->rng()%3+1;

    int x_off, y_off;
    // get coordinates of curved lines
    for (int k = 0; k < rand_num; k++) {
      x_off = helper->rng() % (width/2) - (width/4);
      x_coords.push_back(x_off);
      y_off = helper->rng()%length;

      if (k == 0) {
        y_coords.push_back(y_off);

      } else if (y_off < y_coords[0]){
        y_coords.insert(y_coords.begin(),y_off);

      } else if (k == 1 && y_off > y_coords[0]){
        y_coords.push_back(y_off);

      } else if (k == 2 && y_off > y_coords[1]){
        y_coords.push_back(y_off);

      } else if (k == 2 && y_off < y_coords[1]){
        y_coords.insert(y_coords.begin()+1,y_off);
      }
    }
  }

  //get a random initial spacing
  int init_spacing = helper->rng()%spacing;
  if(even) init_spacing=spacing;
  int cur_x = -(length-width)/2+init_spacing;
  int increase = (spacing-init_spacing)*2/num;
                
  std::vector<coords> points;

  for (int i = 0; i < num; i++) {
    points.push_back(std::make_pair(cur_x,top_y));
    if (curved) {
      for (int k = 0; k < rand_num; k++) {
        points.push_back(std::make_pair(cur_x + x_coords[k], top_y + y_coords[k]));
      }
    }
    points.push_back(std::make_pair(cur_x,bottom_y));
    lines.push_back(points);
    cur_x += cur_x + increase;
  }

  double c_min, c_max, d_min, d_max;

  if (even) {
    c_min = getParam(std::string("para_curve_c_min"));
    c_max = getParam(std::string("para_curve_c_max"));
    d_min = getParam(std::string("para_curve_d_min"));
    d_max = getParam(std::string("para_curve_d_max"));
  } else {
    c_min = getParam(std::string("vpara_curve_c_min"));
    c_max = getParam(std::string("vpara_curve_c_max"));
    d_min = getParam(std::string("vpara_curve_d_min"));
    d_max = getParam(std::string("vpara_curve_d_max"));
  }

  // get line i and make the points into a cairo_path
  for (int i = 0; i < num; i++){
    points = lines[i];
    helper->points_to_path(cr, points, c_min, c_max, d_min, d_max); //draw path shape
    cairo_stroke(cr);
  }

  // draw grid
  if (grid) {
    cairo_translate(cr, width/2.0, height/2.0);
    cairo_rotate(cr, 3.14/2);
    cairo_translate(cr, -width/2.0, -height/2.0);

    // get line i and make the points into a cairo_path
    for (int i = 0; i < num; i++){
      points = lines[i];
      helper->points_to_path(cr, points, c_min, c_max, d_min, d_max); //draw path shape
      cairo_stroke(cr);
    }
  }

  cairo_rotate(cr, 0);
}


void
MTS_BackgroundHelper::colorDiff (cairo_t *cr, int width, int height, 
                                 double color_min, double color_max) {

  int num_colors_min = getParam(std::string("diff_num_colors_min"));
  int num_colors_max = getParam(std::string("diff_num_colors_max"));

  int num = (helper->rng() % (num_colors_max - num_colors_min + 1)) + num_colors_min; 

  for (int i = 0; i < num; i++) {
    double color = helper->rng()%((int)((color_max-color_min)*100)+1)/100.0+color_min;
    cairo_set_source_rgb(cr,color,color,color);

    bool horizontal = (bool)helper->rng()%2;

    if (horizontal) {
      // whether the left side is filled or the right side
      bool left = (bool)helper->rng()%2;

      int x_top=helper->rng()%width;
      if (left) {
        cairo_move_to (cr, 0, 0);
      } else {
        cairo_move_to (cr, width, 0);
      }
      cairo_line_to (cr, x_top, 0);

      int x_bottom=helper->rng()%width;
      cairo_line_to (cr, x_bottom, height);
      if (left) {
        cairo_line_to (cr, 0, height);
        cairo_line_to (cr, 0, 0);
      } else {
        cairo_line_to (cr, width, height);
        cairo_line_to (cr, width, 0);
      }
    } else {
      // whether the top side is filled or the bottom side
      bool top = (bool)helper->rng()%2;

      int x_left=helper->rng()%height;
      if (top) {
        cairo_move_to (cr, 0, 0);
      } else {
        cairo_move_to (cr, 0, height);
      }
      cairo_line_to (cr, x_left, 0);

      int x_right=helper->rng()%height;
      cairo_line_to (cr, x_right, height);
      if (top) {
        cairo_line_to (cr, width, 0);
        cairo_line_to (cr, 0, 0);
      } else {
        cairo_line_to (cr, width, height);
        cairo_line_to (cr, 0, height);
      }
    }
    cairo_fill(cr);
  }
}


void
MTS_BackgroundHelper::cityPoint(cairo_t *cr, int width, int height) {

  //options for side of the surface the point origin appears on
  enum Side { left, right, top, bottom }; // (top from user perspective)

  int option = helper->rng() % 4; // choose a Side for circle origin
  int x,y; // circle origin coordinates

  // set point radius
  int r_min = getParam(std::string("point_radius_min"));
  int r_max = getParam(std::string("point_radius_max"))+1 - r_min;
  if(r_max+r_min > ((height/2)+1)) r_max = (height/2) - 5; //verify perconditions

  int radius = (helper->rng() % r_max) + r_min; 

  // set circle origin coords based on random choice of side
  switch(option) {
  case left:   // 0
    x = -(helper->rng() % radius);
    y = helper->rng() % height; 
    break;
  case right:  // 1
    x = (helper->rng() % radius) + width;
    y = helper->rng() % height;
    break;
  case top:    // 2
    x = helper->rng() % width;
    y = -(helper->rng() % radius);
    break; 
  case bottom: // 3
    x = helper->rng() % width;
    y = (helper->rng() % radius) + height;
    break;
  }

  //draw and fill the circle arc
  cairo_arc(cr, x, y, radius, 0, 2*M_PI);
  cairo_fill(cr);
}


void
MTS_BackgroundHelper::generateBgFeatures(std::vector<BGFeature> &bg_features){

  // get probabilities of all features
  int maxnum=getParam("max_num_features");
  double probs[12]={
    getParam("diff_prob"),
    getParam("distract_prob"),
    getParam("boundary_prob"),
    getParam("blob_prob"),
    getParam("straight_prob"),
    getParam("grid_prob"),
    getParam("point_prob"),
    getParam("para_prob"),
    getParam("vpara_prob"),
    getParam("texture_prob"),
    getParam("railroad_prob"),
    getParam("river_prob"),
  };

  std::vector<BGFeature> all_features={Colordiff, Distracttext, Boundry, Colorblob, Straight, Grid, Citypoint, Parallel, Vparallel, Texture, Railroad, Riverline};
  int j, cur_index, count = 0;
  bool flag;
  BGFeature cur;

  // iterate through all bg features, applying based on probability
  // until maxnum of features is reached, or there are no features left
  while (count < maxnum && all_features.size() > 0){
    flag = true;

    while (flag && all_features.size() > 0) { 
      j = helper->rng()%all_features.size();
      cur = all_features[j];
      all_features.erase(all_features.begin()+j);
      if (cur==Vparallel && find(bg_features.begin(), bg_features.end(), Parallel)
          != bg_features.end()) continue;

      if (cur==Parallel && find(bg_features.begin(), bg_features.end(), Vparallel)
          != bg_features.end()) continue;

      flag=false;

      cur_index=static_cast<int>(cur);
      // if probability of bg feature at cur_index succedes, add it to 
      // the features to be applied vector 
      if(helper->rndProbUnder(probs[cur_index])){
        bg_features.push_back(cur);
        count++;
      }
    }
  }
  //cout << "got all features" << endl;
}


void 
MTS_BackgroundHelper::generateBgSample(cairo_surface_t *&bg_surface, std::vector<BGFeature> &features, int height, int width, int bg_color, int contrast){

  double c_min, c_max, d_min, d_max, curve_prob;
  int num_lines;
  //cout << "generating bg sample" << endl;
  // initialize the cairo image variables for background
  cairo_surface_t *surface;
  cairo_t *cr;
  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
  cr = cairo_create (surface);

  // paint initial background brightness
  cairo_set_source_rgb(cr, bg_color/255.0,bg_color/255.0,bg_color/255.0);
  cairo_paint (cr);

  if (find(features.begin(), features.end(), Colordiff)!= features.end()) {
    double color_dis = getParam("diff_color_distance");
    double color_min = (bg_color-contrast+color_dis)/255.0;
    double color_max = bg_color/255.0;
    if (color_min > color_max) color_min=color_max;
    colorDiff(cr, width, height, color_min, color_max);
  }
  if (find(features.begin(), features.end(), Colorblob)!= features.end()) {
    int num_min= (int)getParam("blob_num_min");
    int num_max= (int)getParam("blob_num_max");
    double size_min = getParam("blob_size_min");
    double size_max = getParam("blob_size_max");
    double dim_rate = getParam("blob_diminish_rate");
    helper->addSpots(surface,num_min,num_max,size_min,size_max,dim_rate,
                     false,bg_color-helper->rng()%contrast);
  }

  //add background bias field
  addBgBias(cr, width, height, bg_color);

  // set background source brightness
  double color = (bg_color - helper->rng() % contrast) / 255.0;
  cairo_set_source_rgb(cr,color,color,color);

  // GENERATE BACKGROUND FEATURES:
  // add texture swaths by probability
  if (find(features.begin(), features.end(), Texture)!= features.end()) {
    c_min = getParam("texture_curve_c_min");
    c_max = getParam("texture_curve_c_max");
    d_min = getParam("texture_curve_d_min");
    d_max = getParam("texture_curve_d_max");
    num_lines = helper->rng()%2 + 1; // ensure there arent too many swaths

    // add num_lines lines iteratively
    for (int i = 0; i < num_lines; i++) {
      addTexture(cr, (bool)helper->rng()%2, (bool)helper->rng()%2, color, 
                 width, height, c_min, c_max, d_min, d_max); 
    }
  }

  // add evenly spaced parallel lines by probability
  if (find(features.begin(), features.end(), Parallel)!= features.end()) {
    curve_prob = getParam(std::string("para_curve_prob"));
    addBgPattern(cr, width, height, true, false, helper->rndProbUnder(curve_prob));
  }

  // add varied parallel lines by probability
  if (find(features.begin(), features.end(), Vparallel)!= features.end()) {
    curve_prob = getParam(std::string("vpara_curve_prob"));
    addBgPattern(cr, width, height, false, false, helper->rndProbUnder(curve_prob));
  }

  // add grid lines by probability
  if (find(features.begin(), features.end(), Grid)!= features.end()) {
    curve_prob = getParam(std::string("grid_curve_prob"));
    addBgPattern(cr, width, height, true, true, helper->rndProbUnder(curve_prob));
  }

  // add railroads by probability
  if (find(features.begin(), features.end(), Railroad)!= features.end()) {
    int railroad_min = getParam(std::string("railroad_num_lines_min"));
    int railroad_max = getParam(std::string("railroad_num_lines_max"))+1 - railroad_min;
    c_min = getParam("railroad_curve_c_min");
    c_max = getParam("railroad_curve_c_max");
    d_min = getParam("railroad_curve_d_min");
    d_max = getParam("railroad_curve_d_max");
    num_lines = helper->rng()%railroad_max + railroad_min;

    // add num_lines lines iteratively
    for (int i = 0; i < num_lines; i++) {
      addLines(cr, false, true, false, true, false, (bool)helper->rng()%2, width, height, color, c_min, c_max, d_min, d_max);
    }
  }

  // add boundary lines by probability
  if (find(features.begin(), features.end(), Boundry)!= features.end()) {
    int boundary_min = getParam(std::string("boundary_num_lines_min"));
    int boundary_max = getParam(std::string("boundary_num_lines_max"))+1 - boundary_min;

    num_lines = helper->rng()%boundary_max + boundary_min;
    double dash_probability = getParam(std::string("boundary_dashed_prob"));
    c_min = getParam("boundary_curve_c_min");
    c_max = getParam("boundary_curve_c_max");
    d_min = getParam("boundary_curve_d_min");
    d_max = getParam("boundary_curve_d_max");

    // add num_lines lines iteratively
    for (int i = 0; i < num_lines; i++) {
      addLines(cr, true, false, helper->rndProbUnder(dash_probability), true, false,
               (bool)helper->rng()%2, width, height, color, c_min, c_max, d_min, d_max);
    }
  }

  // add straight lines by probability
  if (find(features.begin(), features.end(), Straight)!= features.end()) {
    int straight_min = getParam(std::string("straight_num_lines_min"));
    int straight_max = getParam(std::string("straight_num_lines_max"))+1 - straight_min;
    num_lines = helper->rng() % straight_max + straight_min;

    // add num_lines lines iteratively
    for (int i = 0; i < num_lines; i++) {
      addLines(cr, false, false, false, false, false, (bool)helper->rng()%2, 
               width, height, color, 0,0,0,0);
    }
  }

  // add rivers by probability
  if (find(features.begin(), features.end(), Riverline)!= features.end()) {
    int river_min = getParam(std::string("river_num_lines_min"));
    int river_max = getParam(std::string("river_num_lines_max"))+1 - river_min;

    num_lines = helper->rng()%river_max + river_min;
    c_min = getParam("river_curve_c_min");
    c_max = getParam("river_curve_c_max");
    d_min = getParam("river_curve_d_min");
    d_max = getParam("river_curve_d_max");

    // add num_lines lines iteratively
    for (int i = 0; i < num_lines; i++) {
      addLines(cr, false, false, false, true, (bool)helper->rng()%2, 
               (bool)helper->rng()%2, width, height, color, c_min, c_max, d_min, d_max);
    }
  }

  // add city point by probability
  if (find(features.begin(), features.end(), Citypoint)!= features.end()) {
    cityPoint(cr, width, height);
  }

  bg_surface = surface;
}
