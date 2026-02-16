

//#include "pico.wifi.h"

#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

WebServer server(80);

int loops = 0;
void handleWebServer( String src ) {
  loops++;
  unsigned long ms = millis();
  /*
  Serial.print( "handleWebServer loops: " );
  Serial.print( loops );
  Serial.print( " millis: " );
  Serial.print( ms );
  Serial.print( " src: " );
  Serial.println( src );
  */
  /*
  if ( ( loops % 10 ) == 0 ) {
    Serial.print( "handleWebServer loops: " );
    Serial.print( loops );
    Serial.print( " millis: " );
    Serial.println( ms );
  }
  */
  server.handleClient();
}

#include "spider.servo.h"
#include "spider.face.h"
#include "animate.h"

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

}
