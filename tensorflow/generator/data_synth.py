'''
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
'''

import numpy as np
import ctypes as c
import cv2
import time
import os

def get_mts_interface_lib():
    """ Prep and return lib for mts interfacing """

    # Allow for transparent use when importing elsewhere in filesys
    libname = "libmtsi.so"
    abs_path = os.path.dirname(os.path.abspath(__file__))
    lib_path_complete = abs_path + os.path.sep + libname
    
    lib = c.cdll.LoadLibrary(lib_path_complete)

    # get_sample takes no args, returns void*
    lib.get_sample.argtypes = [c.c_void_p]
    lib.get_sample.restype = c.c_void_p 
    
    # free_sample takes void*, returns nothing
    lib.free_sample.argtypes = [c.c_void_p]
    lib.free_sample.restype = None
    
    # get_caption takes void*, returns char* (null terminated)
    lib.get_caption.argtypes = [c.c_void_p]
    lib.get_caption.restype = c.c_char_p 

    # get_height takes void*, returns size_t 
    lib.get_height.argtypes = [c.c_void_p]
    lib.get_height.restype = c.c_ulonglong 

    # get_width takes void*, returns size_t
    lib.get_width.argtypes = [c.c_void_p]
    lib.get_width.restype = c.c_ulonglong 
    
    # get_img_data takes void*, returns void*
    # (really an unsigned char* array of size width*height) 
    lib.get_img_data.argtypes = [c.c_void_p]
    lib.get_img_data.restype = c.c_void_p

    # in: string: config_path
    # out: void* to the MTS_Buff object
    lib.mts_init.argtypes = [c.c_char_p] 
    lib.mts_init.restype = c.c_void_p

    # in: void* to MTS_Buff, out: void
    lib.mts_cleanup.argtypes = [c.c_void_p]
    lib.mts_cleanup.restype = None
    
    return lib

def format_sample(lib, ptr):
    """ Transform raw data ptr into usable data """
    # For c array -> numpy conversion
    buffer_from_memory = c.pythonapi.PyBuffer_FromMemory
    buffer_from_memory.restype = c.py_object

    if not ptr:
        print "No sample produced."
        exit()

    # Trivial extraction for 'simple' args
    height = lib.get_height(ptr)
    raw_data = lib.get_img_data(ptr)
    caption = lib.get_caption(ptr)
    width = lib.get_width(ptr)
    
    raw_data_ptr = c.cast(raw_data, c.POINTER(c.c_ubyte))
    # https://stackoverflow.com/questions/4355524/getting-data-from-ctypes-array-into-numpy
    # Above link used as reference for c array -> numpy conversion
    buffer = buffer_from_memory(raw_data_ptr, width*height)
    img_flat = np.frombuffer(buffer, np.uint8)

    # Convert to [height, width, 1] shape
    img_shaped = np.reshape(img_flat, (height, width, 1))
    
    return (caption, img_shaped)


def data_generator(config_file):
    """ Generator to be used in tensorflow """
    mtsi_lib = get_mts_interface_lib()
    mts_buff = mtsi_lib.mts_init(config_file)
    
    while True:
        ptr = c.c_void_p(mtsi_lib.get_sample(mts_buff))
        (caption, image) = format_sample(mtsi_lib, ptr)
        yield caption, image
        mtsi_lib.free_sample(ptr)


def test_generator(num_values=10, show_images=False,
                   log_time=False, buffered=False):
    """ For testing purposes only """
    iter = None
    config_file = "config.txt"
    
    if not buffered:
        iter = data_generator(config_file)
    else:
        pass # Kept for future IPC stuff
        
    if log_time:
        start_time = time.time()
        
    for _ in range(num_values):
        caption, image = next(iter)
        if show_images:
            cv2.imshow(caption, image)
            cv2.waitKey(0)
            
    if buffered:
        pass # Kept for future IPC stuff
    
    if log_time:
        end_time = time.time()
        print "Time: ", end_time-start_time
