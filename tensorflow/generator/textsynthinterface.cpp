/* 
   CNN-LSTM-CTC-OCR       
   C code wrapper for MTS C++ code

   Copyright (C) 2018 Benjamin Gafford, Ziwen Chen, Liam Niehus-Staab

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
extern "C" {
#include "mts_ipc.h"
#include "ipc_consumer.h"
}

struct MTS_Buffer {
  virtual void cleanup(void) = 0;
  virtual sample_t* get_sample(void) = 0;
};

struct MTS_Singlethreaded : MTS_Buffer {
  cv::Ptr<MapTextSynthesizer> mts;
  MTS_Singlethreaded(const char* config_path);
  void cleanup(void);
  sample_t* get_sample(void);
};

struct MTS_Multithreaded : MTS_Buffer {
  int num_producers;
  MTS_Multithreaded(const char* config_path, int num_producers);
  void cleanup(void);
  sample_t* get_sample(void);
};

MTS_Singlethreaded::MTS_Singlethreaded(const char* config_file) {
  this->mts = MapTextSynthesizer::create(config_file);
}

MTS_Multithreaded::MTS_Multithreaded(const char* config_file, \
				     int num_producers) {
  this->num_producers = num_producers;
  mts_ipc_init(num_producers, config_file);  
}

sample_t* MTS_Singlethreaded::get_sample(void) {
  auto mts = this->mts;
  
  std::string label;
  cv::Mat image;
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

  return spl;
}

sample_t* MTS_Multithreaded::get_sample(void) {
  return (sample_t*)mts_ipc_get_sample();
}

void MTS_Singlethreaded::cleanup(void) {
  /*Currently does nothing. Retained for potential future use. */
}

void MTS_Multithreaded::cleanup(void) {
  mts_ipc_cleanup();
}

// For ctypes visibility
extern "C" {
  unsigned char* get_img_data(void* spl);
  size_t get_height(void* spl);
  size_t get_width(void* spl);
  char* get_caption(void* spl);
  void* mts_init(const char* config_path, int num_producers);
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

/* Get a sample */
void* get_sample(void* mts_buff_arg) {
  MTS_Buffer* mts_buff = (MTS_Buffer*)mts_buff_arg;
  void* ret = ((MTS_Buffer*)mts_buff_arg)->get_sample();
  return ret;
}

/* Called before using python generator function */
void* mts_init(const char* config_path, int num_threads) {
  if(num_threads >= 1) {
    return (void*)new MTS_Multithreaded(config_path, num_threads);
  } else {
    return (void*)new MTS_Singlethreaded(config_path);
  }
}

/* Called after using python generator function */
void mts_cleanup(void* mts) {
  ((MTS_Buffer*)mts)->cleanup();
  free(mts);
}
