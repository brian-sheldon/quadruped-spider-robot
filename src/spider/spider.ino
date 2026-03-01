

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
  frameLoop();
  //
  // do every loop
  //
  servoLoop();
  faceLoop();
  wifiLoop();
}

//
// vars for frame timings
//
unsigned long micro = micros();
unsigned long fps60 = 16667;
unsigned long fps100 = 10000;
unsigned long fps10 = 100000;
unsigned long fps1 = 1000000;
unsigned long int fps60next = micro + fps60;
unsigned long int fps100next = micro + fps100;
unsigned long int fps10next = micro + fps10;
unsigned long int fps1next = micro + fps1;
int fps60count = 0;
int fps100count = 0;
int fps10count = 0;
int fps1count = 0;
//
// vars for application frames
//
float batteryVolts = 0;
int batteryPercent = 0;

void frameLoop() {
  //
  // do every so many millis
  //
  micro = micros();
  //
  // 60 fps
  //
  if ( micro > fps60next ) {
    fps60count++;
    fps60next += fps60;
  }
  //
  // 100 fps
  //
  if ( micro > fps100next ) {
    fps100count++;
    fps100next += fps100;
  }
  //
  // 10 fps
  //
  if ( micro > fps10next ) {
    fps10count++;
    fps10next += fps10;
  }
  //
  // 1 fps
  //
  if ( micro > fps1next ) {
    fps1count++;
    fps1next += fps1;
    //
    // use modulo for any intervals less than a second
    //
    // - every 5 seconds
    //
    if ( ( fps1count % 5 ) == 0 ) {
      if ( true ) {
        //
        // check battery
        //
        analogReadResolution(12);
        //
        int adc28 = analogRead( 28 );
        float voltage = adc28 * 3.3 * 3 / 4096;
        if ( voltage < 6.8 ) {
          voltage = 6.8;
        }
        if ( batteryVolts == 0 ) {
          batteryVolts = voltage;
        } else {
          batteryVolts = 0.8 * batteryVolts + 0.2 * voltage;
        }
      }
    }
    //
    // - every 10 seconds
    //
    if ( ( fps1count % 10 ) == 0 ) {
      oledInit();
    }
    //
    // - every minute
    //
    if ( ( fps1count % 60 ) == 0 ) {
      if ( true ) {
        //float percent[] = { 3.40, 3.60, 3.70, 3.75, 3.80, 3.83, 3.90, 3.96, 4.03, 4.10, 4.20 };
        //                     0     5    10    15    20    25    30    35    40    45    50    55    60    65    70    75    80    85    90    95   100
        float percent[] = { 6.80, 7.22, 7.37, 7.41, 7.45, 7.49, 7.53, 7.57, 7.59, 7.63, 7.67, 7.71, 7.75, 7.83, 7.91, 7.97, 8.05, 8.16, 8.22, 8.30, 8.40 };
        int batteryPercent = 0;
        for ( int i = 0; i < 20; i++ ) {
          float l = percent[i];
          float h = percent[i+1];
          float d = ( h - l ) / 5;
          if ( batteryVolts >= l && batteryVolts <= h ) {
            batteryPercent = i * 5 + ( ( batteryVolts - l ) / d );
          }
          if ( batteryVolts > 8.4 ) {
            batteryPercent = 100;
          }
        }
        Serial.print( "Battery " );
        Serial.print( " volts: " );
        Serial.print( batteryVolts );
        Serial.print( " percent: " );
        Serial.println( batteryPercent );
        wifiInfoText = String( batteryVolts );
      }
    }
  }
}
