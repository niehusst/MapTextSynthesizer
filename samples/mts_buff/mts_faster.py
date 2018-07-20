import numpy as np
import ctypes as c
import cv2
import time

def get_lib():
    lib = c.cdll.LoadLibrary('./libmts_faster.so')
    
    # get_sample takes no args, returns void*
    lib.get_sample.argtypes = []
    lib.get_sample.restype = c.c_void_p 
    
    # free_sample takes void*, returns nothing
    lib.free_sample.argtypes = [c.c_void_p]
    lib.free_sample.restype = None
    
    # get_caption takes void*, returns char* (null terminated)
    lib.get_caption.argtypes = [c.c_void_p]
    lib.get_caption.restype = c.c_char_p 

    # get_height takes void*, returns size_t (null terminated)
    lib.get_height.argtypes = [c.c_void_p]
    lib.get_height.restype = c.c_ulonglong 

    # get_width takes void*, returns size_t (null terminated)
    lib.get_width.argtypes = [c.c_void_p]
    lib.get_width.restype = c.c_ulonglong 
    
    # get_img_data takes void*, returns void* (really an unsigned char* array of size width*height) 
    lib.get_img_data.argtypes = [c.c_void_p]
    lib.get_img_data.restype = c.c_void_p

    lib.mts_init.argtypes = []
    lib.mts_init.restype = None

    lib.mts_cleanup.argtypes = []
    lib.mts_cleanup.restype = None

    
    return lib
'''
print 'Starting script...'
lib = get_lib()

lib.mts_init()
time.sleep(1)
ptr = lib.get_sample()
height = lib.get_height(ptr)
raw_data = lib.get_img_data(ptr)
caption = lib.get_caption(ptr)
width = lib.get_width(ptr)
raw_data_ptr = c.cast(raw_data, c.POINTER(c.c_ubyte))
print type(raw_data_ptr), '-------------------------------------------'
# https://stackoverflow.com/questions/4355524/getting-data-from-ctypes-array-into-numpy
print 'Caption:', caption
#print "raw data ", raw_data[0]
#print '________________________________________________', type(c.POINTER(c.c_void_p(raw_data)))
#img_flat = np.ctypeslib.as_array(c.c_ubyte * width*height).from_address(raw_data_ptr.contents)
#print type(img_flat)


buffer_from_memory = c.pythonapi.PyBuffer_FromMemory
buffer_from_memory.restype = c.py_object
buffer = buffer_from_memory(raw_data_ptr, width*height)
img_flat = np.frombuffer(buffer, np.uint8)

img_good = np.reshape(img_flat, (height, width))

print 'Ptr:', ptr
print 'Caption:', caption
print 'Height:', height
print 'Width:', width
print 'np array:', img_good

cv2.imshow('image', img_good)
cv2.waitKey(0)
lib.free_sample(ptr)

lib.mts_cleanup()
print 'Well, that worked.'
'''
def data_generator(lib):
    while True:
        ptr = lib.get_sample()
        '''
        if not ptr:
            print "ran out of memory :("
            exit()
        '''
        height = lib.get_height(ptr)
        raw_data = lib.get_img_data(ptr)
        caption = lib.get_caption(ptr)
        width = lib.get_width(ptr)
        raw_data_ptr = c.cast(raw_data, c.POINTER(c.c_ubyte))
        # https://stackoverflow.com/questions/4355524/getting-data-from-ctypes-array-into-numpy
        buffer_from_memory = c.pythonapi.PyBuffer_FromMemory
        buffer_from_memory.restype = c.py_object
        buffer = buffer_from_memory(raw_data_ptr, width*height)
        img_flat = np.frombuffer(buffer, np.uint8)

        img_good = np.reshape(img_flat, (height, width))
        yield caption, img_good
        lib.free_sample(ptr)


def gather_data(num_values):
    lib = get_lib()
    lib.mts_init()
    iter = data_generator(lib)
    for _ in range(num_values):
        next(iter)
    lib.mts_cleanup()

start = time.time()
gather_data(10000)
#print 'hello?'
end = time.time()
print 'Time: ', end-start
