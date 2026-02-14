
//#include <Wire.h>
//#include <ESP32Servo.h>

#include <Servo.h>

#include "movement-sequences.h"

// Servo Pins for Distro Board
// ======================================================================
// Pin numbers are coorisponding to the ESP32 GPIO pins and may differ based on which board you use.
// If you are using a different board, please adjust the servoPins array accordingly.
// ======================================================================
Servo servos[8];
// Sesame Distro Board Pinout
//const int servoPins[8] = {15, 2, 23, 19, 4, 16, 17, 18};

// should create a structure for each servo with label, pin, min/max config, forward/side angles or up/down angles (also sets range)

// Lolin S2 Mini Pinout
//             motor num          6     7     8     9     
//                                  R1   R2    L1    L2      R4    R3    L3    L4
//                                   0,   1,    2,    3,      4,    5,    6,    7                   
//const int servoPins[8] =     {   12,   14,   19,   17,     13,   15,   18,   16 };
//const int servoMins[8] =     {  530,  430,  550,  640,    500,  690,  400,  380 };
//const int servoMaxs[8] =     { 2600, 2440, 2510, 2550,   2520, 2600, 2450, 2370 };
//const int servoAngleMin[8] = {   90,    0,    0,   90,      0,    0,    0,    0 };
//const int servoAngleMax[8] = {  180,   90,   90,  180,    180,  180,  180,  180 };
const int servoPins[8] =     {   12,   14,   19,   17,     15,  13,   18,   16 };
const int servoMins[8] =     {  530,  430,  550,  640,    690, 500,  400,  380 };
const int servoMaxs[8] =     { 2600, 2440, 2510, 2550,   2600, 2520, 2450, 2370 };
const int servoAngleMin[8] = {   90,    0,    0,   90,      0,    0,    0,    0 };
const int servoAngleMax[8] = {  180,   90,   90,  180,    180,  180,  180,  180 };
//                                                         ft    ft    ft    ft
//                               r1    r2    l1    l2      r4    r3    l3    l4
//                     forward  180     0     0   180         
//                     to side   90    90    90    90      
//                     up                                 180     0     0   180
//                     down                                 0   180   180     0
// Stand angles as reference
//setServoAngle(0, 135);  r1 12
//setServoAngle(1, 45);   r2 14
//setServoAngle(2, 45);   l1 19
//setServoAngle(3, 135);  l2 17
//setServoAngle(4, 0);    r3 13
//setServoAngle(5, 180);  r4 15
//setServoAngle(6, 0);    l3 18
//setServoAngle(7, 180);  l4 16

// Animation constants
int frameDelay = 100;
int walkCycles = 10;
int motorCurrentDelay = 20; // ms delay between motor movements to prevent over-current

// Prototypes
void setServoAngle(uint8_t channel, int angle);
void updateFaceBitmap(const unsigned char* bitmap);
void setFace(const String& faceName);
void setFaceMode(FaceAnimMode mode);
void setFaceWithMode(const String& faceName, FaceAnimMode mode);
void updateAnimatedFace();
void delayWithFace(unsigned long ms);
void enterIdle();
void exitIdle();
void updateIdleBlink();
int getFaceFpsForName(const String& faceName);
bool pressingCheck(String cmd, int ms);

void servosAngle( int angle ) {
  Serial.println( "" );
  for ( int i = 0; i < 8; i++ ) {
    int newAngle = angle;
    int min = servoAngleMin[i];
    int max = servoAngleMax[i];
    if ( newAngle < min ) {
      newAngle = min;
    }
    if ( newAngle > max ) {
      newAngle = max;
    }
    Serial.print( "servo: " );
    Serial.print( i );
    Serial.print( " initial angle: " );
    Serial.print( angle );
    Serial.print( " angle: " );
    Serial.print( newAngle );
    Serial.println( "" );
    servos[i].write( newAngle );
    delay( 250 );
  }
}

void servoSetup() {

  // PWM Init
  //ESP32PWM::allocateTimer(0);
  //ESP32PWM::allocateTimer(1);
  //ESP32PWM::allocateTimer(2);
  //ESP32PWM::allocateTimer(3);
  
  for (int i = 0; i < 8; i++) {
    //servos[i].setPeriodHertz(50);
    // Map 0-180 to approx 732-2929us
    //servos[i].attach(servoPins[i], 732, 2929); // Range to wide for some servos, damaged one
    int pin = servoPins[i];
    Serial.print( "servo.attach servo: " );
    Serial.print( i );
    Serial.print( " pin: " );
    Serial.print( pin );
    int res = servos[i].attach( pin, servoMins[i], servoMaxs[i] );
    Serial.print( " res: " );
    Serial.println( res );
  }
  delay(10);
  servosAngle( 90 );
  
}

void servoLoop() {
  
  if (currentCommand != "") {
    String cmd = currentCommand;
    if (cmd == "forward") runWalkPose();
    else if (cmd == "backward") runWalkBackward();
    else if (cmd == "left") runTurnLeft();
    else if (cmd == "right") runTurnRight();
    else if (cmd == "rest") { runRestPose(); if (currentCommand == "rest") currentCommand = ""; }
    else if (cmd == "stand") { runStandPose(1); if (currentCommand == "stand") currentCommand = ""; }
    else if (cmd == "wave") runWavePose();
    else if (cmd == "dance") runDancePose();
    else if (cmd == "swim") runSwimPose();
    else if (cmd == "point") runPointPose();
    else if (cmd == "pushup") runPushupPose();
    else if (cmd == "bow") runBowPose();
    else if (cmd == "cute") runCutePose();
    else if (cmd == "freaky") runFreakyPose();
    else if (cmd == "worm") runWormPose();
    else if (cmd == "shake") runShakePose();
    else if (cmd == "shrug") runShrugPose();
    else if (cmd == "dead") runDeadPose();
    else if (cmd == "crab") runCrabPose();
  }

  // Serial CLI for debugging (can be used to diagnose servo position issues and wiring)
  if (Serial.available()) {
    static char command_buffer[32];
    static byte buffer_pos = 0;
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (buffer_pos > 0) {
        command_buffer[buffer_pos] = '\0';
        int motorNum, angle;
        if(strcmp(command_buffer, "run walk") == 0 || strcmp(command_buffer, "rn wf") == 0) { currentCommand = "forward"; runWalkPose(); currentCommand = ""; }
        else if(strcmp(command_buffer, "rn wb") == 0) { currentCommand = "backward"; runWalkBackward(); currentCommand = ""; }
        else if(strcmp(command_buffer, "rn tl") == 0) { currentCommand = "left"; runTurnLeft(); currentCommand = ""; }
        else if(strcmp(command_buffer, "rn tr") == 0) { currentCommand = "right"; runTurnRight(); currentCommand = ""; }
        else if(strcmp(command_buffer, "run rest") == 0 || strcmp(command_buffer, "rn rs") == 0) runRestPose();
        else if(strcmp(command_buffer, "run stand") == 0 || strcmp(command_buffer, "rn st") == 0) runStandPose(1);
        else if(strcmp(command_buffer, "rn wv") == 0) { currentCommand = "wave"; runWavePose(); }
        else if(strcmp(command_buffer, "rn dn") == 0) { currentCommand = "dance"; runDancePose(); }
        else if(strcmp(command_buffer, "rn sw") == 0) { currentCommand = "swim"; runSwimPose(); }
        else if(strcmp(command_buffer, "rn pt") == 0) { currentCommand = "point"; runPointPose(); }
        else if(strcmp(command_buffer, "rn pu") == 0) { currentCommand = "pushup"; runPushupPose(); }
        else if(strcmp(command_buffer, "rn bw") == 0) { currentCommand = "bow"; runBowPose(); }
        else if(strcmp(command_buffer, "rn ct") == 0) { currentCommand = "cute"; runCutePose(); }
        else if(strcmp(command_buffer, "rn fk") == 0) { currentCommand = "freaky"; runFreakyPose(); }
        else if(strcmp(command_buffer, "rn wm") == 0) { currentCommand = "worm"; runWormPose(); }
        else if(strcmp(command_buffer, "rn sk") == 0) { currentCommand = "shake"; runShakePose(); }
        else if(strcmp(command_buffer, "rn sg") == 0) { currentCommand = "shrug"; runShrugPose(); }
        else if(strcmp(command_buffer, "rn dd") == 0) { currentCommand = "dead"; runDeadPose(); }
        else if(strcmp(command_buffer, "rn cb") == 0) { currentCommand = "crab"; runCrabPose(); }
        else if (strncmp(command_buffer, "all ", 4) == 0) {
             if (sscanf(command_buffer + 4, "%d", &angle) == 1) {
                 for (int i = 0; i < 8; i++) setServoAngle(i, angle);
                 Serial.print("All servos set to "); Serial.println(angle);
             }
        }
        else if (sscanf(command_buffer, "%d %d", &motorNum, &angle) == 2) {
             if (motorNum >= 0 && motorNum < 8) {
                 setServoAngle(motorNum, angle);
                 Serial.print("Servo "); Serial.print(motorNum); Serial.print(" set to "); Serial.println(angle);
             } else {
                 Serial.println("Invalid motor number (0-7)");
             }
        }
        buffer_pos = 0;
      }
    } else if (buffer_pos < sizeof(command_buffer) - 1) {
      command_buffer[buffer_pos++] = c;
    }
  }

}

// ====== HELPERS ======
void setServoAngle(uint8_t channel, int angle) { 
  if (channel < 8) {
    Serial.print( "setServoAngle for servo: " );
    Serial.print( channel );
    Serial.print( " angle: " );
    Serial.println( angle );
    servos[channel].write(angle);
    delayWithFace(motorCurrentDelay);
  }
}

bool pressingCheck(String cmd, int ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    //server.handleClient();
    //dnsServer.processNextRequest();
    updateAnimatedFace();
    if (currentCommand != cmd) {
      runStandPose(1);
      return false;
    }
    yield();
  }
  return true;
}

