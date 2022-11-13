#include <Arduino.h>

#include "ref_sys.h"

//		Timing variables
unsigned long top_time;
unsigned long cycle_time = 10000;

ref_sys curr_ref;

// Runs once
void setup() {
	Serial.begin(9600);

	if (Serial.available())
		Serial.println("-- TEENSY SERIAL START --");
    
 	// Hardware setup
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(3,OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
}

// Runs continuously
void loop() {
	
    byte arr[64];

	top_time = micros();


	digitalWrite(3, 1);	
	delay(100);
	digitalWrite(3, 0);
	delay(100);	

	curr_ref.read_serial();	

    curr_ref.hid_buff_write(arr);

	for(int i = 0; i <= 23; i++ ){
	Serial.print(i);
	Serial.print(" index   ");
    Serial.println(arr[i]);
	}

	
}