import numpy as np
import cv2
import sys
import os

if __name__=='__main__':
    #show png
    if len(sys.argv) > 1: 
        filename = sys.argv[1]
        img=cv2.imread(filename,-1)
        if len(img.shape)==3 and img.shape[2]==4:
            blank=np.zeros((img.shape[0],img.shape[1],3),np.uint8)
            blank[:,:]=(255,255,255)
            alpha_channel = img[:,:,3]
            alpha_channel[alpha_channel!=0]=255
            mask_in = alpha_channel
            mask_out = cv2.bitwise_not(alpha_channel)
            color = img[:,:,:3]
            new_img = cv2.bitwise_or(color,blank,mask=mask_out)
            new_img = cv2.bitwise_or(new_img,color)
            cv2.imshow("image",new_img)
            cv2.waitKey(0)
            cv2.destroyAllWindows()
        else:
            cv2.imshow("image",img)
            cv2.waitKey(0)
            cv2.destroyAllWindows()

