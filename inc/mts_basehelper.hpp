#ifndef MTS_BASE_HELPER_HPP
#define MTS_BASE_HELPER_HPP

#include <unordered_map>
#include <memory>
#include <vector>

#include <boost/random.hpp>

#include <pango/pangocairo.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using boost::random::mt19937;

// All possible features that can be incorporated into a background
enum BGFeature {Colordiff=0, Distracttext, Boundary, Colorblob, 
    Straight, Grid, Citypoint, Parallel, 
    Vparallel, Texture, Railroad, Riverline};

// rename pair of doubles for readability as coordinates (x,y)
typedef std::pair<double, double> coords;

////////////// from Behdad's cairotwisted.c (required functions) /////////////
typedef double parametrization_t;  

/* Simple struct to hold a path and its parametrization */
typedef struct {
    cairo_path_t *path;
    parametrization_t *parametrization;
} parametrized_path_t;

// path transforming function pointer
typedef void (*transform_point_func_t) (void *closure, double *x, double *y);
///////////https://github.com/phuang/pango/blob/master/examples/cairotwisted.c


/*
 * A general class to hold methods and fields that are used in many 
 * functions for different purposes.
 */
class MTS_BaseHelper {
    private://----------------------- PRIVATE METHODS --------------------------

        ////////////// from Behdad's cairotwisted.c (required functions) /////////////


        /* Returns Euclidean distance between two points */
        double
            two_points_distance (cairo_path_data_t *a, 
                    cairo_path_data_t *b);

        /* Returns length of a Bezier curve.
         * Seems like computing that analytically is not easy.  The
         * code just flattens the curve using cairo and adds the length
         * of segments.
         */
        double
            curve_length (double x0, double y0,
                    double x1, double y1,
                    double x2, double y2,
                    double x3, double y3);

        /* Compute parametrization info.  That is, for each part of the
         * cairo path, tags it with its length.
         *
         * Free returned value with g_free().
         */
        parametrization_t *
            parametrize_path (cairo_path_t *path);

        /* Project a path using a function.  Each point of the path (including
         * Bezier control points) is passed to the function for transformation.
         */
        void
            transform_path (cairo_path_t *path, transform_point_func_t f, void *closure);

        /* Project a point X,Y onto a parameterized path.  The final point is
         * where you get if you walk on the path forward from the beginning for X
         * units, then stop there and walk another Y units perpendicular to the
         * path at that point.  In more detail:
         *
         * There's three pieces of math involved:
         *
         *   - The parametric form of the Line equation
         *     http://en.wikipedia.org/wiki/Line
         *
         *   - The parametric form of the Cubic Bézier curve equation
         *     http://en.wikipedia.org/wiki/B%C3%A9zier_curve
         *
         *   - The Gradient (aka multi-dimensional derivative) of the above
         *     http://en.wikipedia.org/wiki/Gradient
         *
         * The parametric forms are used to answer the question of "where will I be
         * if I walk a distance of X on this path".  The Gradient is used to answer
         * the question of "where will I be if then I stop, rotate left for 90
         * degrees and walk straight for a distance of Y".
         */
        static void
            point_on_path (parametrized_path_t *param, double *x, double *y);


        /* Projects the current path of cr onto the provided path. */
        void
            map_path_onto (cairo_t *cr, cairo_path_t *path);

        ///////////https://github.com/phuang/pango/blob/master/examples/cairotwisted.c


        /*
         * Takes four x,y coordinate points and returns two control points
         *
         * start - the start point
         * f1 - a point on the curve
         * f2 - another point on the curve
         * end - end point
         * cp1 - the first control point
         * cp2 - the second control points
         */
        void 
            four_point_to_cp(coords start,
                    coords f1,
                    coords f2,
                    coords end,
                    coords *cp1,
                    coords *cp2);

        /*
         * Make an arc that follows the path of points vector
         *
         * cr - cairo context
         * points - a vector of x,y coordinate pairs (precondition: size == 2)
         * radius - the radius of the curvature
         * width - width of the surface
         * height - height of the surface
         * direction - a flag that tells whether the returned points will be along the 
         *             top or bottom of the circle. true : top, false : bottom 
         */
        void 
            points_to_arc_path(cairo_t *cr, std::vector<coords> points, double radius, 
                    double width, double height, bool direction);

        RNG rng_;

        //All parameters
        unordered_map<string, double> params;

    public://----------------------- PUBLIC METHODS --------------------------

        /* 
         * Draws a path shape from points using (semi-)cubic interpolation.  
         * uses coords from points vec to determine where to draw curves to and from.
         * draws curves point by point from vector.
         *
         * cr - cairo context
         * c_min - the min range value for squared variable in the first cubic 
         *        curving equation
         * c_max - the max range value for squared variable in the first cubic 
         *        curving equation
         * d_min - the min range value for cubed variable in the first cubic 
         *        curving equation
         * d_max - the max range value for cubed variable in the first cubic 
         *        curving equation
         * points - a vector of x,y coordinate pairs 
         *          (precondition: must contain at least 2 elements)
         */
        void points_to_path(cairo_t *cr, std::vector<coords> points, double c_min=-2, double c_max=2, double d_min=-2, double d_max=2, double cd_sum_max=10);


        /*
         * Makes a vector of 2 x,y coordinates that follow the arc of the input 
         * radius. For noticable results with minimal distortion, radius 
         * should be greater than (1/2 * width) but less than (5 * width)
         *
         * width - surface width in pixels
         * height - surface height in pixels
         * radius - the curve radius of the arc (precondition: radius >= 1/2 * width)
         * direction - a flag that tells whether the returned points will be along the 
         *             top or bottom of the circle. true : top, false : bottom 
         */
        std::vector<coords> make_points_arc(double width, double height, double radius, bool direction);

        /*
         * Makes and returns a vector of x,y coordinate points for
         * a wave path to be drawn along. Coordinate point variation
         * is determined with rng within certain bounds to prevent
         * distorted results.
         *
         * width - surface width in pixels
         * height - surface height in pixels
         * num_points - the number of points to push onto the vector 
         *              (minimum 3) (range 3-5 for least text distortion)
         * y_var_min_ratio - the minimum fluctuation of the fixing points of the curve in y-direction w.r.t. the height of image
         * y_var_max_ratio - the maximum fluctuation of the fixing points of the curve in y-direction w.r.t. the height of image
         */
        std::vector<coords> make_points_wave(double width, double height, int num_points, double y_var_min_ratio, double y_var_max_ratio);

        /*
         * Iterativly translates each cairo movement stored in path and 
         * data by xtrans, ytrans. 
         *
         * cr - cairo context
         * path - the cairo path to be translated
         * data - the data corresponding to path (initialized in method)
         * xtrans - the translation distance in pixels in the x direction
         * ytrans - the translation distance in pixels in the y direction
         */
        void manual_translate(cairo_t *cr, cairo_path_t *path, cairo_path_data_t *data, 
                double xtrans, double ytrans);

        /*
         * Creates an arc path that allows for text to be drawn along
         * it. For minimal distortion and visible results, radius should 
         * be greater than (1/2 * width) but less than (5 * width)
         *
         * cr - cairo context 
         * path - a non-local cairo_path_t variable for this function to use 
         *        (is assigned in function)
         * line - a non-local PangoLayoutLine variable for this function to use 
         *        (is assigned in function)
         * layout - the PangoLayout used for the desired text
         * x - the horizontal distance in pixels the text is positioned along the path
         * y - the vertical distance in pixels the letters are removed from the path 
         *     (y > 20 can become distorted)
         * radius - the radius of the curvature to be created 
         *          (precondition: radius >= .5*width)
         * width - the width of the surface
         * height - the height of the surface
         * direction - a flag that tells whether the returned points will be along the 
         *             top or bottom of the circle. true : top, false : bottom 
         */
        void create_arc_path (cairo_t *cr, cairo_path_t *path, PangoLayoutLine *line, 
                PangoLayout *layout, double x, double y, double radius, 
                double width, double height, bool direction);


        /*
         * Creates a curved path from points using cubic interpolation, and allows 
         * for text to be drawn along it
         *
         * cr - cairo context 
         * path - a non-local cairo_path_t variable for this function 
         *        to use (is assigned in function)
         * line - a non-local PangoLayoutLine variable for this function 
         *        to use (is assigned in function)
         * layout - the PangoLayout used for the desired text
         * width - the width of the surface
         * height - the height of the surface
         * x - the horizontal distance in pixels the text is positioned along the path
         * y - the vertical distance in pixels the letters are removed from the path 
         *     (y > 20 can cause distortion)
         * num_points - the number of points that form the path. 
         *              This number determines how many curves are generated.
         *              (minimum 3) (range 3-5 for least distortion)
         * c_min - the min range value for squared variable in the first cubic 
         *        curving equation
         * c_max - the max range value for squared variable in the first cubic 
         *        curving equation
         * d_min - the min range value for cubed variable in the first cubic 
         *        curving equation
         * d_max - the max range value for cubed variable in the first cubic 
         * y_var_min_ratio - the minimum fluctuation of the fixing points of the curve in y-direction w.r.t. the height of image
         * y_var_max_ratio - the maximum fluctuation of the fixing points of the curve in y-direction w.r.t. the height of image
         *        curving equation

         */
        void create_curved_path (cairo_t *cr, cairo_path_t *path, PangoLayoutLine *line, 
                PangoLayout *layout, double width, double height, 
                double x, double y, int num_points, double c_min, double c_max, double d_min, 
                double d_max, double y_var_min_ratio, double y_var_max_ratio);

        /*
         * An overload for create_curved_path that allows for the points vector
         * to be set outside the function.   
         *
         * points - vector of x,y coordinate pairs that are used to make the
         *          shape of the path
         * stroke - a flag to tell function whether or not to stroke the line or
         *          simply leave it as a path. (optional parameter, default false)
         * c_min - the min range value for squared variable in the first cubic 
         *        curving equation
         * c_max - the max range value for squared variable in the first cubic 
         *        curving equation
         * d_min - the min range value for cubed variable in the first cubic 
         *        curving equation
         * d_max - the max range value for cubed variable in the first cubic 
         *        curving equation

         */
        void create_curved_path (cairo_t *cr, cairo_path_t *path, PangoLayoutLine *line,
                PangoLayout *layout, double width, double height,
                double x, double y, std::vector<coords> points,
                double c_min, double c_max, double d_min, double d_max,
                bool stroke=false);


        //Another RNG for beta, gamma, normal distributions
        mt19937 rng2_;

        //Constructor
        MTS_BaseHelper(unordered_map<string, double> params);

        //Destructor
        ~MTS_BaseHelper();

        /* Returns the value for key*/
        double getParam(string key);

        /*
         * Sets all the user configurable parameters from a text file
         *
         * params - a map from the name of the value to the value
         */
        void setParams(unordered_map<string, double> params);


        /*
         * Returns true or false based on a randomly generated probability under
         * the input value probability
         *
         * probability - the probability to be calculated under (0 - 1.0)
         */
        bool rndProbUnder(double probability);

        /*
         * A wrapper for the openCV random number generator. Returns a
         * positive random number.
         */
        unsigned int rng();

        /*
         * A setter function for the seed of the random number generator
         *
         * rndState - the number to seed the rng with
         */
        void setSeed(uint64 rndState);

        /*
         * Makes a mask that has holes in it to project over background or text
         *
         * surface - the surface to draw holes onto
         * num_min - minimum number of holes
         * num_max - maximum number of holes
         * size_min - mininum size of hole wrt height
         * size_max - maxinum size of hole wrt height
         * diminish_rate - diminish_rate of the edge of the hole
         * transparent - whether the holes will be transparent or not
         * color_min - if not trans, then the min color of the holes
         * color_max - if not trans, then the max color of the holes
         */
        void addSpots(cairo_surface_t *surface, int num_min, int num_max, double size_min, double size_max, double diminish_rate, bool transparent, int color_min=0, int color_max=0);

};

#endif
