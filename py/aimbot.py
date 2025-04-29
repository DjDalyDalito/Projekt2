import mss #screenshotter
import numpy #image processing
import serial #pyserial för att kunna snacka med serial ports
import time #nödvändigt för tid-funktioner

SERIAL_PORT = "COM3"
SERIAL-BIT-RATE = 9600
BROWN = numpy.array(150,100,50) #elr 150, 75, 0, kolla vilken som blir mest accurate
TOLERANCE = 40;

def is_brown(pixel: numpy.ndarray)  -> bool: #vi tar allt, annars är formatet: numpy.ndarray(shape, dtype=float, buffer=None, offset=0, strides=None, order=None)
    return numpy.linalg.norm(pixel - BROWN) < TOLERANCE #format: linalg.norm(array, float)

def main():
    