#!/usr/bin/python2.7
from RPIO.PWM import Servo
from time import sleep

STEP = 20
pos = 1500

s = Servo()
i = 0
while (True) :
    f = open('/home/pi/robot/servopos', 'r')
    stringpos = f.read()
    if (stringpos != ""):
        newpos = int(stringpos)
        break
    sleep(0.05)

while (True) :
    f = open('/home/pi/robot/servopos', 'r')
    stringpos = f.read()
    if (stringpos != ""):
        newpos = int(stringpos)
        if (newpos>pos):
            if (pos+STEP < newpos):
                pos = pos+STEP
            else:
                pos = newpos
        else:
            if (pos-STEP > newpos):
                pos = pos-STEP
            else:
                pos = newpos
        s.set_servo(17,round(pos,-1))
    print(pos)
    sleep(0.05)
