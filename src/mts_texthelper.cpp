#include <math.h>
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>

#include <pango/pangocairo.h>

#include "mts_texthelper.hpp"

using namespace std;
using boost::random::beta_distribution;
using boost::random::variate_generator;

double MTS_TextHelper::getParam(string key) {
    double val = helper->getParam(key);
    return val;
}

MTS_TextHelper::MTS_TextHelper(shared_ptr<MTS_BaseHelper> h)
    //:helper(make_shared<MTS_BaseHelper>(*h)),  // initialize fields
    :helper(&(*h)),  // initialize fields
    spacing_dist(h->getParam("spacing_alpha"),h->getParam("spacing_beta")),
    spacing_gen(h->rng2_, spacing_dist),
    stretch_dist(h->getParam("stretch_alpha"),h->getParam("stretch_beta")),
    stretch_gen(h->rng2_, stretch_dist),
    digit_len_dist(h->getParam("digit_len_alpha"),h->getParam("digit_len_beta")),
    digit_len_gen(h->rng2_, digit_len_dist)
{}

MTS_TextHelper::~MTS_TextHelper(){
    cout << "text helper destructed" << endl;
}

// SEE mts_texthelper.hpp FOR ALL DOCUMENTATION

void 
MTS_TextHelper::generateFont(char *font, int fontsize){

    cout << "in generate font" << endl;
    // get font probabilities from user configured parameters
    int font_prob = helper->rng() % 10000;
    double blockyProb=getParam("font_blocky");
    double normalProb=getParam("font_normal");
    double probs[3]={blockyProb, normalProb + blockyProb, 1};
    cout << "got probs" << endl;

    const char *font_name;
    // randomly select a font style 
    for (int i = 0; i < 3; i++) {
        if(font_prob < 10000 * probs[i]){
            cout << "in if " << endl;
            int listsize = fonts_[i]->size();
            cout << "got size " << endl;
            if (listsize <= 0) {
                cerr << "font list size should be positive integer!" << endl;
                exit(0);
            }
            //CV_Assert(listsize);
            font_name = fonts_[i]->at(helper->rng()%listsize).c_str();
            strcpy(font,font_name);
            break;
        }
    }

    //set probability of being Italic
    if (helper->rndProbUnder(getParam("italic_prob"))) {
        // add italic information to the font string
        strcat(font," Italic");
    }

    // add font size information to the font string
    strcat(font," ");
    std::ostringstream stm;
    stm << fontsize;
    strcat(font,stm.str().c_str());
    cout << font << endl;
}

void
MTS_TextHelper::generateFeatures(double &rotated_angle, bool &curved, double &spacing_deg, double &spacing, double &stretch_deg, int &x_pad, int &y_pad, double &scale, PangoFontDescription *&desc, int height) {

    // if determined by probability of rotation, set rotated angle
    if (helper->rndProbUnder(getParam("rotate_prob"))){
        int min_deg = getParam("rotate_degree_min");
        int max_deg = getParam("rotate_degree_max");
        int degree = helper->rng() % (max_deg-min_deg+1) + min_deg;
        cout << "degree " << degree << endl;
        rotated_angle=((double)degree / 180) * M_PI;
    } else {
        rotated_angle= 0;
    }

    double curvingProb=getParam("curve_prob");

    // set probability of being curved
    if(helper->rndProbUnder(curvingProb)){
        curved = true;
    }

    spacing_deg = round((20*spacing_gen()-1)*100)/100;
    cout << "spacing deg " << spacing_deg << endl;

    stretch_deg = round((3*stretch_gen()+0.5)*100)/100;
    cout << "stretch deg " << stretch_deg << endl;

    double fontsize = (double)height;
    spacing = fontsize / 20 * spacing_deg;

    double pad_max = getParam("pad_max");
    double pad_min = getParam("pad_min");
    int maxpad=(int)(height*pad_max);
    int minpad=(int)(height*pad_min);
    x_pad = helper->rng() % (maxpad-minpad+1) + minpad;
    y_pad = helper->rng() % (maxpad-minpad+1) + minpad;
    cout << "pad " << x_pad << " " << y_pad << endl;

    int scale_max = (int)(100*getParam("scale_max"));
    int scale_min = (int)(100*getParam("scale_min"));
    scale = (helper->rng()%(scale_max-scale_min+1)+scale_min)/100.0;
    cout << "scale " << scale << endl;

    cout << "generate font" << endl;
    char font[50];
    generateFont(font,(int)fontsize);
    cout << font << endl;
    //cout << caption << endl;

    //set font destcription
    desc = pango_font_description_from_string(font);

    //Weight
    double light_prob = getParam("weight_light_prob");
    double normal_prob = getParam("weight_normal_prob");
    int weight_prob = helper->rng()%10000;

    if(weight_prob < 10000*light_prob){
        pango_font_description_set_weight(desc, PANGO_WEIGHT_LIGHT);
    } else if(weight_prob < 10000*(light_prob+normal_prob)){
        pango_font_description_set_weight(desc, PANGO_WEIGHT_NORMAL);
    } else {
        pango_font_description_set_weight(desc, PANGO_WEIGHT_BOLD);
    }

}

void
MTS_TextHelper::getTextExtents(PangoLayout *layout, PangoFontDescription *desc, int &x, int &y, int &w, int &h, int &size) {
    PangoRectangle *ink_rect = new PangoRectangle;
    PangoRectangle *logical_rect = new PangoRectangle;
    pango_layout_get_extents(layout, ink_rect, logical_rect);

    x=ink_rect->x/1024;
    y=ink_rect->y/1024;
    w=ink_rect->width/1024;
    h=ink_rect->height/1024;

    size = pango_font_description_get_size(desc);

    free(logical_rect);
    free(ink_rect);
}

void get_normal_vector(cairo_path_t *path, double x_exp, double &x, double &y, double &rad) {

    cairo_path_data_t *data;
    bool pre = false;
    cairo_path_data_t *data2;

    int i;
    x = 0;
    bool stop = false;

    for (i = 0; i < path->num_data; i += path->data[i].header.length) {
        data = &path->data[i];
        switch (data->header.type) {
            case CAIRO_PATH_MOVE_TO:
                x = data[1].point.x;
                if (x >= x_exp) {
                    stop = true;
                }
                break;
            case CAIRO_PATH_LINE_TO:
                x = data[1].point.x;
                if (x >= x_exp) {
                    stop = true;
                }
                break;
            case CAIRO_PATH_CURVE_TO:
                cerr << "unexpected curve to" << endl;
                exit(1);
                break;
            case CAIRO_PATH_CLOSE_PATH:
                cerr << "unexpected close path" << endl;
                exit(1);
                break;
        }
        if (stop) {
            break;
        }
    }

    if (i == path->num_data - 2) {
        pre = true;
        data2 = &path->data[i-2];
    } else {
        data2 = &path->data[i+2];
    }

    switch (data->header.type) {
        case CAIRO_PATH_MOVE_TO:
            {
                if (pre) {
                    cerr << "unexpected move to" << endl;
                    exit(1);
                }
                x = data[1].point.x;
                y = data[1].point.y;
                double x_dis = data2[1].point.x - data[1].point.x;
                double y_dis = data2[1].point.y - data[1].point.y;
                if (x_dis == 0) {
                    if (y_dis > 0) {
                        rad = M_PI / 2;
                    } else {
                        rad = - M_PI / 2;
                    }
                } else {
                    rad = atan(y_dis/x_dis);
                }
            }
            break;
        case CAIRO_PATH_LINE_TO:
            {
                double x_dis,y_dis;
                if (pre) {
                    x = data2[1].point.x;
                    y = data2[1].point.y;
                    x_dis = data[1].point.x - data2[1].point.x;
                    y_dis = data[1].point.y - data2[1].point.y;
                } else {
                    x = data[1].point.x;
                    y = data[1].point.y;
                    x_dis = data2[1].point.x - data[1].point.x;
                    y_dis = data2[1].point.y - data[1].point.y;
                }
                if (x_dis == 0) {
                    if (y_dis > 0) {
                        rad = M_PI / 2;
                    } else {
                        rad = - M_PI / 2;
                    }
                } else {
                    rad = atan(y_dis/x_dis);
                }
            }
            break;
        case CAIRO_PATH_CURVE_TO:
            cerr << "unexpected curve to" << endl;
            exit(1);
            break;
        case CAIRO_PATH_CLOSE_PATH:
            cerr << "unexpected close path" << endl;
            exit(1);
            break;
    }
}

void
MTS_TextHelper::create_curved_text(cairo_t *cr, PangoLayout *layout,
        double width, double height, int num_points, double c_min, 
        double c_max, double d_min, double d_max, double cd_sum_max, double stretch_deg,
        double y_var_min_ratio, double y_var_max_ratio) {

    if (num_points < 2) {
        cerr << "number of points is less than 2 in create_curved_text() !" << endl;
        exit(1);
    }   
    cout << "curved text width " << width << endl;

    //set the points for the path
    std::vector<coords> points= helper->make_points_wave(width, height, num_points, y_var_min_ratio, y_var_max_ratio);

    helper->points_to_path(cr, points, c_min,c_max,d_min,d_max, cd_sum_max); //draw path shape

    cout << "finished getting curve path!" << endl;

    // Decrease tolerance, since the text going to be magnified 
    cairo_set_tolerance(cr, 0.01);

    cairo_path_t *path = cairo_copy_path_flat(cr);
    cairo_new_path(cr);

    // Get text
    char caption[100];
    strcpy(caption, pango_layout_get_text(layout));
    int caption_len = strlen(caption);

    int path_point_num = (path->num_data)/2;
    cout << "num of point flat: " << path_point_num << endl;
    double spacing = width / (caption_len-1);

    // Loop through the text
    int i = 0;
    char cur = caption[i];

    cairo_path_t *path_so_far = NULL;

    while (cur != '\0') {
        cairo_save(cr);
        char tmp[2] = {cur, '\0'};
        pango_layout_set_text(layout, tmp, -1);
        pango_cairo_layout_path(cr, layout);
        double rad, x, y;
        get_normal_vector(path, i*spacing, x, y, rad);
        double x1,y1,x2,y2;
        cairo_path_extents(cr, &x1, &y1, &x2, &y2);
        cairo_path_t *tmp_path = cairo_copy_path(cr);
        cairo_new_path(cr);

        cairo_translate(cr, i * spacing, y);
        //cairo_translate(cr, x, y - height/2);
        cairo_rotate(cr, rad);
        cairo_scale(cr, stretch_deg, 1);
        //cairo_translate(cr, - i * spacing, -y);
        cairo_translate(cr, -(x1+(x2-x1)/2), -(y1+(y2-y1)/2));
        cairo_append_path(cr, tmp_path);
        //cairo_fill_preserve(cr);
        cairo_restore(cr);
        if (path_so_far != NULL) {
            cairo_append_path(cr, path_so_far);
        }
        path_so_far = cairo_copy_path(cr);
        cairo_new_path(cr);
        cur = caption[++i];
    }
    cairo_append_path(cr, path_so_far);
    //cairo_fill(cr);

    //clean up
    cairo_path_destroy(path);
}

void
MTS_TextHelper::create_curved_text_deformed(cairo_t *cr,
                PangoLayout *layout, double width, double height,
                int num_points, double c_min, double c_max, double d_min,
                double d_max, double cd_sum_max, double stretch_deg,
                double y_var_min_ratio, double y_var_max_ratio) {

    if (num_points < 2) {
        cerr << "number of points is less than 2 in create_curved_path() !" << endl;
        exit(1);
    }

    cairo_save(cr);

    //set the points for the path
    std::vector<coords> points= helper->make_points_wave(width, height, num_points, y_var_min_ratio, y_var_max_ratio);

    helper->points_to_path(cr, points, c_min,c_max,d_min,d_max, cd_sum_max); //draw path shape

    // Decrease tolerance, since the text going to be magnified
    cairo_set_tolerance(cr, 0.01);

    cairo_path_t *path = cairo_copy_path_flat(cr);
    cairo_new_path(cr);

    cairo_scale(cr, stretch_deg, 1);
    pango_cairo_layout_path(cr, layout);

    helper->map_path_onto (cr, path);

    //clean up
    cairo_path_destroy (path);

    cairo_restore(cr);

}


void 
MTS_TextHelper::generateTextPatch(cairo_surface_t *&text_surface, 
        string caption,int height,int &width, int text_color, bool distract){

    int len = caption.length();

    // cairo stuff
    cairo_surface_t *surface;
    cairo_t *cr;

    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 40*height, height);
    cr = cairo_create (surface);

    cairo_set_source_rgb(cr, text_color/255.0,text_color/255.0,text_color/255.0);
    PangoLayout *layout;
    PangoFontDescription *desc;

    layout = pango_cairo_create_layout (cr);

    // text attributes
    double rotated_angle;
    bool curved;
    double spacing_deg, spacing, stretch_deg;
    int x_pad, y_pad;
    double scale;

    generateFeatures(rotated_angle, curved, spacing_deg, spacing, stretch_deg, x_pad, y_pad, scale, desc, height);

    int point_num_max = len / (int)(getParam("curve_min_char_num_per_point"));
    if (point_num_max < 2) {
        curved = false;
    }

    // applying the attributes
    pango_layout_set_font_description (layout, desc);

    int spacing_1024 = (int)(1024*spacing);

    std::ostringstream stm;
    stm << spacing_1024;
    // set the markup string and put into pango layout
    string mark = "<span letter_spacing='"+stm.str()+"'>"+caption+"</span>";
    cout << "mark " << mark << endl;

    pango_layout_set_markup(layout, mark.c_str(), -1);


    // get ink extents and adjust font size
    int ink_x, ink_y, ink_w, ink_h, size; 

    getTextExtents(layout, desc, ink_x, ink_y, ink_w, ink_h, size);

    size = (int)((double)size/ink_h*height);
    cout << "size " << size << endl;
    pango_font_description_set_size(desc, size);
    pango_layout_set_font_description (layout, desc);

    getTextExtents(layout, desc, ink_x, ink_y, ink_w, ink_h, size);

    ink_w=stretch_deg*(ink_w);

    int patch_width = (int)ink_w;

    if (rotated_angle!=0) {
        cout << "rotated angle" << rotated_angle << endl;
        cairo_rotate(cr, rotated_angle);

        double sine = abs(sin(rotated_angle));
        double cosine = abs(cos(rotated_angle));

        double ratio = ink_h/(double)ink_w;
        double text_width, text_height;

        text_width=(height/(cosine*ratio+sine));
        text_height = (ratio*text_width);
        patch_width = (int)ceil(cosine*text_width+sine*text_height);

        // adjust text attributes according to rotate angle
        size = pango_font_description_get_size(desc);
        size = (int)((double)size/ink_h*text_height);
        cout << "rotate size " << size << endl;
        pango_font_description_set_size(desc, size);
        pango_layout_set_font_description (layout, desc);

        spacing_1024 = (int)floor((double)spacing_1024/ink_h*text_height);

        std::ostringstream stm;
        stm << spacing_1024;
        string mark = "<span letter_spacing='"+stm.str()+"'>"+caption+"</span>";
        cout << "mark " << mark << endl;

        pango_layout_set_markup(layout, mark.c_str(), -1);

        // adjust text position
        double x_off=0, y_off=0;
        if (rotated_angle<0) {
            x_off=-sine*sine*text_width;
            y_off=cosine*sine*text_width;
        } else {
            x_off=cosine*sine*text_height;
            y_off=-sine*sine*text_height;
        }   
        cairo_translate (cr, x_off, y_off);

        cairo_scale(cr, stretch_deg, 1);

        double height_ratio=text_height/ink_h;
        y_off=(ink_y*height_ratio);
        x_off=(ink_x*height_ratio);
        cairo_translate (cr, -x_off, -y_off);

        pango_cairo_show_layout (cr, layout);

    } else if (curved 
            //&& patch_width > getParam("curve_min_wid_hei_ratio")*height && 
            && spacing_deg >= getParam("curve_min_spacing")
            ) {

        int num_min = (int)(getParam("curve_num_points_min"));
        int num_max = (int)(getParam("curve_num_points_max"));
        int num_points = helper->rng()%(num_max-num_min+1)+num_min;
        num_points = min(num_points, point_num_max);

        cout << "num curve points " << num_points << endl;

        double c_min = getParam("curve_c_min");
        double c_max = getParam("curve_c_max");
        double d_min = getParam("curve_d_min");
        double d_max = getParam("curve_d_max");

        double cd_sum_max = getParam("curve_cd_sum_max");
        double cd_increase_rate = getParam("curve_cd_increase_rate");

        c_min -= cd_increase_rate*(num_points - 2);
        c_max += cd_increase_rate*(num_points - 2);
        d_min -= cd_increase_rate*(num_points - 2);
        d_max += cd_increase_rate*(num_points - 2);
        
        double y_var_min = getParam("curve_y_variance_min");
        double y_var_max = getParam("curve_y_variance_max");

        double deform = getParam("curve_is_deformed_prob");

        if (helper->rndProbUnder(deform)) {
            create_curved_text_deformed(cr, layout, (double)patch_width, (double)height,
                num_points, c_min, c_max, d_min, d_max, 
                cd_sum_max, stretch_deg, y_var_min, y_var_max);
        } else {
            create_curved_text(cr,layout,(double)patch_width,
                    (double) height,num_points,c_min,c_max,d_min,d_max,
                    cd_sum_max,stretch_deg, y_var_min, y_var_max);
        }

        // get extents and adjust the position
        double x1,x2,y1,y2;
        cairo_path_extents(cr,&x1,&y1,&x2,&y2);

        cairo_path_t *path_n=cairo_copy_path(cr);
        cairo_new_path(cr);
        cairo_translate(cr, -x1, -y1);
        cairo_append_path(cr, path_n);
        cairo_translate(cr, x1, y1);
        //helper->manual_translate(cr, path_n, path_data_n, -x1, -y1);

        cairo_path_extents(cr,&x1,&y1,&x2,&y2);

        // copy the path out
        path_n=cairo_copy_path(cr);
        cairo_new_path(cr);

        // create a new surface that tightly bounds the ink
        cairo_surface_t *surface_c;
        cairo_t *cr_c;

        surface_c = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, (int)ceil(x2-x1), (int)ceil(y2-y1));
        cr_c = cairo_create(surface_c);
        cairo_new_path(cr_c);
        cairo_append_path(cr_c,path_n);
        cairo_fill(cr_c);

        // copy the ink back and adjust the size
        double height_ratio = height/(y2-y1);
        cairo_scale(cr,height_ratio,height_ratio);
        //patch_width=(int)(ceil((x2-x1)*height_ratio)*stretch_deg);
        patch_width=(int)(ceil((x2-x1)*height_ratio));
        cairo_set_source_surface(cr, surface_c, 0, 0);
        cairo_rectangle(cr, 0, 0, x2-x1, y2-y1);
        cairo_fill(cr);
        cairo_path_destroy(path_n);
        cairo_destroy (cr_c);
        cairo_surface_destroy (surface_c);
        cout << "real curved text width " << patch_width << endl;
    } else {
        cairo_scale(cr, stretch_deg, 1);
        cairo_translate (cr, -ink_x, -ink_y);
        pango_cairo_show_layout (cr, layout);
    }

    cairo_identity_matrix(cr);

    // free layout
    g_object_unref(layout);
    pango_font_description_free (desc);
    cairo_destroy (cr);

    // create a new surface that has the correct width
    cairo_surface_t *surface_n;
    cairo_t *cr_n;

    surface_n = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, patch_width, height);
    cr_n = cairo_create (surface_n);

    // apply arbitrary padding and scaling
    cairo_translate (cr_n, x_pad, y_pad);

    cairo_translate (cr_n, patch_width/2, height/2);
    cairo_scale(cr_n, scale, scale);
    cairo_translate (cr_n, -patch_width/2, -height/2);

    // copy text onto new surface
    cairo_set_source_surface(cr_n, surface, 0, 0);
    cairo_rectangle(cr_n, 0, 0, patch_width, height);
    cairo_fill(cr_n);
    cairo_surface_destroy (surface);

    // draw the remaining stuff on new surface
    cairo_set_source_rgb(cr_n, text_color/255.0,text_color/255.0,text_color/255.0);
    if (distract) {
        int num_min = (int)getParam("distract_num_min");
        int num_max = (int)getParam("distract_num_max");
        int dis_num = helper->rng()%(num_max-num_min+1)+num_min;

        int shrink_min=(int)(100*getParam("distract_size_min"));
        int shrink_max=(int)(100*getParam("distract_size_max"));
        double shrink = (helper->rng()%(shrink_max-shrink_min+1)+shrink_min)/100.0;
        cout << "shrink " << shrink << endl;

        for (int i=0;i<dis_num;i++) {
            char distract_font[50];
            generateFont(distract_font,(int)(shrink*height));
            distractText(cr_n, patch_width, height, distract_font);
        }
    }

    cairo_destroy (cr_n);

    cout << "add spots" << endl;
    if(helper->rndProbUnder(getParam("missing_prob"))){
        int num_min=(int)getParam("missing_num_min");
        int num_max=(int)getParam("missing_num_max");
        double size_min=getParam("missing_size_min");
        double size_max=getParam("missing_size_max");
        double dim_rate=getParam("missing_diminish_rate");
        helper->addSpots(surface_n,num_min,num_max,size_min,size_max,dim_rate,true);
    }

    //pass back values
    text_surface=surface_n;
    width=patch_width;
}

void 
MTS_TextHelper::generateTextSample (string &caption, cairo_surface_t *&text_surface, int height, 
        int &width, int text_color, bool distract){

    cout << "text color " << text_color << endl;
    if (helper->rndProbUnder(getParam("digit_prob"))) {
        caption = "";
        int digit_len = int(ceil(1/digit_len_gen()));
        int max_len = int(getParam("digit_len_max"));
        if (digit_len>max_len) digit_len=max_len;
        for (int i = 0; i < digit_len; i++) {
            caption+=randomDigit();
        }
    } else {
        if(sampleCaptions_->size() != 0){
            // if there are sample captions, select one randomly and generate text
            caption = sampleCaptions_->at(helper->rng() % sampleCaptions_->size());
        } else {
            caption = "MapTextSynthesizer";
        }
    }
    cout << "generating text patch" << endl;
    generateTextPatch(text_surface,caption,height,width,text_color,distract);
}


char
MTS_TextHelper::randomChar() {
    // string containing available characters to select
    string total("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.,'");
    return total.at(helper->rng()%(total.length()));
}

char
MTS_TextHelper::randomDigit() {
    // string containing available digits
    string total("0123456789");
    return total.at(helper->rng()%(total.length()));
}

void
MTS_TextHelper::distractText (cairo_t *cr, int width, int height, char *font) {

    // generate text
    int len_min = (int)getParam("distract_len_min");
    int len_max = (int)getParam("distract_len_max");
    int len = helper->rng() % (len_max-len_min + 1) + len_min;
    char text[len+1];

    // generate a random string of characters
    for (int i = 0; i < len; i++) {
        text[i] = randomChar();
    }
    text[len] = '\0'; //null terminate the cstring

    // use pango to turn cstring into vector text
    PangoLayout *layout;
    PangoFontDescription *desc;
    layout = pango_cairo_create_layout (cr);

    desc = pango_font_description_from_string(font);
    pango_layout_set_font_description (layout, desc);
    pango_layout_set_text(layout, text, -1);

    // find text bounding rectangle
    PangoRectangle *text_rect = new PangoRectangle;
    PangoRectangle *logical_rect = new PangoRectangle;
    pango_layout_get_extents(layout, text_rect, logical_rect);

    // get the text dimensions from its bounding rectangle
    int text_width = text_rect->width/1024;
    int text_height = text_rect->height/1024;

    // translate to arbitrary point on the canvas
    int x = helper->rng()%width;
    int y = helper->rng()%height;
    cairo_translate (cr, (double)x, (double)y);

    // randomly choose and set rotation angle
    int deg = helper->rng() % 360;
    double rad = deg/180.0 * M_PI;

    cairo_translate (cr, text_width/2.0, text_height/2.0);
    cairo_rotate(cr, rad);
    cairo_translate (cr, -text_width/2.0, -text_height/2.0);

    // put text on cairo context
    pango_cairo_show_layout (cr, layout);

    // clean up 
    cairo_identity_matrix(cr);
    g_object_unref(layout);
    pango_font_description_free (desc);
    free(logical_rect);
    free(text_rect);
}


void
//MTS_TextHelper::setFonts(std::shared_ptr<std::vector<string> > *data) {
MTS_TextHelper::setFonts(std::vector<string> **data) {
    fonts_ = data;
}


void
//MTS_TextHelper::setSampleCaptions(std::shared_ptr<std::vector<string> > data) {
MTS_TextHelper::setSampleCaptions(std::vector<string> *data) {
    sampleCaptions_ = data;
}
