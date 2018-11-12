import sensor, image, time
import json
from pyb import UART
# For color tracking to work really well you should ideally be in a very, very,
# very, controlled enviroment where the lighting is constant...
#yellow_threshold   = ( 46,  100,  -68,   72,   58,   92)
red_thresholds = (20, 75, 28, 127, -128, 127)
balck_thresholds = (5, 15, -10, 2, -6, 6)

#black_thresholds = (0,30,-50,50,-50,50)
# You may need to tweak the above settings for tracking green things...
# Select an area in the Framebuffer to copy the color settings.

sensor.reset() # Initialize the camera sensor.
sensor.set_pixformat(sensor.RGB565) # use RGB565.
sensor.set_framesize(sensor.QQVGA) # use QQVGA for speed.
sensor.skip_frames(time = 2000) # Let new settings take affect.
#sensor.set_auto_whitebal(False) # turn this off.
clock = time.clock() # Tracks FPS.

uart = UART(3, 115200)

while(True):
    clock.tick() # Track elapsed milliseconds between snapshots().
    img = sensor.snapshot() # Take a picture and return the image.

    red_blobs_data = []
    balck_spot_data = []
    red_blobs = img.find_blobs([red_thresholds])
    black_spot = img.find_blobs([balck_thresholds])
    if red_blobs:
        print('sum : %d'% len(red_blobs))
        for b in red_blobs:
            # Draw a rect around the blob.
            img.draw_rectangle(b.rect()) # rect
            img.draw_cross(b.cx(), b.cy()) # cx, cy
            red_blobs_data.append((b.cx(),b.cy()))

    if black_spot:
        print('sum : %d'% len(red_blobs))
        for b in black_spot:
            # Draw a rect around the blob.
            img.draw_rectangle(b.rect()) # rect
            img.draw_cross(b.cx(), b.cy()) # cx, cy
            balck_spot_data.append((b.cx(),b.cy()))

    rx_buf = ""
    if len(red_blobs_data) > 0:
        rx_buf = str(red_blobs_data[0])
        rx_buf = "R" + rx_buf + "B"

    if len(balck_spot_data) >= 9:
        for i in range(9):
            rx_buf = rx_buf + str(balck_spot_data[i])
            i = i + 1
    else:
        i = 0
        while i < len(balck_spot_data):
            rx_buf = rx_buf + str(balck_spot_data[i])
            i = i + 1
    if len(rx_buf) > 0:
        i = 0
        while i < len(rx_buf):
            temp = rx_buf[i]
            uart.write(temp)
            i = i + 1;
        uart.write("\r\n")
        print(rx_buf + "\r\n")
