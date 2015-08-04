#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include "printf.h"
#include <SPI.h>
// Arduino Sketch to read the Texas Instruments
// TMP75 Temperature Sensor
//  It uses the I2C in the Wire Library
//  Written by J.Park jim(_dot_)buzz(_at_)gmail(_dot_)com
//  23 April 2012 reference

// Define includes
#include <Wire.h>        // Wire header file for I2C and 2 wire
#include <String.h>
#include <stdio.h>


// I used a Sensor PCB from a scrapped White Intel based iMac
// Note there are two sensor boards both with a TMP75 sensor in the iMac.

// TMP75 Address is 0x48 and from its Datasheet = A0,A1,A2 are all grounded.
int TMP75_Address = 0x48; // Apple iMac Temp Sensor Circular PCB
//int TMP75_Address = 0x49; // Apple iMac Temp Sensor Square PCB

// LCD pinouts used (rs, enable, d4, d5, d6, d7)
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };


// Declare some nice variables
char buffer[30] = " ";        // printbuffer
int decPlaces = 1;
int numOfBytes = 2;
int baudRate = 9600;
int columns = 16;   // LCD is 16 Column 2 Row Hitachi LCD
int rows = 2;
byte configReg = 0x01;  // Address of Configuration Register
byte bitConv = B01100000;   // Set to 12 bit conversion
byte rdWr = 0x01;       // Set to read write
byte rdOnly = 0x00;    // Set to Read
String temperature, SerialDegC, lcdDegC;
const int LED = 5;   // separate LED pin
RF24 radio(9, 10); 

//int serial_putc( char c, FILE * )
//{
	//Serial.write( c );
//
	//return c;
//}
//
//void printf_begin(void)
//{
	////JESUS - For reddirect stdout to /dev/ttyGS0 (Serial Monitor port)
	//stdout = freopen("/dev/ttyGS0","w",stdout);
	//delay(500);
	//printf("redirecting to Serial...");
	//
	////JESUS -----------------------------------------------------------
//}

// Setup the Arduino LCD (Columns, Rows)
void setup(){	

	digitalWrite(LED, HIGH);	//LED off


	//start radio:	
	radio.begin();
	// optionally, increase the delay between retries & # of retries
	radio.setRetries(15, 15);
	radio.enableDynamicPayloads();
	
	//ping out:
	radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(1, pipes[1]);

	// Dump the configuration of the rf unit for debugging
	


	// initialize serial communication:
	//Serial.begin(115200);		//pin 0 = input, pin 1 = output
	//Serial.begin(57600);	
	
	Serial.begin(baudRate);            // Set Serial Port speed
	Wire.begin();                      // Join the I2C bus as a master



	Wire.beginTransmission(TMP75_Address);       // Address the TMP75 sensor
	Wire.write(configReg);                       // Address the Configuration register
	Wire.write(bitConv);                         // Set the temperature resolution
	Wire.endTransmission();                      // Stop transmitting
	Wire.beginTransmission(TMP75_Address);       // Address the TMP75 sensor
	Wire.write(rdOnly);                          // Address the Temperature register
	Wire.endTransmission();                      // Stop transmitting
	//Serial.print(radio.getCRCLength());
	//Serial.println();
////	Serial.print(radio.get_status());
////	Serial.println();
	//Serial.print(radio.getDataRate());
	//Serial.println();
	//Serial.print(radio.getDynamicPayloadSize());
	//Serial.println();
	radio.printDetails();
	sprintf(buffer, "yeah baby5");
	Serial.println(buffer);	
        radio.printDetails();
}

// Main Program Infinite loop
void loop(){
	radio.printDetails();	
	float tmp = readTemp();             // Read the temperature now
	sendTemp(tmp);
	Serial.print(tmp,decPlaces);        // Temperature value to 1 Decimal place no newline to serial
	Serial.println(SerialDegC);          // Degree symbol after the temperature value with newline
	delay(1000);                          // Slow the reads down so we can follow the output
}

// Begin the reading the TMP75 Sensor
float readTemp(){
	// Now take a Temerature Reading
	Wire.requestFrom(TMP75_Address,numOfBytes);  // Address the TMP75 and set number of bytes to receive
	byte MostSigByte = Wire.read();              // Read the first byte this is the MSB
	byte LeastSigByte = Wire.read();             // Now Read the second byte this is the LSB

	// Being a 12 bit integer use 2's compliment for negative temperature values
	int TempSum = (((MostSigByte << 8) | LeastSigByte) >> 4);
	// From Datasheet the TMP75 has a quantisation value of 0.0625 degreesC per bit
	float temp = (TempSum*0.0625);
	//Serial.println(MostSigByte, BIN);   // Uncomment for debug of binary data from Sensor
	//Serial.println(LeastSigByte, BIN);  // Uncomment for debug  of Binary data from Sensor
	return temp;                           // Return the temperature value
}

int sendTemp(float tmp){
	char tmpBuffer[10]= " ";
	
	//sprintf(buffer, "Now sending temp %f ",tmp );
	
	//
	//Serial.print(buffer);
	//Serial.print(tmpBuffer);
        int bla =int(tmp*1000);
        
        sprintf(tmpBuffer,"%i",bla);
	Serial.print(bla);
	Serial.println(tmpBuffer);
        radio.stopListening();
	radio.write(&tmpBuffer, sizeof(tmpBuffer));
        radio.startListening();
	
}
