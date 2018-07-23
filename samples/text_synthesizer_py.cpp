#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/numpy/ndarray.hpp>
#include <mtsynth/map_text_synthesizer.hpp>
#include <vector>
#include <string>

using namespace boost::python;
typedef std::vector<cv::String> py_vec;

class mts_wrapper {
    public:

        cv::Ptr<MapTextSynthesizer> mts;

        mts_wrapper() {
            this->mts = MapTextSynthesizer::create();
        }

        static std::vector<cv::String> list_to_vec(boost::python::list words) {
            std::vector<cv::String> vec;
            for (int i=0; i<len(words); i++) {
                vec.push_back(cv::String(boost::python::extract<string>(words[i])));
            }
            return vec;
        }

       static boost::python::list vec_to_list(std::vector<unsigned char> vec) {
            boost::python::list list;
            for (int i=0; i<vec.size(); i++) {
	      list.append(vec[i]);
            }
            return list;
        }

        void setSampleCaptions(boost::python::list words) {
            std::vector<cv::String> vec_words = list_to_vec(words);
            mts->setSampleCaptions(vec_words);
        }

        void setBlockyFonts(boost::python::list fonts) {
            std::vector<cv::String> vec_fonts = list_to_vec(fonts);
            mts->setBlockyFonts(vec_fonts);
        }

        void setRegularFonts(boost::python::list fonts) {
            std::vector<cv::String> vec_fonts = list_to_vec(fonts);
            mts->setRegularFonts(vec_fonts);
        }

        void setCursiveFonts(boost::python::list fonts) {
            std::vector<cv::String> vec_fonts = list_to_vec(fonts);
            mts->setCursiveFonts(vec_fonts);
        }

        object generateSample() {

	  /*
	    Py_Initialize();
	    boost::python::numpy::initialize();

 unsigned char data[] = {1,2,3,4,5};

            boost::python::numpy::dtype dt = boost::python::numpy::dtype::get_builtin<unsigned char>();

	    boost::python::tuple shape = boost::python::make_tuple(height*width);
	    boost::python::tuple stride = boost::python::make_tuple(sizeof(unsigned char));
	    boost::python::object own = boost::python::object();

	    boost::python::numpy::ndarray image_arr = boost::python::numpy::from_data(data, dt, shape, stride, own);
	  */

            String caption;
            Mat sample;
            mts->generateSample(caption, sample);
            

            int height = sample.rows;
            int width = sample.cols;

	      //object py_image = (object)(const char*)sample.data;
	    
	    std::vector<unsigned char> data_vec(sample.data, 
	    	sample.data + height * width);
            boost::python::list data_list = vec_to_list(data_vec);
	    auto image_data = boost::python::make_tuple(height, width, data_list);
	    //boost::python::numpy::ndarray np_sample(sample);
            string cap = caption;
            boost::python::object py_caption(cap);
	    //boost::python::object py_image(data);
            //auto image_data = boost::python::make_tuple(height, width, image_arr);
            //Mat py_image(np_sample);

            return boost::python::make_tuple(py_caption, image_data);
        }
};

BOOST_PYTHON_MODULE(mtsynth) {

    //def("create", &MapTextSynthesizer::create);
    //def("generate_sample", generate_sample);
    class_<mts_wrapper>("MapTextSynthesizer", init<>())
        .def("setSampleCaptions", &mts_wrapper::setSampleCaptions)
        .def("setBlockyFonts",    &mts_wrapper::setBlockyFonts)
        .def("setRegularFonts",   &mts_wrapper::setRegularFonts)
        .def("setCursiveFonts",   &mts_wrapper::setCursiveFonts)
        .def("generateSample",    &mts_wrapper::generateSample)
        ;
    /*
       class_<MapTextSynthesizer, shared_ptr<MapTextSynthesizer>, boost::noncopyable>("MapTextSynthesizer", no_init)
       .def("create",            &MapTextSynthesizer::create)
       .staticmethod("create")
       .def("setSampleCaptions", &MapTextSynthesizer::setSampleCaptions)
       .def("setBlockyFonts",    &MapTextSynthesizer::setBlockyFonts)
       .def("setRegularFonts",   &MapTextSynthesizer::setRegularFonts)
       .def("setCursiveFonts",   &MapTextSynthesizer::setCursiveFonts)
       .def("generateSample",    &MapTextSynthesizer::generateSample)
       ;
       class_<std::vector<cv::String> >("std::vector<cv::String>")
       .def(vector_indexing_suite<std::vector<cv::String> >() );
     */
}
