#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <string>

#include <pango/pangocairo.h>

#include "mts_bghelper.hpp"

using namespace std;
using boost::random::beta_distribution;
using boost::random::normal_distribution;
using boost::random::gamma_distribution;
using boost::random::variate_generator;


// SEE mts_bghelper.hpp FOR ALL DOCUMENTATION

double
MTS_BackgroundHelper::getParam(string key) {
    return helper->getParam(key);
}

MTS_BackgroundHelper::MTS_BackgroundHelper(shared_ptr<MTS_BaseHelper> h)
    //:helper(make_shared<MTS_BaseHelper>(*h)),  // initialize fields
    :helper(&(*h)),  // initialize fields
    bias_var_dist(h->getParam(string("bias_std_alpha")),
            h->getParam(string("bias_std_beta"))),
    bias_var_gen(h->rng2_, bias_var_dist),
    texture_distribution(h->getParam(string("texture_width_alpha")), 
            h->getParam(string("texture_width_beta"))),
    texture_distrib_gen(h->rng2_, texture_distribution)
{}

MTS_BackgroundHelper::~MTS_BackgroundHelper(){
    cout << "bg helper destructed" << endl;
}

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
    double dash[dash_len], offset[dash_len];
    cairo_get_dash(cr, dash, offset);

    // calculate a distance between lines
    int dist_min = getParam(string("boundary_distance_min"));
    int dist_max = getParam(string("boundary_distance_max"))+1 - dist_min;
    //double distance = (dist_min + helper->rng() % dist_max) * linewidth;
    double x_dis = (dist_min + helper->rng() % dist_max) * linewidth;
    double y_dis = (dist_min + helper->rng() % dist_max) * linewidth;

    // set boundary line characteristics
    int width_min = getParam(string("boundary_linewidth_min"));
    int width_max = getParam(string("boundary_linewidth_max"))+1 - width_min;
    double new_linewidth = linewidth * (width_min + helper->rng() % width_max);
    cairo_set_line_width(cr, new_linewidth);
    cairo_set_dash(cr, dash, 0,0); //set dash pattern to none

    // set boundary line gray-scale color (lighter than original)
    int color_min = 100 * getParam(string("boundary_color_diff_min"));
    int color_max = 100 * getParam(string("boundary_color_diff_max"))+1 - color_min;
    double color_diff = (color_min + helper->rng() % color_max) / 100.0;
    double color = og_col + color_diff;
    cairo_set_source_rgb(cr, color, color, color);

    // stroke the boundary line
    cairo_stroke_preserve(cr);

    // translate distance so that main line is drawn off center of boundary
    //draw_parallel(cr, horizontal, distance, false); //doesn't stroke new path
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
    int width_min = getParam(string("railroad_cross_width_min"));
    int width_max = getParam(string("railroad_cross_width_max"))+1 - width_min;
    double wide = (width_min + (helper->rng() % width_max)) * linewidth;
    cairo_set_line_width(cr, wide);

    // set distance between hatches
    int dist_min = getParam(string("railroad_distance_between_crosses_min"));
    int dist_max = getParam(string("railroad_distance_between_crosses_max"))+1 - dist_min;

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
    //cairo_set_dash(cr, pattern, 0, 0); 
    cairo_set_dash(cr, dash, dash_len, 0);
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
    int pat_len_min = getParam("dash_pattern_len_min");
    int pat_len_max = getParam("dash_pattern_len_max");
    int pattern_len = pat_len_min + (helper->rng() % (pat_len_max-pat_len_min+1));
    double dash_pattern[pattern_len];
    double dash;

    int len_min = (int)(getParam("dash_len_min")*10000);
    int len_max = (int)(getParam("dash_len_max")*10000);
    double len;

    //make and set pattern
    for(int i = 0; i < pattern_len; i++) {
        len = (helper->rng()%(len_max-len_min+1)+len_min)/10000.0;
        dash_pattern[i] = len;
    }

    //make_dash_pattern(dash_pattern, pattern_len);
    cairo_set_dash(cr, dash_pattern, pattern_len, 0);
}

coords
MTS_BackgroundHelper::orient_path(cairo_t *cr, bool horizontal, bool curved, 
        int length, int width, int height) {
    double x=0,y=0,angle;
    int translation_x, translation_y;
    angle = (helper->rng() % 360)/180.0*M_PI; //get angle 0 - 2PI
    translation_x = helper->rng() % (width+1); // 0 - width
    translation_y = helper->rng() % (height+1); // 0 - height 

    cairo_translate(cr, translation_x, translation_y);
    cairo_rotate(cr, angle); 
    cairo_translate(cr, -length/2.0, -height);


    /*
    // randomly set translation and rotation based on orientation 
    if(horizontal) { //horizontal line orientation
        //make a starting point for straight lines
        x = 0;
        y = helper->rng() % (height + 1); // y variation of 0-height along left side

        //make a starting translation and angle
        angle = ((helper->rng() % 91)-45)/180.0*M_PI; //get angle +- PI/4
        translation_x = helper->rng() % (width+1); // 0 - width
        translation_y = helper->rng() % (height+1); // 0 - width
        //if(curved) { translation_y = -fabs(y); } // translate points up from bottom
        //else { translation_y = 0; }

        cairo_translate(cr, translation_x, translation_y);
        cairo_rotate(cr, angle); 
        cairo_translate(cr, -width/2.0, -height);

    } else { //vertical line orientation
        //make a starting point for straight lines
        y = 0;
        x = width - (helper->rng() % width); // x variation of 0-width along top side

        //make a starting translation and angle
        angle = -((3 * 7854) - 2 * (helper->rng() % 7854))/10000.0; // from PI/4 to 3PI/4

        //translate to approximate curved line center point, rotate around it, 
        //  translate back 
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
    */
    //set and return starting coordinats
    coords start(x,y);
    return start;
}

void
MTS_BackgroundHelper::generate_curve(cairo_t *cr, bool horizontal, int width, 
        int height,  double c_min, double c_max, double d_min, double d_max) {

    std::vector<coords> points;
    //PangoLayout *layout;
    //cairo_path_t *path;
    //PangoLayoutLine *line;
    int num_min = getParam("bg_curve_num_points_min");
    int num_max = getParam("bg_curve_num_points_max");
    int num_points = num_min + (helper->rng() % (num_max-num_min+1));

    double y_var_min = getParam("bg_curve_y_variance_min");
    double y_var_max = getParam("bg_curve_y_variance_max");

    //get correct point vector based on line orientation
    points = helper->make_points_wave(width, height, num_points, y_var_min, y_var_max);
    helper->points_to_path(cr, points, c_min, c_max, d_min, d_max);
    /*
    if(horizontal) {
        points = helper->make_points_wave(width, height, num_points, y_var_min, y_var_max);
    } else {
        points = helper->make_points_wave(height, height, num_points, y_var_min, y_var_max);
    }
    */

    //curve the path and give extra optional parameter to stroke the path
    //helper->create_curved_path(cr,path,line,layout,width,
    //        height,0,0,points,c_min,c_max,d_min,d_max,true);
}


void
MTS_BackgroundHelper::addLines(cairo_t *cr, bool boundary, bool hatched, 
        bool dashed, bool curved, bool doubleline, 
        bool horizontal, int width, int height, 
        double c_min, double c_max, double d_min, double d_max, double color){

    double magic_line_ratio, line_width;
    //coords start_point;

    //set line color and width
    //cairo_set_source_rgb(cr, color, color, color); // gray-scale
    // set ratio to keep line scaled for image size
    int ratio_min = (int)(getParam("line_width_scale_min") * 10000);
    int ratio_max = 1 + (int)(getParam("line_width_scale_max") * 10000) - ratio_min;
    magic_line_ratio = (ratio_min + helper->rng() % ratio_max) / 10000.0; 
    line_width = std::min(width, height) * magic_line_ratio;
    cairo_set_line_width(cr, line_width);

    //move to origin of surface
    //cairo_move_to(cr, 0, 0);

    int length = (int)(pow(pow(width,2)+pow(height,2),0.5));

    //orient the path for the line correctly
    //start_point = orient_path(cr, horizontal, curved, length, width, height);
    orient_path(cr, horizontal, curved, length, width, height);

    // set path shape 
    if(curved) { 
        // draw a wiggly line
        generate_curve(cr, horizontal, length, height, c_min, c_max, d_min, d_max);
    } else { // draw a straight line
        // move to starting point
        //cairo_move_to(cr, start_point.first, start_point.second); 
        cairo_move_to(cr, 0, 0);
        cairo_line_to(cr, length, 0); 
        /*
        if(horizontal) {
            // make a line to width and a random height
            cairo_line_to(cr, width, helper->rng() % height); 
        } else { //vertical 
            // make a line to height and a random width
            cairo_line_to(cr, helper->rng() % width, height); 
        }
        */
    } 

    // set line style to dashed or not (default solid)
    if(dashed) { set_dash_pattern(cr); } 

    // set boundary or not
    if(boundary) { draw_boundary(cr, line_width, color, horizontal); }

    // set hatching or not
    if(hatched) { draw_hatched(cr, line_width); } 

    // draw parallel or not
    if(doubleline) { 
        cairo_stroke_preserve(cr);
        //draw_parallel(cr, horizontal, 3*line_width); 
        int dist_min = getParam(string("double_distance_min"));
        int dist_max = getParam(string("double_distance_max"))+1 - dist_min;
        double x_dis = (dist_min + helper->rng() % dist_max) * line_width;
        double y_dis = (dist_min + helper->rng() % dist_max) * line_width;
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

    int length = (int)(pow(pow(width,2)+pow(height,2),0.5));
    //orient the path for the line correctly
    start_point = orient_path(cr, horizontal, curved, length, width, height);

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
    cout << "start adding bias" << endl;
    cairo_pattern_t *pattern_vertical = cairo_pattern_create_linear(helper->rng()%width, 0,
            helper->rng()%width, height);
    cairo_pattern_t *pattern_horizontal = cairo_pattern_create_linear(0, helper->rng()%height,
            width, helper->rng()%height);

    // set the number of points
    int points_min = getParam(string("bias_vert_num_min"));
    int points_max = getParam(string("bias_vert_num_max"))+1 - points_min;
    int num_points_vertical = helper->rng()%points_max + points_min;
    int num_points_horizontal = helper->rng()%points_max + points_min;

    if (width > height) {
        // num points in horizontal direction is multiplied by ratio width/height
        num_points_horizontal = helper->rng()%((width/height)*points_max) + points_min; 
    }
    double offset_vertical = 1.0 / (num_points_vertical - 1);
    double offset_horizontal = 1.0 / (num_points_horizontal - 1);

    // get and set bias std variables
    double std_scale = getParam(string("bias_std_scale"));
    double std_shift = getParam(string("bias_std_shift"));
    double mean = getParam(string("bias_mean"));

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
        color_stop_val = min(color_stop_val, 255);
        color_stop_val = max(color_stop_val, 0);
        dcolor = color_stop_val / 255.0;

        cairo_pattern_add_color_stop_rgb(pattern_vertical, i*offset_vertical, dcolor,dcolor,dcolor);
    }
    // add color stops for each point along the horizontal line
    for (int i = 0; i < num_points_horizontal; i++){
        color_stop_val = color + (int)round(bias_gen());
        // bound the number between 0 and 255
        color_stop_val = min(color_stop_val, 255);
        color_stop_val = max(color_stop_val, 0);
        dcolor = color_stop_val / 255.0;

        cairo_pattern_add_color_stop_rgb(pattern_horizontal, i*offset_horizontal, dcolor,dcolor,dcolor);
    }

    cairo_set_source(cr, pattern_horizontal);
    cairo_paint_with_alpha(cr,0.3);
    cairo_set_source(cr, pattern_vertical);
    cairo_paint_with_alpha(cr,0.3);
    cout << "finished bias" << endl;

}


void
MTS_BackgroundHelper::addBgPattern (cairo_t *cr, int width, int height, 
        bool even, bool grid, bool curved) {

    double line_width, magic_line_ratio;
    int ratio_min = (int)(getParam("line_width_scale_min") * 10000);
    int ratio_max = 1 + (int)(getParam("line_width_scale_max") * 10000) - ratio_min;
    magic_line_ratio = (ratio_min + helper->rng() % ratio_max) / 10000.0; 
    line_width = std::min(width, height) * magic_line_ratio;
    cairo_set_line_width(cr, line_width);

    //randomly choose number of lines 
    int lines_min, lines_max;
    if (grid) { // correctly get number of lines to draw from user config
        lines_min = getParam(string("grid_num_min"));
        lines_max = getParam(string("grid_num_max"))+1 - lines_min;
    } else if (even) {
        lines_min = getParam(string("para_num_min"));
        lines_max = getParam(string("para_num_max"))+1 - lines_min;
    } else {
        lines_min = getParam(string("vpara_num_min"));
        lines_max = getParam(string("vpara_num_max"))+1 - lines_min;
    }
    int num = helper->rng()%lines_max + lines_min;
    cout << "line nums " << num << endl;

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
    double cur_y = -(length-height)/2.0+init_spacing;
    double increase = (spacing-init_spacing)*2/num;

    // to store the real points of every line
    std::vector<coords> points;

    // to store the curve pattern
    std::vector<coords> curve_points;

    if (curved) {
        double y_var_min = getParam("bg_curve_y_variance_min");
        double y_var_max = getParam("bg_curve_y_variance_max");
        int num_min = getParam("bg_curve_num_points_min");
        int num_max = getParam("bg_curve_num_points_max");
        int num_points = num_min + (helper->rng() % (num_max-num_min+1));
        curve_points = helper->make_points_wave(length,length,num_points,y_var_min,y_var_max);
    }


    for (int i = 0; i < num; i++) {
        if (curved) {
            for (int k = 0; k < curve_points.size(); k++) {
                points.push_back(std::make_pair(left_x+curve_points[k].first, curve_points[k].second-(length-cur_y)));
            }
        } else {
            points.push_back(std::make_pair(left_x,cur_y));
            points.push_back(std::make_pair(right_x,cur_y));
        }
        lines.push_back(points);
        points.clear();
        cur_y += init_spacing + increase;
        init_spacing += increase;
    }

    double c_min, c_max, d_min, d_max;

    if (curved) {
        if (even) {
            c_min = getParam(string("para_curve_c_min"));
            c_max = getParam(string("para_curve_c_max"));
            d_min = getParam(string("para_curve_d_min"));
            d_max = getParam(string("para_curve_d_max"));
        } else {
            c_min = getParam(string("vpara_curve_c_min"));
            c_max = getParam(string("vpara_curve_c_max"));
            d_min = getParam(string("vpara_curve_d_min"));
            d_max = getParam(string("vpara_curve_d_max"));
        }
    }

    // get line i and make the points into a cairo_path
    for (int i = 0; i < num; i++){
        points = lines[i];
        if (curved) {
            helper->points_to_path(cr, points, c_min, c_max, d_min, d_max); //draw path shape
        } else {
            //cout << points[0].first << " " << points[0].second << endl;
            //cout << points[points.size()-1].first << " " << points[points.size()-1].second << endl;
            cairo_move_to(cr,points[0].first,points[0].second);
            cairo_line_to(cr,points[points.size()-1].first,points[points.size()-1].second);
        }
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
                helper->points_to_path(cr, points, c_min, c_max, d_min, d_max); //draw path shape
            } else {
                cairo_move_to(cr,points[0].first,points[0].second);
                cairo_line_to(cr,points[points.size()-1].first,points[points.size()-1].second);
            }
            cairo_stroke(cr);
        }
    }

    cairo_identity_matrix(cr);
}


void
MTS_BackgroundHelper::colorDiff (cairo_t *cr, int width, int height, 
        double color_min, double color_max) {

    int num_colors_min = getParam("diff_num_colors_min");
    int num_colors_max = getParam("diff_num_colors_max");

    int num = (helper->rng() % (num_colors_max - num_colors_min + 1)) + num_colors_min; 

    for (int i = 0; i < num; i++) {
        double color = helper->rng()%((int)((color_max-color_min)*100)+1)/100.0+color_min;
        cairo_set_source_rgb(cr,color,color,color);

        bool horizontal = helper->rng()%2;

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
}


void
MTS_BackgroundHelper::cityPoint(cairo_t *cr, int width, int height, bool hollow) {

    //options for side of the surface the point origin appears on
    enum Side { left, right, top, bottom }; // (top from user perspective)

    int option = helper->rng() % 4; // choose a Side for circle origin
    int x,y; // circle origin coordinates

    // set point radius
    int r_min = (int)(getParam("point_radius_min") * height);
    int r_max = (int)(getParam("point_radius_max") * height);
    if (r_max > (height/2)) r_max = height / 2; //verify perconditions

    int radius = (helper->rng() % (r_max - r_min + 1)) + r_min; 

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
    if (hollow) {
        double line_width, magic_line_ratio;
        int ratio_min = (int)(getParam("line_width_scale_min") * 10000);
        int ratio_max = 1 + (int)(getParam("line_width_scale_max") * 10000) - ratio_min;
        magic_line_ratio = (ratio_min + helper->rng() % ratio_max) / 10000.0; 
        line_width = std::min(width, height) * magic_line_ratio;
        cairo_set_line_width(cr, line_width);
        cairo_stroke(cr);
    } else {
        cairo_fill(cr);
    }
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

    std::vector<BGFeature> all_features={Colordiff, Distracttext, Boundary, Colorblob, Straight, Grid, Citypoint, Parallel, Vparallel, Texture, Railroad, Riverline};
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
    cout << "got all features" << endl;
}


void 
MTS_BackgroundHelper::generateBgSample(cairo_surface_t *&bg_surface, std::vector<BGFeature> &features, int height, int width, int bg_color, int contrast){

    cout << "bg color " << bg_color << endl;
    cout << "constrast " << contrast << endl;

    double c_min, c_max, d_min, d_max, curve_prob;
    int num_lines;
    cout << "generating bg sample" << endl;
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

    //add background bias field
    addBgBias(cr, width, height, bg_color);

    if (find(features.begin(), features.end(), Colorblob)!= features.end()) {
        int num_min= (int)getParam("blob_num_min");
        int num_max= (int)getParam("blob_num_max");
        double size_min = getParam("blob_size_min");
        double size_max = getParam("blob_size_max");
        double dim_rate = getParam("blob_diminish_rate");
        //helper->addSpots(surface,num_min,num_max,size_min,size_max,dim_rate,
        //        false,bg_color-contrast, bg_color);
        helper->addSpots(surface,num_min,num_max,size_min,size_max,dim_rate,
                false,bg_color-contrast, bg_color);
    }

    // set background source brightness
    int color_dis_min = getParam("bg_feature_color_dis_min");
    int color_dis_max = getParam("bg_feature_color_dis_max");
    if (color_dis_max > contrast) color_dis_max = contrast;
    int text_color = bg_color - contrast;
    double color = (text_color + color_dis_min + helper->rng() % (color_dis_max-color_dis_min + 1)) / 255.0;
    cairo_set_source_rgb(cr,color,color,color);

    // GENERATE BACKGROUND FEATURES:
    // add texture swaths by probability
    if (find(features.begin(), features.end(), Texture)!= features.end()) {
        c_min = getParam("texture_curve_c_min");
        c_max = getParam("texture_curve_c_max");
        d_min = getParam("texture_curve_d_min");
        d_max = getParam("texture_curve_d_max");

        int num_lines_min = getParam("texture_num_lines_min");
        int num_lines_max = getParam("texture_num_lines_max");
        num_lines = helper->rng()%(num_lines_max-num_lines_min+1) + num_lines_min; 

        // add num_lines lines iteratively
        for (int i = 0; i < num_lines; i++) {
            addTexture(cr, helper->rng()%2, helper->rng()%2, color, 
                    width, height, c_min, c_max, d_min, d_max); 
        }
    }

    // add evenly spaced parallel lines by probability
    if (find(features.begin(), features.end(), Parallel)!= features.end()) {
        curve_prob = getParam(string("para_curve_prob"));
        addBgPattern(cr, width, height, true, false, helper->rndProbUnder(curve_prob));
    }

    // add varied parallel lines by probability
    if (find(features.begin(), features.end(), Vparallel)!= features.end()) {
        curve_prob = getParam(string("vpara_curve_prob"));
        addBgPattern(cr, width, height, false, false, helper->rndProbUnder(curve_prob));
    }

    // add grid lines by probability
    if (find(features.begin(), features.end(), Grid)!= features.end()) {
        curve_prob = getParam(string("grid_curve_prob"));
        addBgPattern(cr, width, height, true, true, helper->rndProbUnder(curve_prob));
    }

    // add railroads by probability
    if (find(features.begin(), features.end(), Railroad)!= features.end()) {
        int railroad_min = getParam(string("railroad_num_lines_min"));
        int railroad_max = getParam(string("railroad_num_lines_max"))+1 - railroad_min;
        c_min = getParam("railroad_curve_c_min");
        c_max = getParam("railroad_curve_c_max");
        d_min = getParam("railroad_curve_d_min");
        d_max = getParam("railroad_curve_d_max");
        num_lines = helper->rng()%railroad_max + railroad_min;

        // add num_lines lines iteratively
        for (int i = 0; i < num_lines; i++) {
            addLines(cr, false, true, false, true, false, helper->rng()%2, width, height, c_min, c_max, d_min, d_max);
        }
    }

    // add boundary lines by probability
    if (find(features.begin(), features.end(), Boundary)!= features.end()) {
        int boundary_min = getParam(string("boundary_num_lines_min"));
        int boundary_max = getParam(string("boundary_num_lines_max"))+1 - boundary_min;

        num_lines = helper->rng()%boundary_max + boundary_min;
        double dash_probability = getParam("boundary_dashed_prob");
        c_min = getParam("boundary_curve_c_min");
        c_max = getParam("boundary_curve_c_max");
        d_min = getParam("boundary_curve_d_min");
        d_max = getParam("boundary_curve_d_max");

        // add num_lines lines iteratively
        for (int i = 0; i < num_lines; i++) {
            addLines(cr, true, false, helper->rndProbUnder(dash_probability), true, false,
                    helper->rng()%2, width, height, c_min, c_max, d_min, d_max, color);
        }
    }

    // add straight lines by probability
    if (find(features.begin(), features.end(), Straight)!= features.end()) {
        int straight_min = getParam(string("straight_num_lines_min"));
        int straight_max = getParam(string("straight_num_lines_max"))+1 - straight_min;
        num_lines = helper->rng() % straight_max + straight_min;

        // add num_lines lines iteratively
        for (int i = 0; i < num_lines; i++) {
            addLines(cr, false, false, false, false, false, helper->rng()%2, 
                    width, height);
        }
    }

    // add rivers by probability
    if (find(features.begin(), features.end(), Riverline)!= features.end()) {
        int river_min = getParam(string("river_num_lines_min"));
        int river_max = getParam(string("river_num_lines_max"))+1 - river_min;

        num_lines = helper->rng()%river_max + river_min;
        c_min = getParam("river_curve_c_min");
        c_max = getParam("river_curve_c_max");
        d_min = getParam("river_curve_d_min");
        d_max = getParam("river_curve_d_max");

        // add num_lines lines iteratively
        for (int i = 0; i < num_lines; i++) {
            addLines(cr, false, false, false, true, helper->rng()%2, 
                    helper->rng()%2, width, height, c_min, c_max, d_min, d_max);
        }
    }

    // add city point by probability
    if (find(features.begin(), features.end(), Citypoint)!= features.end()) {
        double hollow = getParam("point_hollow_prob");
        int num_min = getParam("point_num_min");
        int num_max = getParam("point_num_max");
        int point_num = helper->rng()%(num_max-num_min+1)+num_min;
        for (int i = 0; i < point_num; i++) {
            cityPoint(cr, width, height, helper->rndProbUnder(hollow));
        }
    }

    bg_surface = surface;
}
