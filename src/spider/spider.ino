


#include "spider.servo.h"
#include "spider.face.h"

#include "pico.wifi.h"

//Servo myservo;  // Create a servo object 

int pos = 0;    // Variable to store the servo position 
//int servoPin = 16; // Define the GPIO pin used for the signal

void setup() {
  //
  // Setup serial
  //
  // - flash the led fast to indicate serial not ready
  //
  Serial.begin(115200);
  int serialTries = 0;
  while ( serialTries++ < 100 && ! Serial ) {
    digitalWrite( LED_BUILTIN, HIGH );
    delay( 100 );
    digitalWrite( LED_BUILTIN, LOW );
    delay( 100 );
  }
  Serial.print( "serial start time in ms: " );
  Serial.println( serialTries * 10 );

  Serial.println( "" );
  Serial.println( "" );
  Serial.println( "" );
  Serial.println( "" );
  Serial.println( "" );

  Serial.println( "Starting ..." );
  
  servoSetup();
  faceSetup();
  wifiSetup();

  //myservo.attach(servoPin); // Attach the servo variable to the specific pin
}

void loop() {
  
  servoLoop();
  faceLoop();
  wifiLoop();

  // Sweep from 0 to 180 degrees
  for (pos = 0; pos <= 180; pos += 1) { 
    //myservo.write(pos);    // Tell the servo to go to a particular angle
    delay(15);             // Wait 15 milliseconds for the servo to reach the position 
  }
  // Sweep from 180 to 0 degrees
  for (pos = 180; pos >= 0; pos -= 1) { 
    //myservo.write(pos);    // Tell the servo to go to a particular angle
    delay(15);             // Wait 15 milliseconds for the servo to reach the position 
  }
}
