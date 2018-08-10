import numpy as np
import mtsynth
import time

# Python example that uses boost-python to run MTS

def read_words(words_file):
    open_file = open(words_file, 'r')
    words_list =[] 
    contents = open_file.readlines()
    for i in range(len(contents)):
        words_list.append(contents[i].strip('\n'))
    return words_list    
    open_file.close()

def initialiseSynthesizers(s):
    blocky_fonts=['MathJax_Fraktur','eufm10']
    regular_fonts=['cmmi10','Sans','Serif']
    cursive_fonts=['URW Chancery L']

    words = read_words("IA/Civil.txt")

    #s.setSampleCaptions(words)
    s.setSampleCaptions(words)

    s.setBlockyFonts(blocky_fonts)
    s.setRegularFonts(regular_fonts)
    s.setCursiveFonts(cursive_fonts)


def data_gen():
    while True:
        [caption, [h,w,img_data]] = s.generateSample()
        img = np.reshape(np.fromiter(img_data, np.uint8),(h,w))
        yield caption


def main():
    iter = data_gen()

    start=time.time()
    for _ in range(1000):
        next(iter)
        
    end=time.time()
    print('Time: ', end-start)
    #cv2.imshow("Sample", img)
    #cv2.waitKey(0)
    #val = mtsynth.generate_sample()
    #exit()
    #s = (mtsynth.MapTextSynthesizer.create())
    s = mtsynth.MapTextSynthesizer()
    initialiseSynthesizers(s)


if __name__=="__main__":
    main()
