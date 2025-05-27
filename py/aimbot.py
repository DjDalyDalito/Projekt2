import mss #screenshotter
import numpy #image processing
import serial #pyserial för att kunna snacka med serial ports
import time #nödvändigt för tid-funktioner
import pyautogui
import cv2

SERIAL_PORT = "COM4"
SERIAL_BIT_RATE = 115200
LOWER_YELLOW = numpy.array([24,200,200]) #elr höj alla värden lite ksk, kolla vilken som blir mest accurate
UPPER_YELLOW = numpy.array([26,255,255])

#def is_brown(pixel: numpy.ndarray)  -> bool: #vi tar allt, annars är formatet: numpy.ndarray(shape, dtype=float, buffer=None, offset=0, strides=None, order=None)
    #return numpy.linalg.norm(pixel - BROWN) < TOLERANCE #format: linalg.norm(array, float)

# ta en skärmdump
# hitta alla pixlar med färgen _____
# om det finns pixlar med färgen
    # - beräkna närmsta pixel
    # - skicka dessa koordinater
    # - vänta på "OK" från .ino 
# delay innan nästa iteration

def main():
    FOV = int(input("Ange FOV (px): ") or 2560)
    SMOOTH = int(input("Ange SMOOTH (steg): ") or 1)
    ser = serial.Serial(SERIAL_PORT, SERIAL_BIT_RATE, timeout = 0.1) #öppnar seriellport (COM5)
    time.sleep(2) #låt arduinon signalera OK
    ser.reset_input_buffer() #rensar all eventuell data vi har från serieporten
    screenshot = mss.mss() #objekt för skärmdumpar
    monitor = screenshot.monitors[0] # är hela första skärmen
    #center_x = monitor["width"] // 2
    #center_y = monitor["height"] // 2
    while True: #samma som i alla andra språk - körs om och om igen tills programmet stängs
        line = ser.readline().decode(errors='ignore').strip()
        if line != "Moving":
            continue
        
        mouse_x, mouse_y = pyautogui.position()
        frame = numpy.array(screenshot.grab(monitor))[: ,: ,:3] #screenshot.grab = bild av hela skärmen, np.array = gör om till numpy array - array med siffror, :3 för dem tre första färgkanalerna (R G B)
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV) # opencv använder RGB, men gör om till HSV (hue saturation value) GÅR 100x fortare
        mask = cv2.inRange(hsv, LOWER_YELLOW, UPPER_YELLOW) #allt i bilden som ligger mellan lower och upper blir vitt i mask (255), resten svart (0)
        ys, xs = numpy.where(mask > 0) #returnerar två arrayer, en med rader och en med kolumner där masken är vit
        if xs.size: #om listan inte är tom dvs om det finns gula punkter
            distance = (xs - mouse_x)**2 + (ys - mouse_y)**2 #behöver inte ta roten ur ;) minsta d^2 är också minsta d, sparar säkert nån ms
            if distance.min() ** 0.5 > FOV:
                continue
            index = numpy.argmin(distance) #ger indexet för minsta d^2 dvs närmsta punkten
            tx, ty = int(xs[index]), int(ys[index])
            while True:
                dx = (tx - mouse_x) // SMOOTH
                dy = (ty - mouse_y) // SMOOTH
                if abs(dx) < 0.1 and abs(dy) < 0.1: # för att fixa flickering
                    break
                sx = max(min(dx, 127), -127) #constrain i py - min(dx, 127) => större än 127 blir 127, max(..., -127) => mindre än -127 blir -127
                sy = max(min(dy, 127), -127)
                ser.write(f"{sx} {sy}\n".encode()) #.encode gör om sträng till bytes som går att skicka direkt till serial port
                ser.readline() #läser av serial port
                    #printa error meddelande/ stäng av programmet om du inte får ett ok från serial port
                mouse_x += sx
                mouse_y += sy
                
        #time.sleep(0.01)
            
main()
