#include <vector>
#include <math.h>
#include <cmath>
#include <algorithm>
#include <stdlib.h>
#include <iostream> 
#include <unordered_map>
#include <memory>

#include <pango/pangocairo.h>

#include "mts_basehelper.hpp"

using namespace std;
using boost::random::mt19937;


// SEE mts_basehelper.hpp FOR ALL DOCUMENTATION

MTS_BaseHelper::MTS_BaseHelper(shared_ptr<MTSConfig> c)
    :config(&(*c))
{
}

MTS_BaseHelper::~MTS_BaseHelper(){
    cout << "base helper destructed" << endl;
}

bool 
MTS_BaseHelper::rndProbUnder(double probability){
    return (rng() % 10000) < (10000 * probability);
}

void
MTS_BaseHelper::setSeed(uint64 rndState){
    rng_.state = rndState;
}

unsigned int
MTS_BaseHelper::rng(){
    return rng_.next();
}

void 
MTS_BaseHelper::addSpots (cairo_surface_t *surface, int num_min, int num_max, double size_min,
        double size_max, double diminish_rate, bool transparent, int color_min, int color_max){

    // get the width and height of the image
    int height = cairo_image_surface_get_height(surface);
    int width = cairo_image_surface_get_width(surface);

    double prob; //0~100
    unsigned char *data, *data_t;

    int stride = cairo_format_stride_for_width(CAIRO_FORMAT_A8, width);
    // make space in memory for image data for the cairo mask
    data = (unsigned char *) malloc(stride * height);

    int num_spots = rng() % (num_max - num_min + 1) + num_min;
    int x_coords[num_spots];
    int y_coords[num_spots];
    double radii[num_spots];

    int size_min_int = (int)(100 * size_min);
    int size_max_int = (int)(100 * size_max);

    // get random xy coords and the radius for each spot
    for (int i = 0; i < num_spots; i++) {
        x_coords[i] = rng() % stride;
        y_coords[i] = rng() % height;
        double shrink = (rng() % (size_max_int - size_min_int + 1) + size_min_int) / 100.0;
        radii[i] = shrink * height;
    }

    // populate data as all 0s
    for (int row = 0; row < height; row++) {
        for (int column = 0; column < stride; column++) {
            data[row * stride + column] = 0;
        }
    }

    // iterate through mask and set every pixel in range of hole to have value 255
    // the range is calculated with a logistic function
    for (int i = 0; i < num_spots; i++) {
        // get data about hole i
        int x = x_coords[i];
        int y = y_coords[i];
        double rad = radii[i];

        int color = rng() % (color_max-color_min+1) + color_min;
        unsigned char trans = 255 - color;

        // iterate through mask and set pixels within range to 255
        for (int row = 0; row < height; row++) {
            for (int column = 0; column < stride; column++) {
                double dis = pow(pow((double)(row-y),2)+pow((double)(column-x),2),0.5);
                prob = 100 - (100 / (1 + diminish_rate * exp(-(dis - rad))));
                if (rng() % 100 < prob) {
                    if (transparent) {
                        data[row * stride + column] = 255;
                    } else {
                        data[row * stride + column] = trans;
                    }
                }
            }
        }
    }

    // if transparent then loop through surface to set rgba channel to 0s
    if (transparent) {
        data_t = cairo_image_surface_get_data(surface);
        int stride_t = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, width);
        for (int row = 0; row < height; row++) {
            for (int column = 0; column < stride; column++) {
                if (data[row * stride + column] == 255) {
                    if (column * 4 + 3 < stride_t) {
                        data_t[row * stride_t + column * 4] = 0; 
                        data_t[row * stride_t+column * 4 + 1] = 0; 
                        data_t[row * stride_t+column * 4 + 2] = 0; 
                        data_t[row * stride_t+column * 4 + 3] = 0; 
                    }
                }
            }
        }
        cairo_surface_mark_dirty(surface);
        cairo_surface_flush(surface);
        free(data);
    } else { // otherwise, construct a new mask from data
        // create new mask and cairo context to hold it
        cairo_surface_t *mask;
        mask = cairo_image_surface_create_for_data(data, CAIRO_FORMAT_A8, 
                width, height,stride); 

        cairo_t *cr;
        cr = cairo_create (surface);

        // set mask brightness
        cairo_set_source_rgb(cr,0,0,0);

        // apply mask to surface
        cairo_mask_surface(cr, mask, 0, 0);

        // clean up
        cairo_destroy (cr);
        cairo_surface_destroy (mask);
    }

}

///////////////////////////// from behdad's cairotwisted.c (required functions)
/*
 * Written by Behdad Esfahbod, 2006..2007
 *
 * Permission to use, copy, modify, distribute, and sell this example
 * for any purpose is hereby granted without fee.
 * It is provided "as is" without express or implied warranty.
 */

double
MTS_BaseHelper::two_points_distance (cairo_path_data_t *a, cairo_path_data_t *b)
{
    double dx, dy;

    dx = b->point.x - a->point.x;
    dy = b->point.y - a->point.y;

    return sqrt (dx * dx + dy * dy);
}


double
MTS_BaseHelper::curve_length (double x0, double y0,
        double x1, double y1,
        double x2, double y2,
        double x3, double y3)
{
    cairo_surface_t *surface;
    cairo_t *cr;
    cairo_path_t *path;
    cairo_path_data_t *data, current_point;
    int i;
    double length;

    surface = cairo_image_surface_create (CAIRO_FORMAT_A8, 0, 0);
    cr = cairo_create (surface);
    cairo_surface_destroy (surface);

    cairo_move_to (cr, x0, y0);
    cairo_curve_to (cr, x1, y1, x2, y2, x3, y3);

    length = 0;
    path = cairo_copy_path_flat (cr);
    for (i=0; i < path->num_data; i += path->data[i].header.length) {
        data = &path->data[i];
        switch (data->header.type) {

            case CAIRO_PATH_MOVE_TO:
                current_point = data[1];
                break;

            case CAIRO_PATH_LINE_TO:
                length += two_points_distance (&current_point, &data[1]);
                current_point = data[1];
                break;

            default:
            case CAIRO_PATH_CURVE_TO:
            case CAIRO_PATH_CLOSE_PATH:
                g_assert_not_reached ();
        }
    }
    cairo_path_destroy (path);

    cairo_destroy (cr);

    return length;
}


parametrization_t *
MTS_BaseHelper::parametrize_path (cairo_path_t *path)
{
    int i;
    cairo_path_data_t *data, last_move_to, current_point;
    parametrization_t *parametrization;

    parametrization = static_cast<parametrization_t*>(g_malloc (path->num_data * 
                sizeof (parametrization_t)));

    for (i=0; i < path->num_data; i += path->data[i].header.length) {
        data = &path->data[i];
        parametrization[i] = 0.0;
        switch (data->header.type) {
            case CAIRO_PATH_MOVE_TO:
                last_move_to = data[1];
                current_point = data[1];
                break;
            case CAIRO_PATH_CLOSE_PATH:
                /* Make it look like it's a line_to to last_move_to */
                data = (&last_move_to) - 1;
                /* fall through */
            case CAIRO_PATH_LINE_TO:
                parametrization[i] = two_points_distance (&current_point, &data[1]);
                current_point = data[1];
                break;
            case CAIRO_PATH_CURVE_TO:
                /*//naive curve-length, treating bezier as three line segments:
                  parametrization[i] = two_points_distance (&current_point, &data[1])
                  + two_points_distance (&data[1], &data[2])
                  + two_points_distance (&data[2], &data[3]);
                 */ 
                parametrization[i] = curve_length (current_point.point.x, 
                        current_point.point.y,
                        data[1].point.x, data[1].point.y,
                        data[2].point.x, data[2].point.y,
                        data[3].point.x, data[3].point.y);

                current_point = data[3];
                break;
            default:
                g_assert_not_reached ();
        }
    }

    return parametrization;
}


void
MTS_BaseHelper::transform_path (cairo_path_t *path, transform_point_func_t f, 
        void *closure)
{
    int i;
    cairo_path_data_t *data;

    for (i=0; i < path->num_data; i += path->data[i].header.length) {
        data = &path->data[i];
        switch (data->header.type) {
            case CAIRO_PATH_CURVE_TO:
                f (closure, &data[3].point.x, &data[3].point.y);
                f (closure, &data[2].point.x, &data[2].point.y);
            case CAIRO_PATH_MOVE_TO:
            case CAIRO_PATH_LINE_TO:
                f (closure, &data[1].point.x, &data[1].point.y);
                break;
            case CAIRO_PATH_CLOSE_PATH:
                break;
            default:
                g_assert_not_reached ();
        }
    }
}


void
MTS_BaseHelper::point_on_path (parametrized_path_t *param,
        double *x, double *y)
{
    int i;
    double ratio, the_y = *y, the_x = *x, dx, dy;
    cairo_path_data_t *data, last_move_to, current_point;
    cairo_path_t *path = param->path;
    parametrization_t *parametrization = param->parametrization;

    for (i=0; i + path->data[i].header.length < path->num_data &&
            (the_x > parametrization[i] || path->data[i].header.type == 
             CAIRO_PATH_MOVE_TO); i += path->data[i].header.length) {

        the_x -= parametrization[i];
        data = &path->data[i];
        switch (data->header.type) {
            case CAIRO_PATH_MOVE_TO:
                current_point = data[1];
                last_move_to = data[1];
                break;
            case CAIRO_PATH_LINE_TO:
                current_point = data[1];
                break;
            case CAIRO_PATH_CURVE_TO:
                current_point = data[3];
                break;
            case CAIRO_PATH_CLOSE_PATH:
                break;
            default:
                g_assert_not_reached ();
        }
    }
    data = &path->data[i];

    switch (data->header.type) {

        case CAIRO_PATH_MOVE_TO:
            break;
        case CAIRO_PATH_CLOSE_PATH:
            /* Make it look like it's a line_to to last_move_to */
            data = (&last_move_to) - 1;
            /* fall through */
        case CAIRO_PATH_LINE_TO:
            {
                ratio = the_x / parametrization[i];
                /* Line polynomial */
                *x = current_point.point.x * (1 - ratio) + data[1].point.x * ratio;
                *y = current_point.point.y * (1 - ratio) + data[1].point.y * ratio;

                /* Line gradient */
                dx = -(current_point.point.x - data[1].point.x);
                dy = -(current_point.point.y - data[1].point.y);

                /*optimization for: ratio = the_y / sqrt (dx * dx + dy * dy);*/
                ratio = the_y / parametrization[i];
                *x += -dy * ratio;
                *y +=  dx * ratio;
            }
            break;
        case CAIRO_PATH_CURVE_TO:
            {
                /* FIXME the formulas here are not exactly what we want, because the
                 * Bezier parametrization is not uniform.  But I don't know how to do
                 * better.  The caller can do slightly better though, by flattening the
                 * Bezier and avoiding this branch completely.  That has its own cost
                 * though, as large y values magnify the flattening error drastically.
                 *///interpolation?

                double ratio_1_0, ratio_0_1;
                double ratio_2_0, ratio_0_2;
                double ratio_3_0, ratio_2_1, ratio_1_2, ratio_0_3;
                double _1__4ratio_1_0_3ratio_2_0, _2ratio_1_0_3ratio_2_0;

                ratio = the_x / parametrization[i];

                ratio_1_0 = ratio;
                ratio_0_1 = 1 - ratio;

                ratio_2_0 = ratio_1_0 * ratio_1_0; /*      ratio  *      ratio  */
                ratio_0_2 = ratio_0_1 * ratio_0_1; /* (1 - ratio) * (1 - ratio) */

                ratio_3_0 = ratio_2_0 * ratio_1_0; /*      ratio  *      ratio  *      ratio  */
                ratio_2_1 = ratio_2_0 * ratio_0_1; /*      ratio  *      ratio  * (1 - ratio) */
                ratio_1_2 = ratio_1_0 * ratio_0_2; /*      ratio  * (1 - ratio) * (1 - ratio) */
                ratio_0_3 = ratio_0_1 * ratio_0_2; /* (1 - ratio) * (1 - ratio) * (1 - ratio) */

                _1__4ratio_1_0_3ratio_2_0 = 1 - 4 * ratio_1_0 + 3 * ratio_2_0;
                _2ratio_1_0_3ratio_2_0    =     2 * ratio_1_0 - 3 * ratio_2_0;

                /* Bezier polynomial */
                *x = current_point.point.x * ratio_0_3
                    + 3 *   data[1].point.x * ratio_1_2
                    + 3 *   data[2].point.x * ratio_2_1
                    +       data[3].point.x * ratio_3_0;
                *y = current_point.point.y * ratio_0_3
                    + 3 *   data[1].point.y * ratio_1_2
                    + 3 *   data[2].point.y * ratio_2_1
                    +       data[3].point.y * ratio_3_0;

                /* Bezier gradient */
                dx =-3 * current_point.point.x * ratio_0_2
                    + 3 *       data[1].point.x * _1__4ratio_1_0_3ratio_2_0
                    + 3 *       data[2].point.x * _2ratio_1_0_3ratio_2_0
                    + 3 *       data[3].point.x * ratio_2_0;
                dy =-3 * current_point.point.y * ratio_0_2
                    + 3 *       data[1].point.y * _1__4ratio_1_0_3ratio_2_0
                    + 3 *       data[2].point.y * _2ratio_1_0_3ratio_2_0
                    + 3 *       data[3].point.y * ratio_2_0;

                ratio = the_y / sqrt (dx * dx + dy * dy);
                *x += -dy * ratio;
                *y +=  dx * ratio;
            }
            break;
        default:
            g_assert_not_reached ();
    }
}

void
MTS_BaseHelper::map_path_onto (cairo_t *cr, cairo_path_t *path)
{
    cairo_path_t *current_path;
    parametrized_path_t param;

    param.path = path;
    param.parametrization = parametrize_path (path);

    current_path = cairo_copy_path (cr);
    cairo_new_path (cr);

    transform_path (current_path,
            (transform_point_func_t) point_on_path, &param);

    cairo_append_path (cr, current_path);

    cairo_path_destroy (current_path);
    g_free (param.parametrization);
}

/////////////https://github.com/phuang/pango/blob/master/examples/cairotwisted.c


void 
MTS_BaseHelper::four_point_to_cp(coords start,
        coords f1,
        coords f2,
        coords end,
        coords *cp1,
        coords *cp2) {

    /* 
       Calculate control points of a Bezier curve using linear algebra

       f1=(1-t)^3 start + 3(1-t)^2 t cp1 + 3(1-t) t^2 cp2 + t^3 end
       f1=8/27 start + 12/27 cp1 + 6/27 cp2 + 1/27 end
       f2=1/27 start + 6/27 cp1 + 12/27 cp2 + 8/27 end


       f1.x - 1/27 end.x - 8/27 start.x = 12/27 cp1.x + 6/27 cp2.x
       f1.y - 1/27 end.y - 8/27 start.y = 12/27 cp1.y + 6/27 cp2.y
       f2.x - 8/27 end.x - 1/27 start.x = 6/27 cp1.x + 12/27 cp2.x
       f2.y - 8/27 end.y - 1/27 start.y = 6/27 cp1.y + 12/27 cp2.y

       a1 = 12/27 x1 + 6/27 x2
       a2 = 6/27 x1 + 12/27 x2
       0.5 a1 = 6/27 x1 + 3/27 x2

       a2 - 0.5a1 = 9/27 x2

       b2 - 0.5b1 = 9/27 y2
     */

    double AB_DENOM = 27.0;   
    double AB_NUMER1 = 1.0;  
    double AB_NUMER2 = 8.0;  
    double XY_DENOM1 = 9.0;  
    double XY_DENOM2 = 27.0; 
    double XY_DENOM3 = 6.0;  
    double XY_NUMER1 = 27.0; 
    double XY_NUMER2 = 12.0; 
    double XY_MULTI = .5;  

    double a1 = f1.first - (AB_NUMER1/AB_DENOM)*end.first 
        - (AB_NUMER2/AB_DENOM)*start.first;
    double a2 = f2.first - (AB_NUMER2/AB_DENOM)*end.first 
        - (AB_NUMER1/AB_DENOM)*start.first;

    double x2 = (a2 - XY_MULTI*a1) * (XY_NUMER1/XY_DENOM1);
    double x1 = (a2 - (XY_NUMER2/XY_DENOM2)*x2) * (XY_NUMER1/XY_DENOM3);

    double b1 = f1.second - (AB_NUMER1/AB_DENOM)*end.second 
        - (AB_NUMER2/AB_DENOM)*start.second;
    double b2 = f2.second - (AB_NUMER2/AB_DENOM)*end.second  
        - (AB_NUMER1/AB_DENOM)*start.second;

    double y2 = (b2 - XY_MULTI*b1) * (XY_NUMER1/XY_DENOM1);
    double y1 = (b2 - (XY_NUMER2/XY_DENOM2)*y2) * (XY_NUMER1/XY_DENOM3);

    cp1->first = x1;
    cp1->second = y1;
    cp2->first = x2;
    cp2->second = y2;
}


void 
MTS_BaseHelper::points_to_path(cairo_t *cr, std::vector<coords> points, double cmin, double cmax, double dmin, double dmax, double cd_sum_max) {

    unsigned int count = points.size();

    if (count < 2) return; //verify preconditions

    // get start and end points and update count
    coords start = points.back(); 
    points.pop_back();
    coords end = points.back();
    points.pop_back();
    count -= 2;

    /* Calculations to solve cubic function with linear algebra
       y = a + bx + cx^2 + dx^3
       w = a + bu + cu^2 + du^3

       y - dx^3 - cx^2 = a + bx
       w - du^3 - cu^2 = a + bu

       (x/u)(w - du^3 - cu^2) = (x/u)a + (x/u)ub
       y - dx^3 - cx^2 - (x/u)(w - du^3 - cu^2) = (1 - x/u)a

       a = (y - dx^3 - cx^2 - (x/u)(w - du^3 - cu^2)) / (1 - x/u)
       b = (y - dx^3 - cx^2 - a) / x
     */

    double x = start.first / 100, y = start.second, u = end.first / 100, w = end.second;

    //cout << "x, y " << x*100 << " " << y  << endl;

    // set coefficients of cubic equation to describe curve
    double a=0, b=0, c=0, d=0;
    int c_variance, d_variance;
    c_variance = (int)((cmax*100) - (cmin*100) + 1);
    d_variance = (int)((dmax*100) - (dmin*100) + 1);

    // prevent both c and d become 0
    while (c==0 && d==0) {
        c = (rng() % c_variance) / 100.0 + cmin;

        if (count == 0) {
            d = 0;
        } else {
            d = (rng() % d_variance) / 100.0 + dmin;

            if (abs(c+d) > cd_sum_max) {
                d = cd_sum_max - c;
            }
        }
    }

    if (x == u) {
        cerr << "Cannot draw vertical curve in points_to_path()!" << endl;
        exit(1);
    }
    else if (x == 0) {
        a = y;
        b = (w - y - d*pow(u,3) - c*pow(u,2)) / u;
    } else if (u == 0) {
        a = w;
        b = (y - w - d*pow(x,3) - c*pow(x,2)) / x;
    } else {
        a = (y - d*pow(x,3) - c*pow(x,2) - (x/u)*(w - d*pow(u,3) - c*pow(u,2))) / (1 - x/u);
        b = (y - d*pow(x,3) - c*pow(x,2) - a) / x;
    }

    double coeff[4] = {a,b,c,d};

    //cout << "a b c d " << a << " " << b << " " << c << " " << d << endl;

    // get two points in the middle to calculate control points
    double x1 = (2.0/3)*x + (1.0/3)*u;
    double x2 = (1.0/3)*x + (2.0/3)*u;
    double y1 = coeff[0] + coeff[1]*x1 + coeff[2]*pow(x1,2) + coeff[3]*pow(x1,3);
    double y2 = coeff[0] + coeff[1]*x2 + coeff[2]*pow(x2,2) + coeff[3]*pow(x2,3);

    coords f1 = std::make_pair(x1*100,y1); 
    coords f2 = std::make_pair(x2*100,y2);

    coords cp1;
    coords cp2;

    four_point_to_cp(start, f1, f2, end, &cp1, &cp2);

    //draw a 1st curve using cp (curve points)
    cairo_move_to(cr, start.first, start.second);
    cairo_curve_to(cr, cp1.first, cp1.second, cp2.first, cp2.second, end.first, end.second);

    double first_degree_derivative = coeff[1] + 2*coeff[2]*u + 3*coeff[3]*pow(u,2);

    /*while still points left in vector, advance start to prev end and end to 
      next point and draw curve */
    while (count > 0) {
        start = end;
        end = points.back(); //pop next point from points vec
        points.pop_back();

        x = start.first / 100, y = start.second, u = end.first / 100, w = end.second;
        if (x == u) {
            cerr << "Cannot draw vertical curve in points_to_path()!" << endl;
            exit(1);
        }
        else {

            /*
               y = a + bx + cx^2 + dx^3
               w = a + bu + cu^2 + du^3

               first_degree_derivative = b + 2cx + 3dx^2


               (y+w)/2+100 = a + b(x+u)/2 + c((x+u)/2)^2 + d((x+u)/2)^3
               m = a + bn + cn^2 + dn^3

               solve the matrix
             */
            /*
               d = ((sdd/2)*(pow(x,2)-pow(u,2)-2*x*(x-u)))/
               ((pow(x,3)-pow(u,3))-3*pow(x,3)*(x-u)
               -3*x*(pow(x,2)-pow(u,2)-2*x*(x-u)));
               c = (y-w-first_degree_derivative*(x-u)-d*((pow(x,3)-pow(u,3))-3*pow(x,3)*(x-u)))/
               (pow(x,2)-pow(u,2)-2*x*(x-u));
               b = (y-w-d*(pow(x,3)-pow(u,3))-c*(pow(x,2)-pow(u,2)))/(x-u);
               a = y-d*pow(x,3)-c*pow(x,2)-b*x;
             */

            // (n,m) is the arbitrary new middle point
            double n = (x+u)/2;
            double m = (y+w)/2;

            //cout << "x, y, n, m " << x*100 << " " << y << " " << n*100 << " " << m << endl;

            double k = (x-u)/(x-n);
            double x_2 = pow(x,2), u_2 = pow(u,2), n_2 = pow(n,2);
            double x_3 = pow(x,3), u_3 = pow(u,3), n_3 = pow(n,3);

            double j = (x_2-u_2-(x_2-n_2)*k)/(x_2-u_2-2*x*(x-u));
            d = (y-w-(y-m)*k-j*(y-w-first_degree_derivative*(x-u)))/(x_3-u_3-(x_3-n_3)*k-j*(x_3-u_3-3*x_2*(x-u)));
            c = (y-w-(y-m)*k-d*(x_3-u_3-(x_3-n_3)*k))/(x_2-u_2-(x_2-n_2)*k);
            b = (y-w-d*(x_3-u_3)-c*(x_2-u_2))/(x-u);
            a = (y-d*x_3-c*x_2-b*x);
            //cout << "a b c d " << a << " " << b << " " << c << " " << d << endl;
        }

        coeff[0] = a, coeff[1] = b, coeff[2] = c, coeff[3] = d;

        // get two points in the middle to calculate control points
        x1 = (2.0/3)*x + (1.0/3)*u;
        x2 = (1.0/3)*x + (2.0/3)*u;
        y1 = coeff[0] + coeff[1]*x1 + coeff[2]*pow(x1,2) + coeff[3]*pow(x1,3);
        y2 = coeff[0] + coeff[1]*x2 + coeff[2]*pow(x2,2) + coeff[3]*pow(x2,3);

        f1.first = x1 * 100, f1.second = y1;
        f2.first = x2 * 100, f2.second = y2;

        four_point_to_cp(start, f1, f2, end, &cp1, &cp2);

        //draw next curve of the path
        cairo_curve_to(cr, cp1.first, cp1.second, cp2.first, cp2.second, end.first,
                end.second);

        first_degree_derivative = coeff[1] + 2*coeff[2]*u + 3*coeff[3]*pow(u,2);

        count -= 1;
    }
}


std::vector<coords>
MTS_BaseHelper::make_points_wave(double length, double height, 
        int num_points, double y_var_min_ratio, double y_var_max_ratio) {

    std::vector<coords> points;

    if (num_points < 2) num_points = 2; //verify preconditions

    int y_variance;
    double x, y;    

    //created num_points x,y coords
    for(int i = num_points - 1; i >= 0; i--) {
        int y_var_min = (int)(height * y_var_min_ratio);
        int y_var_max = (int)(height * y_var_max_ratio);

        y_variance = (rng() % (y_var_max - y_var_min + 1)) + y_var_min;

        x = ((length / (num_points - 1)) * i);
        y = height - y_variance; //ensure points stay above the bottom of the canvas

        coords new_point(x,y);
        points.push_back(new_point);
    }

    return points;
}
