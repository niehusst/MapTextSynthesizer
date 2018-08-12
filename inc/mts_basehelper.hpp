#ifndef MTS_BASE_HELPER_HPP
#define MTS_BASE_HELPER_HPP

#include <unordered_map>
#include <memory>
#include <vector>

#include <boost/random.hpp>

#include <pango/pangocairo.h>
#include <opencv2/core/core.hpp> // cv::RNG

#include "mts_config.hpp"

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
            transform_path (cairo_path_t *path, transform_point_func_t f,
                            void *closure);

        /* Project a point X,Y onto a parameterized path.  The final point is
         * where you get if you walk on the path forward from the beginning for
         * X units, then stop there and walk another Y units perpendicular to 
         * the path at that point.  In more detail:
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
         * The parametric forms are used to answer the question of "where will I
         * be if I walk a distance of X on this path".  The Gradient is used to
         * answer the question of "where will I be if then I stop, rotate left 
         * for 90 degrees and walk straight for a distance of Y".
         */
        static void
            point_on_path (parametrized_path_t *param, double *x, double *y);

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

        //the random number generator
        cv::RNG rng_;

public://----------------------- PUBLIC METHODS --------------------------

        MTSConfig* config;

        /* Projects the current path of cr onto the provided path. */
        /* from https://github.com/phuang/pango/blob/master/examples/cairotwisted.c */
        void
            map_path_onto (cairo_t *cr, cairo_path_t *path);
  
        /* 
         * Draws a path shape from points using (semi-)cubic interpolation.  
         * uses coords from points vec to determine where to draw curves to and
         * from. Draws curves point by point from vector.
         *
         * cr - cairo context
         * points - a vector of x,y coordinate pairs 
         *          (precondition: must contain at least 2 elements)
         * c_min - the min range value for squared variable in the first cubic 
         *        curving equation (optional)
         * c_max - the max range value for squared variable in the first cubic 
         *        curving equation (optional)
         * d_min - the min range value for cubed variable in the first cubic 
         *        curving equation (optional)
         * d_max - the max range value for cubed variable in the first cubic 
         *        curving equation (optional)
         * text - Boolean flag say whether this function is being called to 
         *        draw a path for text or a line. (optional)
         */
        void points_to_path(cairo_t *cr, std::vector<coords> points,
                            double c_min=-2, double c_max=2, double d_min=-2,
                            double d_max=2, bool text = false);


        /*
         * Makes and returns a vector of x,y coordinate points for
         * a wave path to be drawn along. Coordinate point variation
         * is determined with rng within certain bounds to prevent
         * distorted results.
         *
         * length - the start to end straight line length of the curve
         * height - surface height in pixels
         * num_points - the number of points to push onto the vector 
         *              (minimum 3) (range 3-5 for least text distortion)
         * y_var_min - the minimum fluctuation of the fixing points of the 
         *             curve in y-direction w.r.t. the height of image
         * y_var_max - the maximum fluctuation of the fixing points of the 
         *             curve in y-direction w.r.t. the height of image
         */
        std::vector<coords> make_points_wave(double length, double height,
                           int num_points, double y_var_min, double y_var_max);


        //Another RNG for beta, gamma, and normal distributions
        mt19937 rng2_;

        //Constructors
        MTS_BaseHelper(std::shared_ptr<MTSConfig> c);

        //Destructor
        ~MTS_BaseHelper() {}

        /*
         * Returns true or false based on a randomly generated probability under
         * the input value probability
         *
         * probability - the probability to be calculated under (0 - 1.0)
         */
        bool rndProbUnder(double probability);

        /*
         * Returns a random double between min and max (inclusive)
         */
        double rndBetween(double min, double max);
        int rndBetween(int min, int max);

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

        //strip the spaces in the front and end of the string
        static std::string
            strip(std::string str);

        /*
         * A Helper method to easily read lines from a file
         *
         * filename - the path to the file.
         */
        std::vector<std::string>
            readLines(std::string filename);

        // tokenize str according to delim
        std::vector<std::string>
            tokenize(std::string str, const char *delim);

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
        void addSpots(cairo_surface_t *surface, int num_min, int num_max,
                      double size_min, double size_max, double diminish_rate,
                      bool transparent, int color_min=0, int color_max=0);

};

#endif
