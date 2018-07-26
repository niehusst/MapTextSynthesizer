import cv2
import mtsynth
import sys
import numpy as np

# Global Variable definition

s = mtsynth.MapTextSynthesizer()
pause=200

# GUI Callsback functions

def updateTime(x):
    global pause
    pause=x

'''
def read_words(words_file):
    open_file = open(words_file, 'r')
    words_list =[]
    contents = open_file.readlines()
    for i in range(len(contents)):
       words_list.append(contents[i].strip('\n'))
    return words_list    
    open_file.close()
'''

def initialiseSynthesizers():
    global s

    s.setSampleCaptions("IA/Civil.txt")
    #s.setSampleCaptions("latin_extended_additional.txt")

    s.setBlockyFonts("blocky.txt")
    s.setRegularFonts("regular.txt")
    s.setCursiveFonts("cursive.txt")

# Other functions

def initWindows():
    global s
    global pause
    cv2.namedWindow('Text Synthesizer Demo',cv2.WINDOW_NORMAL)
    cv2.resizeWindow('Text Synthesizer Demo',1000,500)
    cv2.moveWindow('Text Synthesizer Demo',100,100)
    cv2.createTrackbar('Pause ms','Text Synthesizer Demo',int(pause),500,updateTime)

def updateTrackbars():
    global s
    global pause
    cv2.setTrackbarPos('Pause ms','Text Synthesizer Demo',int(pause))

def guiLoop():
    global s
    global pause
    k=''
    while ord('q')!=k:
        if pause<500:
            [caption, [h,w,img_data], actual_h] = s.generateSample()
            img = np.reshape(np.fromiter(img_data, np.uint8),(h,w))
            cv2.imshow('Text Synthesizer Demo',img)
            print caption
            print "actual: ", actual_h
        k=cv2.waitKey(pause+1)

# Main Programm

if __name__=='__main__':
    #colorImg=cv2.imread('1000_color_clusters.png',cv2.IMREAD_COLOR)
    #1000_color_clusters.png has the 3 most dominant color clusters 
    #from the first 1000 samples of MSCOCO-text trainset
    #print helpStr
    initialiseSynthesizers()
    initWindows()
    updateTrackbars()
    guiLoop()
