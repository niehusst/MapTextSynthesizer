import cv2
import sys
import os
import numpy as np
from ctypes import CDLL

# Set up MTS using Ctypes
path = os.environ['LD_LIBRARY_PATH']
sharedlibc = str(path) + 'libmtsynth.so'
mtsynth = CDLL(sharedlibc)

# Global Variable definition
mts = mtsynth.create()
pause=200 #starting miliseconds between images



### GUI Callsback functions

def updateTime(x):
    global pause
    pause=x

def initialiseSynthesizers():
    global mts
    # setup mts fonts and captions
    mts.setSampleCaptions("IA/Civil.txt")
    #mts.setSampleCaptions("latin_extended_additional.txt")

    mts.setBlockyFonts("blocky.txt")
    mts.setRegularFonts("regular.txt")
    mts.setCursiveFonts("cursive.txt")


    
### GUI window functions

def initWindows():
    global mts
    global pause
    # create window to visualize the images
    cv2.namedWindow('Text Synthesizer Demo',cv2.WINDOW_NORMAL)
    cv2.resizeWindow('Text Synthesizer Demo',1000,500)
    cv2.moveWindow('Text Synthesizer Demo',100,100)
    
    # create trackbar so the pause time can be dynamically adjuseted
    cv2.createTrackbar('Pause ms','Text Synthesizer Demo',int(pause),
                       500,updateTime)

def updateTrackbars():
    global pause
    cv2.setTrackbarPos('Pause ms','Text Synthesizer Demo',int(pause))

def guiLoop():
    global mts
    global pause
    k=''
    #run the gui image show loop until user presses 'q'
    while ord('q')!=k:
        if pause<500:
            # generate sample and store data
            [caption, [h,w,img_data], actual_h] = mts.generateSample()
            img = np.reshape(np.fromiter(img_data, np.uint8),(h,w))
            # show the sample image
            cv2.imshow('Text Synthesizer Demo',img)
            # print sample information
            print caption
            print "Image height: ", actual_h
        k=cv2.waitKey(pause+1)


        
# main; run the gui
if __name__=='__main__':
    # init MTS and gui window info
    initialiseSynthesizers()
    initWindows()
    updateTrackbars()
    # show images
    guiLoop()
