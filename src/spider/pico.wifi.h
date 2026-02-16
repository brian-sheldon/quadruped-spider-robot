
//
// WIFI Includes
//

#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

//#include "captive-portal.h"

#include "page.html.h"
#include "page.css.h"
#include "page.js.h"

//
// Wifi Pre-Setup
//

// define wifi login ssid and password
// - kept in a separate file to avoid being uploaded to github

#include "wifi.creds.h"

//const char* ssid = "";
//const char* password = "";

// DNS Server for Captive Portal
//DNSServer dnsServer;
//const byte DNS_PORT = 53;

//WebServer server(80);

// Prototypes
void handleGetSettings();
void handleSetSettings();

void handleRoot() {
  Serial.println( "handleRoot ..." );
  Serial.print( "  uri: " );
  Serial.println( server.uri() );
  Serial.print( "  num args: " );
  Serial.println( server.args() );
  server.send(200, "text/html", pageHtml);
}

void handleCommandWeb() {
  // We send 200 OK immediately so the web browser doesn't hang waiting for animation to finish
  Serial.println( "handleCommandWeb ..." );
  Serial.print( "  uri: " );
  Serial.println( server.uri() );
  Serial.print( "  num args: " );
  Serial.println( server.args() );
  if (server.hasArg("pose")) {
    String poseStr = server.arg( "pose" );
    int pose = poseStr.toInt();
    Serial.print( " pose: " );
    Serial.println( poseStr );
    if ( poseStr == "test" ) {
      // Note: should not be used when femor is attached to frame
      // as the frame limits servo movement, which could damage
      // servo.
      /*
      servosAngle( 90 );
      delay( 2000 );
      servosAngle( 0 );
      delay( 2000 );
      servosAngle( 90 );
      delay( 2000 );
      servosAngle( 180 );
      */
    } else {
      currentCommand = poseStr;
    }
    //currentCommand = server.arg("pose");
    exitIdle();
    server.send(200, "text/plain", "OK"); 
  } 
  else if (server.hasArg("go")) {
    currentCommand = server.arg("go");
    exitIdle();
    server.send(200, "text/plain", "OK");
  } 
  else if (server.hasArg("stop")) {
    currentCommand = "";
    server.send(200, "text/plain", "OK");
  }
  else {
    server.send(400, "text/plain", "Bad Args");
  }
  Serial.println( "" );
}

void handleGetSettings() {
  Serial.println( "handleGetSettings ..." );
  Serial.print( "  uri: " );
  Serial.println( server.uri() );
  Serial.print( "  num args: " );
  Serial.println( server.args() );
  String json = "{";
  json += "\"frameDelay\":" + String(frameDelay) + ",";
  json += "\"walkCycles\":" + String(walkCycles) + ",";
  json += "\"motorCurrentDelay\":" + String(motorCurrentDelay) + ",";
  json += "\"faceFps\":" + String(faceFps);
  json += "}";
  server.send(200, "application/json", json);
}

void handleSetSettings() {
  Serial.println( "handleSetSettings ..." );
  Serial.print( "  uri: " );
  Serial.println( server.uri() );
  Serial.print( "  num args: " );
  Serial.println( server.args() );
  if (server.hasArg("frameDelay")) frameDelay = server.arg("frameDelay").toInt();
  if (server.hasArg("walkCycles")) walkCycles = server.arg("walkCycles").toInt();
  if (server.hasArg("motorCurrentDelay")) motorCurrentDelay = server.arg("motorCurrentDelay").toInt();
  if (server.hasArg("faceFps")) faceFps = (int)max(1L, server.arg("faceFps").toInt());
  server.send(200, "text/plain", "OK");
}

void handleGetServoStateJson() {
  int servo = -1;
  if ( server.hasArg( "servo" ) ) {
    servo = server.arg( "servo" ).toInt();
  }
  Serial.print( "getServoStateJson servo: " );
  Serial.print( servo );
  if ( servo >= 0 && servo <= 7 ) {
    String json = servoCmd[servo].stateJson();
    server.send( 200, "application/json", json );
    Serial.println( " response sent ..." );
  } else {
    server.send( 200, "text/plain", "invalid request ..." );
    Serial.println( " invalid request ..." );
  }
}
void handleServoAngle() {
  int servo = -1;
  int angle = -1;
  if ( server.hasArg( "servo" ) ) {
    servo = server.arg( "servo" ).toInt();
  }
  if ( server.hasArg( "angle" ) ) {
    angle = server.arg( "angle" ).toInt();
  }
  if ( servo >= 0 && servo <= 7 ) {
    if ( angle >= 0 && angle <= 180 ) {
      servoCmd[servo].setAngle( angle );
      setServoAngle( servo, angle );
    }
  }
  String json = servoCmd[servo].stateJson();
  server.send( 200, "application/json", json );
}

//
// Wifi Setup
//

void wifiSetup() {
  //
  // wifi setup
  //
  // - flash the led a moderate speed to indicate wifi not connected
  //
  pinMode( LED_BUILTIN, OUTPUT );
  digitalWrite( LED_BUILTIN, HIGH );
  //
  Serial.print("Connecting to ");
  Serial.println(ssid);
  //
  WiFi.setTimeout(0);
  WiFi.begin( ssid, password );
  //
  int wifiTries = 0;
  while ( WiFi.status() != WL_CONNECTED ) {
    wifiTries++;
    Serial.print( "." );
    if ( ( wifiTries % 10 ) == 0 ) {
      Serial.print( " " );
      Serial.print( wifiTries );
      Serial.println( "" );
    }
    delay( 250 );
    digitalWrite( LED_BUILTIN, LOW );
    delay( 250 );
    digitalWrite( LED_BUILTIN, HIGH) ;
  }
  Serial.println( "" );

  Serial.println( "WiFi connected ..." );
  Serial.println( "IP address: " );
  Serial.println( WiFi.localIP() );
  
  // Start DNS Server for Captive Portal
  // This redirects ALL domain requests to the ESP32's IP
  //dnsServer.start(DNS_PORT, "*", myIP);
  
  // moved to function to be reusable for uri that is not /
  //server.on( "/", []() {
    //Serial.println( "HTTP request uri: /" );
    //server.send( 200, "text/html", pageHtml );
  //});
  server.on( "/page.css", []() {
    Serial.println( "HTTP request uri: /page.css" );
    server.send( 200, "text/css", pageCss );
  });
  server.on( "/page.js", []() {
    Serial.println( "HTTP request uri: /page.js" );
    server.send( 200, "text/javascript", pageJs );
  });
  server.on( "/favicon.ico", []() {
    //Serial.println( "HTTP request uri: /favicon.ico" );
    //server.send( 400, "text/plain", "" );
  });
  
  // Web Server Routes
  server.on("/", handleRoot);
  server.on("/cmd", handleCommandWeb);
  server.on("/getSettings", handleGetSettings);
  server.on("/setSettings", handleSetSettings);

  server.on( "/servoAngle", handleServoAngle );
  server.on( "/getServoStateJson", handleGetServoStateJson );

  // Catch-all route for captive portal
  // This ensures any URL redirects to the controller page
  server.onNotFound(handleRoot);
  
  server.begin();
  Serial.println( "Http server started ..." );
  
}

//
// Wifi Loop
//

void wifiLoop() {

  // Process DNS requests for captive portal
  //dnsServer.processNextRequest();
  handleWebServer( "wifiLoop" );
  

}
