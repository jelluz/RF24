#!/usr/bin/env python

#
# Example using Dynamic Payloads
# 
#  This is an example of how to use payloads of a varying (dynamic) size.
# 

import time
import RF24
import os


########### USER CONFIGURATION ###########
# See https://github.com/TMRh20/RF24/blob/master/RPi/pyRF24/readme.md

# CE Pin, CSN Pin, SPI Speed

#RPi B+
# Setup for GPIO 22 CE and CE0 CSN for RPi B+ with SPI Speed @ 8Mhz
#radio = RF24(RPI_BPLUS_GPIO_J8_22, RPI_BPLUS_GPIO_J8_24, BCM2835_SPI_SPEED_8MHZ)
radio = RF24.RF24(RF24.RPI_V2_GPIO_P1_22, RF24.RPI_V2_GPIO_P1_24, RF24.BCM2835_SPI_SPEED_1MHZ)

##########################################

pipes = [0xF0F0F0F0E1, 0xF0F0F0F0D2]
send_payload = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ789012'

radio.begin()
radio.enableDynamicPayloads()
radio.setRetries(5,15)
radio.printDetails()
print "going to log some temperature now :)"

radio.openWritingPipe(pipes[1])
radio.openReadingPipe(1,pipes[0])
radio.startListening()

filename = os.path.join("/home",'pi',"temperature_log.txt")

# forever loop
while 1:
    # Pong back role.  Receive each packet, dump it out, and send it back

    # if there is data ready
    if radio.available():
        while radio.available():
            # Fetch the payload, and see if this was the last one.
            len = radio.getDynamicPayloadSize()
            receive_payload = radio.read(len)
            t_str = receive_payload.split("\x00")[0]
            try:
                tmp = float(t_str)/1000.0
            except:
                tmp = -999
            # Spew it
	    print 'received tmp:',tmp
            # save it
            with open(filename,'a') as fp:
                fp.write("{} {}\n".format(time.time(),tmp))
                

