/* 
   CNN-LSTM-CTC-OCR
   Copyright (C) 2018 Benjamin Gafford, Ziwen Chen

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string>
#include <vector>
#include <fstream>
#include <mtsynth/map_text_synthesizer.hpp>
#include <stdio.h>

// Contains all of the raw data of a sample
typedef struct sample {
  unsigned char* img_data;
  size_t height;
  size_t width;
  char* caption;
} sample_t;


struct MTS_Buffer {
  cv::Ptr<MapTextSynthesizer> mts;
  MTS_Buffer(const char* config_path, const char* lexicon_path);
  void cleanup(void);
};

void prepare_synthesis(cv::Ptr<MapTextSynthesizer> s,
		       const char* lexicon_path);
MTS_Buffer::MTS_Buffer(const char* config_path, const char* lexicon_path) {
  this->mts = MapTextSynthesizer::create(config_path);
  prepare_synthesis(this->mts, lexicon_path);
}

void MTS_Buffer::cleanup() {
  /*Currently does nothing. Retained for potential future use. */
}

// For ctypes visibility
extern "C" {
  unsigned char* get_img_data(void* spl);
  size_t get_height(void* spl);
  size_t get_width(void* spl);
  char* get_caption(void* spl);
  void* mts_init(const char* config_path, const char* lexicon_path);
  void* get_sample(void* mts_buff);
  void free_sample(void* spl);
  void mts_cleanup(void* mts_buff);
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

/* Prepare synthesizer object for synthesis w/ params */
void prepare_synthesis(cv::Ptr<MapTextSynthesizer> s,
		       const char* lexicon_path) {
  vector<String> caps;
  
  vector<string> blocky;
  blocky.push_back("Sans");
  blocky.push_back("Serif");

  vector<string> regular;
  //regular.push_back("cmmi10");
  regular.push_back("Sans");
  regular.push_back("Serif");

  vector<string> cursive;
  cursive.push_back("Sans");

  s->setSampleCaptions(lexicon_path);
  s->setBlockyFonts(blocky);
  s->setRegularFonts(regular);
  s->setCursiveFonts(cursive);
}

/* Get a sample */
void* get_sample(void* mts_buff_arg) {
  //extract actual mts
  auto mts_buff = (MTS_Buffer*)mts_buff_arg;
  auto mts = mts_buff->mts;
  
  string label;
  Mat image;
  int height;

  // Fill in label, image
  mts->generateSample(label, image, height);

  // Stick the necessary data into sample_t struct
  sample_t* spl;
  if(!(spl = (sample_t*) malloc(sizeof(sample_t)))) {
    perror("Failed to allocate sample struct data!\n");
  }

  //NOTE: implied single channel here -- won't work with nongray images!
  size_t num_elements = image.rows * image.cols;
  size_t buff_size = num_elements * sizeof(unsigned char);

  // Allocate enough space for img_data
  if(!(spl->img_data = (unsigned char*) malloc(buff_size))) {
    perror("Failed to allocate image data!\n");
  }

  // Copy into image data
  memcpy(spl->img_data, image.data, buff_size);
  spl->height = image.rows;
  spl->width = image.cols;

  //Get copy of label
  if(!(spl->caption = strdup(((std::string)label).c_str()))) {
    perror("Failed to allocate memory for caption!\n");
  }

  return (void*)spl;
}

/* Called before using python generator function */
void* mts_init(const char* config_path, const char* lexicon_path) {
  return new MTS_Buffer(config_path, lexicon_path);
}

/* Called after using python generator function */
void mts_cleanup(void* mts) {
  ((MTS_Buffer*)mts)->cleanup();
  free(mts);
}

