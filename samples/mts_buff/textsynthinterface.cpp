#include <boost/lockfree/queue.hpp>
#include <thread>
#include <string>
#include <vector>
#include <fstream>
#include <mtsynth/map_text_synthesizer.hpp>
#include <stdio.h>
#include <X11/Xlib.h>


#define BUFFER_SIZE 50
#define NUM_PRODUCERS 1

int g_keep_producing = 1;

// Contains all of the raw data of a sample
typedef struct sample {
  unsigned char* img_data;
  size_t height;
  size_t width;
  char* caption;
} sample_t;

// For ctypes visibility
extern "C" {
  unsigned char* get_img_data(void* ptr);
  size_t get_height(void* ptr);
  size_t get_width(void* ptr);
  char* get_caption(void* ptr);
  void mts_init(void);
  void* get_sample(void);
  void free_sample(void* ptr);
  void mts_cleanup(void);
}

void free_sample(void* ptr) {
  sample_t* s = (sample_t*)ptr;
  free(s->img_data);
  free(s->caption);
  free(s);
}

unsigned char* get_img_data(void* ptr) {
  return ((sample_t*)ptr)->img_data;
}

size_t get_height(void* ptr) {
  return ((sample_t*)ptr)->height;
}
size_t get_width(void* ptr) {
  return ((sample_t*)ptr)->width;
}
char* get_caption(void* ptr) {
  return ((sample_t*)ptr)->caption;
}

//Queue to manage concurrent producing/consuming
//1 consumer, NUM_PRODUCERS producers
boost::lockfree::queue<sample_t*> g_data_pool(BUFFER_SIZE);
std::vector<std::thread> g_producer_threads;

void read_words(string path, vector<String> &caps){
  ifstream infile(path);
  string line;
  while (std::getline(infile, line))
    {   
      caps.push_back(String(line));
    }   
}

void prepare_synthesis(cv::Ptr<MapTextSynthesizer> s) {
  vector<String> caps;

  read_words("../IA/Civil.txt",caps);

  vector<String> blocky;
  blocky.push_back("MathJax_Fraktur");
  blocky.push_back("eufm10");

  vector<String> regular;
  regular.push_back("cmmi10");
  regular.push_back("Sans");
  regular.push_back("Serif");

  vector<String> cursive;
  cursive.push_back("URW Chancery L");

  s->setSampleCaptions(caps);
  s->setBlockyFonts(blocky);
  s->setRegularFonts(regular);
  s->setCursiveFonts(cursive);
}

/* data synthesis thread. puts data into g_data_pool */
void synthesize_data(cv::Ptr<MapTextSynthesizer> synthesizer) {
  String label;
  Mat image;

  while(g_keep_producing) {
    // Fill in label, image
    synthesizer->generateSample(label, image);
  
    // Stick the necessary data into sample_t struct
    sample_t* spl = (sample_t*)malloc(sizeof(sample_t));

    size_t num_elements = image.rows * image.cols;
    size_t buff_size = num_elements * sizeof(unsigned char); //sizeof(image.data[0]);

    //allocate enough space for img_data
    if(!(spl->img_data = (unsigned char*)malloc(buff_size))) {
      perror("failed to allocate image data!\n");
    }
    //copy into image data 
    memcpy(spl->img_data, image.data, buff_size);
    spl->height = image.rows;
    spl->width = image.cols;
    if(!(spl->caption = strdup(((std::string)label).c_str()))) {
      perror("failed to allocate memory for caption!\n");
    }
    while(!g_data_pool.push(spl)) {
      /* If queue is full, maybe sleep a little bit?? */
      //this_thread::sleep_for(chrono::milliseconds(250));
    }
  }
}
  

void run_producers(void) {
  /* Run all of the threads for producing*/
 
  auto synthesizer = MapTextSynthesizer::create();
  prepare_synthesis(synthesizer);

  for(int i = 0; i < NUM_PRODUCERS; i++) {
    g_producer_threads.push_back(std::thread(synthesize_data, synthesizer));
  }
}


//consumer for use via python runtime
void* get_sample(void) {
  /* called within python generator function */
  sample_t* data;

  //yes, we want to pass a reference to the pointer
  while(!g_data_pool.pop(data)) {
    /*Failed to pop, wait maybe?*/
  }
  printf("img end %u\n",data->img_data[(data->height) * (data->width)-1]);

  return (void*)data;
}

/* Called before using python generator function */
void mts_init(void) {
  run_producers();
  
}

/* Called after using python generator function */
void mts_cleanup(void) {
  //tell producers to knock it off
  g_keep_producing = 0;
  
  //join all producer threads
  for(auto& thd : g_producer_threads) {
    thd.join();
  }
}

