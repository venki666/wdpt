#!/usr/bin/python3
import time
import threading
import RPi.GPIO as GPIO

from picamera2 import Picamera2
from picamera2.encoders import MJPEGEncoder
from picamera2.encoders import H264Encoder
from picamera2.outputs import FfmpegOutput
from picamera2 import Picamera2, Preview


#setup
IO = 16
GPIO.setmode(GPIO.BOARD)
GPIO.setup(IO, GPIO.OUT)
GPIO.output(IO,GPIO.HIGH)
picam2 = Picamera2()
encoder = MJPEGEncoder(10000000)
video_config = picam2.create_video_configuration(main={"size": (640, 480)})
picam2.configure(video_config)

#start record
picam2.start_recording(encoder, 'test.mjpeg')
FRAME_RATE = 120
FRAME_RATE = 1000000 // FRAME_RATE
picam2.set_controls({"AfMode": 1 ,"AfTrigger": 0 ,"FrameDurationLimits":(FRAME_RATE,FRAME_RATE)})
#10sec for camera to focus
time.sleep(10)

#start drop
GPIO.output(IO, GPIO.LOW)
delay_in_sec = 10 / 1000
time.sleep(delay_in_sec)
GPIO.output(IO, GPIO.HIGH)
#wait for 70sec 
time.sleep(70)
#done recording and cleaning up
picam2.stop_recording()
GPIO.cleanup()

