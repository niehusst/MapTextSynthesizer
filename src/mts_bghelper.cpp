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

#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <string>

#include <pango/pangocairo.h>

#include "mts_bghelper.hpp"

using boost::random::beta_distribution;
using boost::random::normal_distribution;
using boost::random::gamma_distribution;
using boost::random::variate_generator;


// SEE mts_bghelper.hpp FOR ALL DOCUMENTATION


MTS_BackgroundHelper::MTS_BackgroundHelper(shared_ptr<MTS_BaseHelper> h,
                                           shared_ptr<MTSConfig> c)
    :helper(&(*h)),  // initialize fields
    config(&(*c)),  
    bias_var_dist(c->getParamDouble("bias_std_alpha"),
                  c->getParamDouble("bias_std_beta")),
    bias_var_gen(h->rng2_, bias_var_dist),
    texture_distribution(c->getParamDouble("texture_width_alpha"), 
                         c->getParamDouble("texture_width_beta")),
    texture_distrib_gen(h->rng2_, texture_distribution)
{}


MTS_BackgroundHelper::~MTS_BackgroundHelper(){}


void
MTS_BackgroundHelper::draw_boundary(cairo_t *cr, double linewidth, 
                                    double og_col, bool horizontal) {

    // get original dash code
    int dash_len = cairo_get_dash_count(cr);
    double dash[dash_len], offset[dash_len];
    cairo_get_dash(cr, dash, offset);

    // calculate a distance between lines
    double dis_min = config->getParamDouble("boundary_distance_min");
    double dis_max = config->getParamDouble("boundary_distance_max");
    double x_dis = helper->rndBetween(dis_min,dis_max) * linewidth;
    double y_dis = helper->rndBetween(dis_min,dis_max) * linewidth;

    // set boundary line characteristics
    double width_min = config->getParamDouble("boundary_linewidth_min");
    double width_max = config->getParamDouble("boundary_linewidth_max");
    double new_linewidth = linewidth * helper->rndBetween(width_min,width_max);
    cairo_set_line_width(cr, new_linewidth);
    cairo_set_dash(cr, dash, 0,0); //set dash pattern to none

    // set boundary line gray-scale color (lighter than original)
    double color_min = config->getParamDouble("boundary_color_diff_min");
    double color_max = config->getParamDouble("boundary_color_diff_max");
    double color_diff = helper->rndBetween(color_min,color_max);
    double color = og_col + color_diff;
    cairo_set_source_rgb(cr, color, color, color);

    // stroke the boundary line
    cairo_stroke_preserve(cr);

    // translate distance so that main line is drawn off center of boundary
    cairo_path_t *path_tmp = cairo_copy_path(cr);
    cairo_new_path(cr);
    cairo_translate(cr, x_dis, y_dis);
    cairo_append_path(cr, path_tmp);
    cairo_path_destroy(path_tmp);

    // reset to color and line width of original line
    cairo_set_line_width(cr, linewidth);
    cairo_set_source_rgb(cr, og_col, og_col, og_col);
    cairo_set_dash(cr, dash, dash_len, 0);
}


void
MTS_BackgroundHelper::draw_hatched(cairo_t *cr, double linewidth) {

    // get original dash code
    int dash_len = cairo_get_dash_count(cr);
    double dash[dash_len], offset[dash_len];
    cairo_get_dash(cr, dash, offset);

    //set width of hatches (in multiples of original linewidth)
    double width_min = config->getParamDouble("railroad_cross_width_min");
    double width_max = config->getParamDouble("railroad_cross_width_max");
    double wide = helper->rndBetween(width_min,width_max) * linewidth;
    cairo_set_line_width(cr, wide);

    //set width of each hatch (in multiples of original linewidth)
    double hatch_width_min = config->getParamDouble("railroad_hatch_width_min");
    double hatch_width_max = config->getParamDouble("railroad_hatch_width_max");
    double hatch_wide = helper->rndBetween(hatch_width_min,hatch_width_max) *
                        linewidth; 

    // set distance between hatches (in multiples of original linewidth)
    double dis_min = config->getParamDouble("railroad_distance_between_crosses_min");
    double dis_max = config->getParamDouble("railroad_distance_between_crosses_max");
    double hatch_dis = helper->rndBetween(dis_min,dis_max) * linewidth;

    //set dash pattern to be used
    const double pattern[] = {hatch_wide, hatch_dis};
    cairo_set_dash(cr, pattern, 2, 0);
    cairo_stroke_preserve(cr);

    // return to original settings
    cairo_set_line_width(cr, linewidth);
    cairo_set_dash(cr, dash, dash_len, 0);
}

void
MTS_BackgroundHelper::set_dash_pattern(cairo_t *cr) {

    int pat_len_min = config->getParamInt("dash_pattern_len_min");
    int pat_len_max = config->getParamInt("dash_pattern_len_max");
    int pattern_len = helper->rndBetween(pat_len_min,pat_len_max); 
    double dash_pattern[pattern_len];
    double dash;

    double len_min = config->getParamDouble("dash_len_min");
    double len_max = config->getParamDouble("dash_len_max");
    double len;

    //make and set pattern
    for(int i = 0; i < pattern_len; i++) {
        len = helper->rndBetween(len_min,len_max);
        dash_pattern[i] = len;
    }

    //make_dash_pattern(dash_pattern, pattern_len);
    cairo_set_dash(cr, dash_pattern, pattern_len, 0);
}

void
MTS_BackgroundHelper::orient_path(cairo_t *cr, bool curved, 
                                  int length, int width, int height) {
    int translation_x, translation_y;
    double angle = (helper->rng() % 360)/180.0*M_PI; //get angle 0 - 2PI
    translation_x = helper->rndBetween(0,width); 
    translation_y = helper->rndBetween(0,height); 

    cairo_translate(cr, translation_x, translation_y);
    cairo_rotate(cr, angle); 
    cairo_translate(cr, -length/2.0, -height);
}

void
MTS_BackgroundHelper::generate_curve(cairo_t *cr, int width, int height,
                                     double c_min, double c_max, double d_min,
                                     double d_max, bool river) {

    std::vector<coords> points;
    int num_min = config->getParamInt("bg_curve_num_points_min");
    int num_max = config->getParamInt("bg_curve_num_points_max");

    // scale the number of points if it's a river
    if (river) {
        double scale = config->getParamDouble("river_curve_num_points_scale");
        num_min *= scale;
        num_max *= scale;
    }
    int num_points = helper->rndBetween(num_min,num_max); 

    double y_var_min = config->getParamDouble("bg_curve_y_variance_min");
    double y_var_max = config->getParamDouble("bg_curve_y_variance_max");

    // scale the y variance if it's a river 
    if (river) {
        double scale = config->getParamDouble("river_curve_y_var_scale");
        y_var_min *= scale;
        y_var_max *= scale;
    }
    
    //get correct point vector based on line orientation
    points = helper->make_points_wave(width, height, num_points,
                                      y_var_min, y_var_max);
    helper->points_to_path(cr, points, c_min, c_max, d_min, d_max);
}


void
MTS_BackgroundHelper::addLines(cairo_t *cr, bool boundary, bool hatched, 
        bool dashed,bool curved,bool doubleline,bool river,int width,int height,
        double c_min, double c_max, double d_min, double d_max, double color){

    double magic_line_ratio, line_width;

    // set ratio to keep line scaled for image size
    double ratio_min = config->getParamDouble("line_width_scale_min");
    double ratio_max = config->getParamDouble("line_width_scale_max");
    magic_line_ratio = helper->rndBetween(ratio_min,ratio_max); 
    line_width = std::min(width, height) * magic_line_ratio;
    cairo_set_line_width(cr, line_width);

    int length = (int)(pow(pow(width,2)+pow(height,2),0.5));

    //orient the path for the line correctly
    orient_path(cr, curved, length, width, height);

    // set path shape 
    if(curved) { 
        // draw a wiggly line
        generate_curve(cr, length, height, c_min, c_max, d_min, d_max, river);
    } else { // draw a straight line
        cairo_move_to(cr, 0, 0);
        cairo_line_to(cr, length, 0); 
    } 

    // set line style to dashed or not (default solid)
    if(dashed) { set_dash_pattern(cr); } 

    // set boundary or not
    if(boundary) { draw_boundary(cr, line_width, color); }

    // set hatching or not
    if(hatched) { draw_hatched(cr, line_width); } 

    // draw parallel or not
    if(doubleline) { 
        cairo_stroke_preserve(cr);
        //draw_parallel(cr, horizontal, 3*line_width); 
        double dis_min = config->getParamDouble("double_distance_min");
        double dis_max = config->getParamDouble("double_distance_max");
        double x_dis = helper->rndBetween(dis_min,dis_max) * line_width;
        double y_dis = helper->rndBetween(dis_min,dis_max) * line_width;
        cairo_path_t *path_tmp = cairo_copy_path(cr);
        cairo_new_path(cr);
        cairo_translate(cr, x_dis, y_dis);
        cairo_append_path(cr, path_tmp);
    }

    //stroke
    cairo_stroke(cr);

    //set rotations and translations back to normal
    cairo_identity_matrix(cr); 
    cairo_set_dash(cr, NULL, 0, 0);
}

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
MTS_BackgroundHelper::diagonal_lines(cairo_t *cr,int x1, int y1, int x2, int y2,
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
MTS_BackgroundHelper::crossed_lines(cairo_t *cr, int x1, int y1, int x2,
                                    int y2, int spacing, int width, int height){
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
MTS_BackgroundHelper::shape_helper(cairo_t *cr, double x, double y,
                                   double angle, double edge_len,
                                   int num_sides, int counter) {
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
MTS_BackgroundHelper::draw_shape(cairo_t *cr, int x, int y,
                                 int num_sides, int radius) {

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
MTS_BackgroundHelper::shape_texture_helper(cairo_t *cr, int origin_x,
               int origin_y, int num_sides, int distance, bool even, int radius,
               int width, int height) {

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
  shape_texture_helper(cr, origin_x, origin_y, num_sides, distance, even,
                       radius, width, height);
}



void
MTS_BackgroundHelper::make_shape_texture(cairo_t *cr,int x, int y, int diameter,
                            int num_sides, int spacing, int width, int height) {
  // call recursive circle helper to draw texture
  shape_texture_helper(cr, x, y, num_sides, spacing, true, diameter/2,
                       width, height);
}


void
MTS_BackgroundHelper::draw_texture(cairo_t *cr, int texture, double brightness,
                                   double linewidth, int diameter,int num_sides,
                                   int spacing, int width, int height) {
  //verify preconditions
  if (spacing < 1) spacing = 1;
  if (texture != 2) diameter = 0, num_sides = 0;

  //set drawing source color and line width of texture
  cairo_set_source_rgb(cr, brightness,brightness,brightness);        
  cairo_set_line_width(cr, linewidth);                 

  // a vector of texture drawing function pointers
  void (*texture_func)(cairo_t*, int, int, int, int, int, int, int);
  std::vector<void (*)(cairo_t*, int, int, int, int, int, int, int)>texture_vec;

  // populate vector with all texture drawing functions
  texture_vec.push_back(&diagonal_lines);
  texture_vec.push_back(&crossed_lines);
  texture_vec.push_back(&make_shape_texture);

  // draw texture 
  (*texture_vec[texture])(cr, 0,0, diameter, num_sides, spacing, width, height);

  // apply texture to surface 
  cairo_stroke(cr);
}


void
MTS_BackgroundHelper::set_texture_source(cairo_t *cr, int texture, double brightness,
                                         double linewidth, int spacing, int width,
                                         int height) {
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
MTS_BackgroundHelper::addTexture(cairo_t *cr, bool curved, double brightness,
                                 int width, int height, double c_min,
                                 double c_max, double d_min, double d_max) {

    cairo_save(cr);

    // set adequate spacing between lines in texture
    int spacing = std::max(4, width/100);
    spacing = spacing + helper->rng() % (2*spacing);  

    // linewidth range (1/3)height - (1/2)height
    int linewidth = (1.0 / (0.5 + texture_distrib_gen() * 2.5)) * height;
    int texture = helper->rng() % 3; // range 0-2
    //coords start_point;

    // set source to correct texture and make line thick & rounded
    set_texture_source(cr, texture, brightness, 1, spacing, width, height); 
    cairo_set_line_width(cr, linewidth);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

    int length = (int)(pow(pow(width,2)+pow(height,2),0.5));
    //orient the path for the line correctly
    orient_path(cr, curved, length, width, height);

    // set path shape 
    if(curved) { 
        // draw a wiggly line
        generate_curve(cr, length, height, c_min,c_max,d_min,d_max);

    } else { // draw a straight line
        cairo_move_to(cr,0,0);
        cairo_line_to(cr, length, 0);
    } 

    // stroke lines to surface
    cairo_stroke(cr);

    // reset to original transformations
    cairo_identity_matrix(cr);
    cairo_restore(cr);
}


void
MTS_BackgroundHelper::addBgBias(cairo_t *cr, int width, int height, int color){

    cairo_pattern_t *pattern_vertical = cairo_pattern_create_linear(
                           helper->rng()%width, 0, helper->rng()%width, height);
    cairo_pattern_t *pattern_horizontal = cairo_pattern_create_linear(0,
                           helper->rng()%height, width, helper->rng()%height);

    // set the number of points
    int points_min = config->getParamInt("bias_vert_num_min");
    int points_max = config->getParamInt("bias_vert_num_max");
    int num_points_vertical = helper->rndBetween(points_min,points_max); 
    int num_points_horizontal = helper->rndBetween(points_min,points_max); 

    if (width > height) {
        // num points in horiz direction is multiplied by ratio width/height
        num_points_horizontal = helper->rndBetween((width/height)*points_min,
                                                   (width/height)*points_max); 
    }
    
    double offset_vertical = 1.0 / (num_points_vertical - 1);
    double offset_horizontal = 1.0 / (num_points_horizontal - 1);

    // get and set bias std variables
    double std_scale = config->getParamDouble("bias_std_scale");
    double std_shift = config->getParamDouble("bias_std_shift");
    double mean = config->getParamDouble("bias_mean");

    double bias_std = round((pow(1/(bias_var_gen() + 0.1), 0.5) 
                * std_scale + std_shift) * 100) / 100;
    
    // set a normal distribution for the bias
    normal_distribution<> bias_dist(mean , bias_std);
    variate_generator<mt19937, normal_distribution<> > bias_gen(helper->rng2_,
                                                                bias_dist);
    bias_gen.engine().seed(helper->rng());
    bias_gen.distribution().reset();

    int color_stop_val;
    double dcolor;
    // add color stops for each point along the vertical line
    for (int i = 0; i < num_points_vertical; i++){
        color_stop_val = color + (int)round(bias_gen());
        // bound the number between 0 and 255
        color_stop_val = min(color_stop_val, 255);
        color_stop_val = max(color_stop_val, 0);
        dcolor = color_stop_val / 255.0;

        cairo_pattern_add_color_stop_rgb(pattern_vertical, i*offset_vertical,
                                         dcolor,dcolor,dcolor);
    }
    // add color stops for each point along the horizontal line
    for (int i = 0; i < num_points_horizontal; i++){
        color_stop_val = color + (int)round(bias_gen());
        // bound the number between 0 and 255
        color_stop_val = min(color_stop_val, 255);
        color_stop_val = max(color_stop_val, 0);
        dcolor = color_stop_val / 255.0;

        cairo_pattern_add_color_stop_rgb(pattern_horizontal,i*offset_horizontal,
                                         dcolor,dcolor,dcolor);
    }

    cairo_set_source(cr, pattern_horizontal);
    cairo_paint_with_alpha(cr,0.3);
    cairo_set_source(cr, pattern_vertical);
    cairo_paint_with_alpha(cr,0.3);
}


void
MTS_BackgroundHelper::addBgPattern (cairo_t *cr, int width, int height, 
                                    bool even, bool grid, bool curved) {

    // get and set base line width from user config params
    double line_width, magic_line_ratio;
    double ratio_min = config->getParamDouble("line_width_scale_min");
    double ratio_max = config->getParamDouble("line_width_scale_max");
    magic_line_ratio = helper->rndBetween(ratio_min,ratio_max);
    line_width = std::min(width, height) * magic_line_ratio;
    cairo_set_line_width(cr, line_width);

    //randomly choose number of lines 
    int lines_min, lines_max;
    if (grid) { // correctly get number of lines to draw from user config
        lines_min = config->getParamInt("grid_num_min");
        lines_max = config->getParamInt("grid_num_max");
    } else if (even) {
        lines_min = config->getParamInt("para_num_min");
        lines_max = config->getParamInt("para_num_max");
    } else {
        lines_min = config->getParamInt("vpara_num_min");
        lines_max = config->getParamInt("vpara_num_max");
    }
    int num = helper->rndBetween(lines_min,lines_max); 

    //length of lines
    double length = std::max(width, height)*pow(2,0.5);

    //average spacing
    double spacing = length / num;

    //randomly choose then apply rotation degree
    int deg = helper->rng()%360;
    double rad = (deg/180.0)*M_PI;
    // translate to center of image to rotate there instead of from origin
    cairo_translate(cr, width/2.0, height/2.0);
    cairo_rotate(cr, rad);
    cairo_translate(cr, -width/2.0, -height/2.0); // translate back

    //initialize the vector of lines stored as xy coordinates
    std::vector<std::vector<coords> > lines;

    double left_x = -(length - width) / 2;
    double right_x = width + (length - width) / 2;

    //get a random initial spacing
    double init_spacing = (helper->rng()%(int)(spacing*100))/100.0;
    if(even) init_spacing=spacing;
    double cur_y = -(length-height) / 2.0 + init_spacing;
    double increase = (spacing-init_spacing) * 2 / num;

    // to store the real points of every line
    std::vector<coords> points;

    // to store the curve pattern
    std::vector<coords> curve_points;

    // get curved line user config params if curved line is to be drawn
    if (curved) {
        double y_var_min = config->getParamDouble("bg_curve_y_variance_min");
        double y_var_max = config->getParamDouble("bg_curve_y_variance_max");
        int num_min = config->getParamInt("bg_curve_num_points_min");
        int num_max = config->getParamInt("bg_curve_num_points_max");
        int num_points = helper->rndBetween(num_min,num_max); 
        curve_points = helper->make_points_wave(length,length,num_points,
                                                y_var_min,y_var_max);
    }
    
    // get the points for each line (there are 'num' number of lines)
    for (int i = 0; i < num; i++) {
        // what points are used depends on if line is curved or not
        if (curved) {
            // add points that are in curved shape
            for (int k = 0; k < curve_points.size(); k++) {
                points.push_back(std::make_pair(left_x + curve_points[k].first,
                                      curve_points[k].second - (length-cur_y)));
            }
        } else {
            // add 2 points; start and end of line
            points.push_back(std::make_pair(left_x,cur_y));
            points.push_back(std::make_pair(right_x,cur_y));
        }
        
        // add the line to the lines vector
        lines.push_back(points);
        points.clear();
        cur_y += init_spacing + increase;
        init_spacing += increase;
    }

    double c_min, c_max, d_min, d_max;

    if (curved) {
        if (even) {
            c_min = config->getParamDouble("para_curve_c_min");
            c_max = config->getParamDouble("para_curve_c_max");
            d_min = config->getParamDouble("para_curve_d_min");
            d_max = config->getParamDouble("para_curve_d_max");
        } else {
            c_min = config->getParamDouble("vpara_curve_c_min");
            c_max = config->getParamDouble("vpara_curve_c_max");
            d_min = config->getParamDouble("vpara_curve_d_min");
            d_max = config->getParamDouble("vpara_curve_d_max");
        }
    }

    // get line i and make the points into a cairo_path
    for (int i = 0; i < num; i++){
        points = lines[i];
        if (curved) {
            // draw curved path 
            helper->points_to_path(cr, points, c_min, c_max, d_min, d_max);
        } else {
            // draw line path
            cairo_move_to(cr,points[0].first,points[0].second);
            cairo_line_to(cr,points[points.size()-1].first,
                          points[points.size()-1].second);
        }

        // stroke the path
        cairo_stroke(cr);
    }

    // draw grid
    if (grid) {
        cairo_translate(cr, width/2.0, height/2.0);
        cairo_rotate(cr, M_PI/2);
        cairo_translate(cr, -width/2.0, -height/2.0);

        // get line i and make the points into a cairo_path
        for (int i = 0; i < num; i++) {
            points = lines[i];
            if (curved) {
                // draw curved path shape
                helper->points_to_path(cr, points, c_min, c_max, d_min, d_max); 
            } else {
                // draw line path 
                cairo_move_to(cr,points[0].first,points[0].second);
                cairo_line_to(cr,points[points.size() - 1].first,
                              points[points.size() - 1].second);
            }

            // stroke the path
            cairo_stroke(cr);
        }
    }

    // clean up transformations
    cairo_identity_matrix(cr);
}


void
MTS_BackgroundHelper::colorDiff (cairo_t *cr, int width, int height, 
                                 double color_min, double color_max) {

    int num_colors_min = config->getParamInt("diff_num_colors_min");
    int num_colors_max = config->getParamInt("diff_num_colors_max");

    int num = helper->rndBetween(num_colors_min,num_colors_max); 

    // generate 'num' number of different color zones 
    for (int i = 0; i < num; i++) {
        double color = helper->rndBetween(color_min,color_max);
        cairo_set_source_rgb(cr,color,color,color);

        bool horizontal = helper->rng() % 2;

        if (horizontal) {
            // whether the left side is filled or the right side
            bool left = helper->rng()%2;

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
            bool top = helper->rng()%2;

            int y_left=helper->rng()%height;
            if (top) {
                cairo_move_to (cr, 0, 0);
            } else {
                cairo_move_to (cr, 0, height);
            }
            cairo_line_to (cr, 0, y_left);

            int y_right=helper->rng()%height;
            cairo_line_to (cr, width, y_right);
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
    cairo_fill(cr);
  }
}


void
MTS_BackgroundHelper::cityPoint(cairo_t *cr, int width, int height, bool hollow)
{
    //options for side of the surface the point origin appears on
    enum Side { left, right, top, bottom }; // (top from user perspective)

    int option = helper->rng() % 4; // choose a Side for circle origin
    int x, y; // circle origin coordinates

    // set point radius
    double r_min = config->getParamDouble("point_radius_min");
    double r_max = config->getParamDouble("point_radius_max");
    if (r_max > 0.5) r_max = 0.5; //verify perconditions

    int radius = (int)(helper->rndBetween(r_min,r_max) * height); 

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

    //draw the circle arc
    cairo_arc(cr, x, y, radius, 0, 2*M_PI);
    
    if (hollow) { // don't fill in the circle
        // set line width from user config params
        double line_width, magic_line_ratio;
        double ratio_min = config->getParamDouble("line_width_scale_min");
        double ratio_max = config->getParamDouble("line_width_scale_max");
        magic_line_ratio = helper->rndBetween(ratio_min,ratio_max); 
        line_width = std::min(width, height) * magic_line_ratio;

        // draw line
        cairo_set_line_width(cr, line_width);
        cairo_stroke(cr);
    } else { // fill in the circle
        cairo_fill(cr);
    }
}


void
MTS_BackgroundHelper::generateBgFeatures(std::vector<BGFeature> &bg_features){

    // get probabilities of all bg features
    int maxnum=config->getParamDouble("max_num_features");
    double probs[12]={
        config->getParamDouble("diff_prob"),
        config->getParamDouble("distract_prob"),
        config->getParamDouble("boundary_prob"),
        config->getParamDouble("blob_prob"),
        config->getParamDouble("straight_prob"),
        config->getParamDouble("grid_prob"),
        config->getParamDouble("point_prob"),
        config->getParamDouble("para_prob"),
        config->getParamDouble("vpara_prob"),
        config->getParamDouble("texture_prob"),
        config->getParamDouble("railroad_prob"),
        config->getParamDouble("river_prob"),
    };

    // init the vector of all possible features to sample from
    std::vector<BGFeature> all_features={Colordiff, Distracttext, Boundary,
            Colorblob, Straight, Grid, Citypoint, Parallel, Vparallel, Texture,
            Railroad, Riverline};
    int j, cur_index, count = 0;
    bool flag;
    BGFeature cur;

    // iterate through all bg features, applying it based on probability
    // until maxnum of features is reached, or there are no features left
    while (count < maxnum && all_features.size() > 0){
        flag = true;

        while (flag && all_features.size() > 0) { 
            j = helper->rng() % all_features.size();
            cur = all_features[j];
            all_features.erase(all_features.begin()+j);
            
            if (cur == Vparallel &&
                find(bg_features.begin(), bg_features.end(), Parallel) !=
                bg_features.end()) {
              continue;
            }
            if (cur == Parallel &&
                find(bg_features.begin(), bg_features.end(), Vparallel)
                != bg_features.end()) {
              continue;
            }
            
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
}


void 
MTS_BackgroundHelper::generateBgSample(cairo_surface_t *&bg_surface,
                 std::vector<BGFeature> &features, int height, int width,
                 int bg_color, int contrast){

    double c_min, c_max, d_min, d_max, curve_prob;
    int num_lines;
 
    // initialize the cairo image variables for background
    cairo_surface_t *surface;
    cairo_t *cr;
    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
    cr = cairo_create (surface);

    // paint initial background brightness
    cairo_set_source_rgb(cr, bg_color/255.0,bg_color/255.0,bg_color/255.0);
    cairo_paint (cr);

    if (find(features.begin(), features.end(), Colordiff)!= features.end()) {
        double color_dis = config->getParamDouble("diff_color_distance");
        double color_min = (bg_color-contrast+color_dis)/255.0;
        double color_max = bg_color/255.0;
        if (color_min > color_max) color_min=color_max;
        colorDiff(cr, width, height, color_min, color_max);
    }

    //add background bias field
    addBgBias(cr, width, height, bg_color);

    if (find(features.begin(), features.end(), Colorblob)!= features.end()) {
        int num_min= config->getParamInt("blob_num_min");
        int num_max= config->getParamInt("blob_num_max");
        double size_min = config->getParamDouble("blob_size_min");
        double size_max = config->getParamDouble("blob_size_max");
        double dim_rate = config->getParamDouble("blob_diminish_rate");
        helper->addSpots(surface,num_min,num_max,size_min,size_max,dim_rate,
                false,bg_color-contrast, bg_color);
    }

    // set background source brightness
    int color_dis_min = config->getParamInt("bg_feature_color_dis_min");
    int color_dis_max = config->getParamInt("bg_feature_color_dis_max");
    if (color_dis_max > contrast) color_dis_max = contrast;
    int text_color = bg_color - contrast;
    double color =(text_color + helper->rndBetween(color_dis_min,color_dis_max))
                  / 255.0;
    cairo_set_source_rgb(cr,color,color,color);

    // GENERATE BACKGROUND FEATURES:
    // add texture swaths by probability
    if (find(features.begin(), features.end(), Texture)!= features.end()) {
        c_min = config->getParamDouble("texture_curve_c_min");
        c_max = config->getParamDouble("texture_curve_c_max");
        d_min = config->getParamDouble("texture_curve_d_min");
        d_max = config->getParamDouble("texture_curve_d_max");

        int num_lines_min = config->getParamInt("texture_num_lines_min");
        int num_lines_max = config->getParamInt("texture_num_lines_max");
        num_lines = helper->rndBetween(num_lines_min,num_lines_max); 

        // add num_lines lines iteratively
        for (int i = 0; i < num_lines; i++) {
          addTexture(cr, (bool) helper->rng() % 2, color, width, height,
                       c_min, c_max, d_min, d_max); 
        }
    }

    // add evenly spaced parallel lines by probability
    if (find(features.begin(), features.end(), Parallel)!= features.end()) {
        curve_prob = config->getParamDouble("para_curve_prob");
        addBgPattern(cr, width, height, true, false,
                     helper->rndProbUnder(curve_prob));
    }

    // add varied parallel lines by probability
    if (find(features.begin(), features.end(), Vparallel)!= features.end()) {
        curve_prob = config->getParamDouble("vpara_curve_prob");
        addBgPattern(cr, width, height, false, false,
                     helper->rndProbUnder(curve_prob));
    }

    // add grid lines by probability
    if (find(features.begin(), features.end(), Grid)!= features.end()) {
        curve_prob = config->getParamDouble("grid_curve_prob");
        addBgPattern(cr, width, height, true, true,
                     helper->rndProbUnder(curve_prob));
    }

    // add railroads by probability
    if (find(features.begin(), features.end(), Railroad)!= features.end()) {
        int railroad_min = config->getParamInt("railroad_num_lines_min");
        int railroad_max = config->getParamInt("railroad_num_lines_max");
        c_min = config->getParamDouble("railroad_curve_c_min");
        c_max = config->getParamDouble("railroad_curve_c_max");
        d_min = config->getParamDouble("railroad_curve_d_min");
        d_max = config->getParamDouble("railroad_curve_d_max");
        num_lines = helper->rndBetween(railroad_min,railroad_max); 

        // add num_lines lines iteratively
        for (int i = 0; i < num_lines; i++) {
            addLines(cr, false, true, false, true, false, false, width, height,
                     c_min, c_max, d_min, d_max);
        }
    }

    // add boundary lines by probability
    if (find(features.begin(), features.end(), Boundary)!= features.end()) {
        int boundary_min = config->getParamInt("boundary_num_lines_min");
        int boundary_max = config->getParamInt("boundary_num_lines_max") + 1
                           - boundary_min;

        num_lines = helper->rndBetween(boundary_min,boundary_max); 
        double dash_probability= config->getParamDouble("boundary_dashed_prob");
        c_min = config->getParamDouble("boundary_curve_c_min");
        c_max = config->getParamDouble("boundary_curve_c_max");
        d_min = config->getParamDouble("boundary_curve_d_min");
        d_max = config->getParamDouble("boundary_curve_d_max");

        // add num_lines lines iteratively
        for (int i = 0; i < num_lines; i++) {
            addLines(cr, true, false, helper->rndProbUnder(dash_probability),
                     true, false, false, width, height, c_min, c_max, d_min,
                     d_max, color);
        }
    }

    // add straight lines by probability
    if (find(features.begin(), features.end(), Straight)!= features.end()) {
        int straight_min = config->getParamInt("straight_num_lines_min");
        int straight_max = config->getParamInt("straight_num_lines_max")+1
                           - straight_min;
        double dash_probability= config->getParamDouble("straight_dashed_prob");
        num_lines = helper->rndBetween(straight_min,straight_max); 

        // add num_lines lines iteratively
        for (int i = 0; i < num_lines; i++) {
            addLines(cr, false, false, helper->rndProbUnder(dash_probability),
                     false, false, false, width, height);
        }
    }

    // add rivers by probability
    if (find(features.begin(), features.end(), Riverline)!= features.end()) {
        int river_min = config->getParamInt("river_num_lines_min");
        int river_max= config->getParamInt("river_num_lines_max")+1 - river_min;

        num_lines = helper->rndBetween(river_min,river_max); 
        c_min = config->getParamDouble("river_curve_c_min");
        c_max = config->getParamDouble("river_curve_c_max");
        d_min = config->getParamDouble("river_curve_d_min");
        d_max = config->getParamDouble("river_curve_d_max");

        // add num_lines lines iteratively
        for (int i = 0; i < num_lines; i++) {
            addLines(cr, false, false, false, true, helper->rng()%2, true, 
                    width, height, c_min, c_max, d_min, d_max);
        }
    }

    // add city point by probability
    if (find(features.begin(), features.end(), Citypoint)!= features.end()) {
        double hollow = config->getParamDouble("point_hollow_prob");
        int num_min = config->getParamInt("point_num_min");
        int num_max = config->getParamInt("point_num_max");
        int point_num = helper->rndBetween(num_min,num_max); 
        for (int i = 0; i < point_num; i++) {
            cityPoint(cr, width, height, helper->rndProbUnder(hollow));
        }
    }

    // save generated cairo surface as the background surface
    bg_surface = surface;
}
