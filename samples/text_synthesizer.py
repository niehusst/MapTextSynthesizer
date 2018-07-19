#!/usr/bin/env python
# -*- coding: utf-8 -*-
import cv2
import sys
import numpy as np

# Global Variable definition

words=[]
#words=['PM','Charlie','Jerod','Anya','Titus']
#fonts=['cmmi10','eufm10','MathJax_Fraktur','Sans','Serif','URW Chancery L']
blocky_fonts=['MathJax_Fraktur','eufm10']
regular_fonts=['cmmi10','Sans','Serif']
cursive_fonts=['URW Chancery L']
        
s=cv2.text.MapTextSynthesizer_create()
pause=200

# GUI Callsback functions

def updateTime(x):
    global pause
    pause=x

def read_words(words_file):
    open_file = open(words_file, 'r')
    words_list =[]
    contents = open_file.readlines()
    for i in range(len(contents)):
       words_list.append(contents[i].strip('\n'))
    return words_list    
    open_file.close()

def initialiseSynthesizers():
    global s
    global words
    global fonts

    words = read_words("IA/Civil.txt")
    s.setSampleCaptions(words)

    s.setBlockyFonts(blocky_fonts)
    s.setRegularFonts(regular_fonts)
    s.setCursiveFonts(cursive_fonts)

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
            caption,mat=s.generateSample()
            cv2.imshow('Text Synthesizer Demo',mat)
            print caption
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
