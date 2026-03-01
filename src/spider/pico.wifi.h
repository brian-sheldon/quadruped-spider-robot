
//
// WIFI Includes
//

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebServer.h>
#include <HTTPClient.h>

//#include "captive-portal.h"

#include "page.html.h"
#include "page.css.h"
#include "page.js.h"

#include <algorithm>
#include <string>

//
// Wifi Pre-Setup
//

WiFiMulti wifiMulti;

// define wifi login ssid and password
// - kept in a separate file to avoid being uploaded to github

#include "wifi.creds.h"

#define ENABLE_NETWORK_MODE false  // Set to true to enable network connection attempts

// DNS Server for Captive Portal
//DNSServer dnsServer;
//const byte DNS_PORT = 53;

//WebServer server(80);

// Prototypes
void handleGetSettings();
void handleSetSettings();

void printReqInfo( String src ) {
  String s = "";
  String uri = server.uri();
  int args = server.args();
  String sargs = String( args );
  s += src;
  s += " ...";
  s += " uri: " + uri;
  s += " num args: " + sargs;
  for ( int i = 0; i < args; i++ ) {
    s += " " + server.argName( i ) + ": ";
    s += server.arg( i );
  }
  Serial.println( s );
}

void handleNotFound() {
  printReqInfo( "handleNotFound" );
}

void handleRoot() {
  printReqInfo( "handleRoot" );
  server.send( 200, "text/html", pageHtml );
}

void handleCommandWeb() {
  // We send 200 OK immediately so the web browser doesn't hang waiting for animation to finish
  printReqInfo( "handleCommandWeb" );
  if (server.hasArg("pose")) {
    String poseStr = server.arg( "pose" );
    int pose = poseStr.toInt();
    Serial.print( " pose: " );
    Serial.println( poseStr );
    if ( poseStr == "test" ) {
      // Place to put code executed by the test button
      // Note: should not be used when femor is attached to frame
      // as the frame limits servo movement, which could damage
      // servo.
      analogReadResolution(12);
      pinMode( 28, INPUT );
      int adc28 = analogRead( 28 );
      if ( true ) {
        Serial.print( "Battery reading: " );
        Serial.print( adc28 );
        float voltage = adc28 * 3.3 * 3 / 4096;
        Serial.print( " voltage: ");
        Serial.println( voltage );
        //wifiInfoText = String( voltage );
        server.send( 200, "text/plain", String( voltage ) );
      }
    } else {
      currentCommand = poseStr;
    }
    recordInput();
    exitIdle();
    server.send( 200, "text/plain", "OK" ); 
  } 
  else if (server.hasArg("go")) {
    currentCommand = server.arg("go");
    recordInput();
    exitIdle();
    server.send(200, "text/plain", "OK");
  } 
  else if (server.hasArg("stop")) {
    currentCommand = "";
    recordInput();
    server.send(200, "text/plain", "OK");
  }
  else if (server.hasArg("motor") && server.hasArg("value")) {
    int motorNum = server.arg("motor").toInt();
    int servoIdx = servoNameToIndex(server.arg("motor"));
    int angle = server.arg("value").toInt();
    if (motorNum >= 1 && motorNum <= 8 && angle >= 0 && angle <= 180) {
      setServoAngle(motorNum - 1, angle); // Convert 1-based to 0-based index
      recordInput();
      server.send(200, "text/plain", "OK");
    } else if (servoIdx != -1 && angle >= 0 && angle <= 180) {
      setServoAngle(servoIdx, angle);
      recordInput();
      server.send(200, "text/plain", "OK");
    } else {
      server.send(400, "text/plain", "Invalid motor or angle");
    }
  }
  else {
    server.send(400, "text/plain", "Bad Args");
  }
}

void handleGetSettings() {
  printReqInfo( "handleGetSettings" );
  String json = "{";
  json += "\"frameDelay\":" + String(frameDelay) + ",";
  json += "\"walkCycles\":" + String(walkCycles) + ",";
  json += "\"motorCurrentDelay\":" + String(motorCurrentDelay) + ",";
  json += "\"faceFps\":" + String(faceFps);
  json += "}";
  server.send(200, "application/json", json);
}

void handleSetSettings() {
  printReqInfo( "handleSetSettings" );
  if (server.hasArg("frameDelay")) frameDelay = server.arg("frameDelay").toInt();
  if (server.hasArg("walkCycles")) walkCycles = server.arg("walkCycles").toInt();
  if (server.hasArg("motorCurrentDelay")) motorCurrentDelay = server.arg("motorCurrentDelay").toInt();
  if (server.hasArg("faceFps")) faceFps = (int)max(1L, server.arg("faceFps").toInt());
  server.send( 200, "text/plain", "OK" );
}

void handleGetStatus() {
  printReqInfo( "handleGetSettings" );
  String json = "{";
  json += "\"currentCommand\":\"" + currentCommand + "\",";
  json += "\"currentFace\":\"" + currentFaceName + "\",";
  json += "\"networkConnected\":" + String(networkConnected ? "true" : "false") + ",";
  json += "\"apIP\":\"" + WiFi.softAPIP().toString() + "\"";
  if (networkConnected) {
    json += ",\"networkIP\":\"" + networkIP.toString() + "\"";
  }
  json += "}";
  server.send( 200, "application/json", json );
}

void handleApiCommand() {
  printReqInfo( "handleApiCommmand" );
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"error\":\"Method not allowed\"}");
    return;
  }
  
  String body = server.arg("plain");
  
  Serial.println("API Command received:");
  Serial.println(body);
  
  // Check for face-only command (no movement)
  int faceOnlyStart = body.indexOf("\"face\":\"");
  if (faceOnlyStart == -1) {
    faceOnlyStart = body.indexOf("\"face\": \"");
  }
  
  // If we have a face but no command field, it's face-only
  bool faceOnly = (faceOnlyStart > 0 && body.indexOf("\"command\":") == -1 && body.indexOf("\"command\": ") == -1);
  
  String command = "";
  String face = "";
  
  // Parse face
  if (faceOnlyStart > 0) {
    faceOnlyStart = body.indexOf("\"", faceOnlyStart + 6) + 1;
    int faceEnd = body.indexOf("\"", faceOnlyStart);
    if (faceEnd > faceOnlyStart) {
      face = body.substring(faceOnlyStart, faceEnd);
      Serial.print("Parsed face: ");
      Serial.println(face);
    }
  }
  
  // Parse command (if not face-only)
  if (!faceOnly) {
    int cmdStart = body.indexOf("\"command\":\"");
    if (cmdStart == -1) {
      cmdStart = body.indexOf("\"command\": \"");
    }
    
    if (cmdStart == -1) {
      Serial.println("Error: command field not found");
      server.send(400, "application/json", "{\"error\":\"Missing command field\"}");
      return;
    }
    
    cmdStart = body.indexOf("\"", cmdStart + 10) + 1;
    int cmdEnd = body.indexOf("\"", cmdStart);
    
    if (cmdEnd <= cmdStart) {
      Serial.println("Error: invalid command format");
      server.send(400, "application/json", "{\"error\":\"Invalid command format\"}");
      return;
    }
    
    command = body.substring(cmdStart, cmdEnd);
    Serial.print("Parsed command: ");
    Serial.println(command);
  }
  
  // Set face if provided
  if (face.length() > 0) {
    setFace(face);
  }
  
  // If face-only, just acknowledge
  if (faceOnly) {
    recordInput();
    server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Face updated\"}");
    return;
  }
  
  // Execute command
  if (command == "stop") {
    currentCommand = "";
    recordInput();
    server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Command stopped\"}");
  } else {
    currentCommand = command;
    recordInput();
    exitIdle();
    server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Command executed\"}");
  }
}


void handleGetServoStateJson() {
  printReqInfo( "handleGetServoStateJson" );
  int servo = -1;
  if ( server.hasArg( "servo" ) ) {
    servo = server.arg( "servo" ).toInt();
  }
  Serial.print( "getServoStateJson servo: " );
  Serial.print( servo );
  if ( servo >= 0 && servo <= 7 ) {
    String json = servoConfig[servo].stateJson();
    server.send( 200, "application/json", json );
    Serial.println( " response sent ..." );
  } else {
    server.send( 200, "text/plain", "invalid request ..." );
    Serial.println( " invalid request ..." );
  }
}
void handleServoAngle() {
  printReqInfo( "handleServoAngle" );
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
      servoConfig[servo].setAngle( angle );
      setServoAngle( servo, angle );
    }
  }
  String json = servoConfig[servo].stateJson();
  server.send( 200, "application/json", json );
}

//
// Wifi Connect
//

bool USE_WIFI_MULTI = true;
bool wifiInitDone = false;
bool wifiConnected = false;
bool wifiConnectChanged = false;
int wifiConnectLoops = 0;

bool wifiIsConnected() {
  int status;
  if ( ! USE_WIFI_MULTI ) {
    status =  WiFi.status();
  } else {
    status = wifiMulti.run();
  }
  return ( status == WL_CONNECTED );
}

void wifiConnect() {
  if ( wifiIsConnected() ) {
    return;
  } else {
    //
    // wifi init if not previously done
    // - flash the led a moderate speed to indicate wifi not connected
    //
    if ( ! wifiInitDone ) {
      wifiInitDone = true;
      digitalWrite( LED_BUILTIN, HIGH );
      //
      if ( ! USE_WIFI_MULTI ) {
        WiFi.setTimeout(0);
        WiFi.begin( ssid2, password2 );
      } else {
        wifiMulti.addAP( ssid1, password1 );
        wifiMulti.addAP( ssid2, password2 );
      }
    }
    //
    while ( ! wifiIsConnected() ) {
      wifiConnectLoops++;
      Serial.print( "." );
      if ( ( wifiConnectLoops % 10 ) == 0 ) {
        Serial.print( " " );
        Serial.print( wifiConnectLoops );
        Serial.println( "" );
      }
      delay( 250 );
      digitalWrite( LED_BUILTIN, LOW );
      delay( 250 );
      digitalWrite( LED_BUILTIN, HIGH) ;
      //
    }
    // display connect info
    Serial.println( "" );
    
    Serial.print( "WiFi connected to ssid: " );
    Serial.print( WiFi.SSID() );
    Serial.print( " address: " );
    IPAddress ipAddr = WiFi.localIP();
    String ipAddrStr = ipAddr.toString();
    Serial.print( ipAddrStr );
    Serial.println( " ... " );
    
    networkConnected = true;
    wifiInfoText = "Wifi connected ... " + ipAddrStr;
  }
}

void wifiSetup() {
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

  // API endpoints for network communication
  server.on("/api/status", handleGetStatus);
  server.on("/api/command", handleApiCommand);

  server.on( "/servoAngle", handleServoAngle );
  server.on( "/getServoStateJson", handleGetServoStateJson );

  // Catch-all route for captive portal
  // This ensures any URL redirects to the controller page
  server.onNotFound( handleNotFound );
  
  wifiConnect();
  
  server.begin();
  Serial.println( "Http server started ..." );
  
}

//
// Wifi Loop
//

void wifiLoop() {
  wifiConnect();
  // Process DNS requests for captive portal
  //dnsServer.processNextRequest();
  handleWebServer( "wifiLoop" );
  

}
