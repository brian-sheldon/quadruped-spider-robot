
//
// WIFI Includes
//

#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

#include "captive-portal.h"

//#include "page.html.h"
//#include "page.css.h"
//#include "page.js.h"

//
// Wifi Pre-Setup
//

const char* ssid = "";
const char* password = "";

// DNS Server for Captive Portal
//DNSServer dnsServer;
//const byte DNS_PORT = 53;

WebServer server(80);

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
      servosAngle( 90 );
      delay( 2000 );
      servosAngle( 0 );
      delay( 2000 );
      servosAngle( 90 );
      delay( 2000 );
      servosAngle( 180 );
      // R1   R2    L1    L2      R4    R3    L3    L4
      //  0,   1,    2,    3,      4,    5,    6,    7
    } else if ( poseStr == "R1" ) {
      setServoAngle(0, 90);
      delay( 1000 );
      setServoAngle(0, 180);
    } else if ( poseStr == "R2" ) {
      setServoAngle(1, 90);
      delay( 1000 );
      setServoAngle(1, 0);
    } else if ( poseStr == "R3" ) {
      setServoAngle(5, 90);
      delay( 1000 );
      setServoAngle(5, 0);
    } else if ( poseStr == "R4" ) {
      setServoAngle(4, 90);
      delay( 1000 );
      setServoAngle(4, 180);
    } else if ( poseStr == "L1" ) {
      setServoAngle(2, 90);
      delay( 1000 );
      setServoAngle(2, 0);
    } else if ( poseStr == "L2" ) {
      setServoAngle(3, 90);
      delay( 1000 );
      setServoAngle(3, 0);
    } else if ( poseStr == "L3" ) {
      setServoAngle(6, 90);
      delay( 1000 );
      setServoAngle(6, 0);
    } else if ( poseStr == "L4" ) {
      setServoAngle(7, 90);
      delay( 1000 );
      setServoAngle(7, 0);
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
  
  //server.on( "/", []() {
    //Serial.println( "HTTP request uri: /" );
    //server.send( 200, "text/html", pageHtml );
  //});
  server.on( "/page.css", []() {
    Serial.println( "HTTP request uri: /page.css" );
    //server.send( 200, "text/css", pageCss );
  });
  server.on( "/page.js", []() {
    Serial.println( "HTTP request uri: /page.js" );
    //server.send( 200, "text/javascript", pageJs );
  });
  
  
  // Web Server Routes
  server.on("/", handleRoot);
  server.on("/cmd", handleCommandWeb);
  server.on("/getSettings", handleGetSettings);
  server.on("/setSettings", handleSetSettings);
  
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
  
  server.handleClient();

}
