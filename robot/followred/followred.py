#!/usr/bin/python2.7
import io
import time
import picamera
import picamera.array
import cv2
import numpy as np
from matplotlib import pyplot as plt
import matplotlib.image as mpimg

width,height = 32,24

def redness(colour):
    return int(100*colour[2])/(int(colour[1])+int(colour[0])+20)

def plot():
    plt.subplot(1,2,1), plt.imshow(red,'gray')
    plt.title("red")
    b,g,r = cv2.split(image)
    rgb = cv2.merge([r,g,b])
    plt.subplot(1,2,2), plt.imshow(rgb)
    plt.show()
    
print "starting"
while (True):
    with picamera.PiCamera() as camera:
        print "camera ready"
        camera.resolution = (640,480)
        with picamera.array.PiRGBArray(camera) as stream:
            camera.capture(stream, format='bgr')
            image = cv2.resize(stream.array, (width,height))
    print "captured"
    best = 0
    besti = 0
    bestj = 0
    red = np.zeros((len(image),len(image[0])))
    for i in range(len(image)):
        for j in range(len(image[0])):
            red[i][j] = redness(image[i][j])
            if (red[i][j]>best):
                best = red[i][j]
                besti, bestj = i,j
    phi = 3.14*(bestj-(width/2))/width #range from -pi/2 to pi/2
    r = 0.3
    print bestj, phi, best
    dirfile = open('direction','w')
    if (best>78):
        dirfile.write("radial\n"+str(phi)+" "+str(r));
        print("radial\n"+str(phi)+" "+str(r));
    else:
        dirfile.write("radial\n0 0");
        print("radial\n0 0");
    print "i: ", besti,"j: ", bestj, " score: ", best
    #plot()

