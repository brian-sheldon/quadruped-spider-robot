
#include <vector>
#include <algorithm>
#include <string>

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

class ServoConfig {
private:
  int _index;
  String _label;
  int _pin;
  int _dutyMin;
  int _dutyMax;
  std::vector<int> _angles;
  std::vector<String> _anglesLabel;
  int _angle;
  int _angleMin;
  int _angleMax;
public:
  //ServoCmd();
  ServoConfig() {
  }
  //void config( int index, String label, const std::vector<int>& angles, const std::vector<char16_t>& anglesLabel );
  void config(int index, String label, int pin, int dutyMin, int dutyMax, const std::vector<int>& angles, const std::vector<String>& anglesLabel) {
    // basic info
    _index = index;
    _label = label;
    // pin
    _pin = pin;
    // duty cycle min and max
    _dutyMin = dutyMin;
    _dutyMax = dutyMax;
    // array of angles to rotate thru for testing and their labels
    _angles = angles;
    _anglesLabel = anglesLabel;
    _angle = angles[0];
    // use the array of test angles to set the min and max angle limits for server
    // limit may be reduced for server installed in a robot
    std::vector<int>::iterator min_it = std::min_element(_angles.begin(), _angles.end());
    std::vector<int>::iterator max_it = std::max_element(_angles.begin(), _angles.end());
    _angleMin = *min_it;
    _angleMax = *max_it;
    attach();
    // lets print the config to serial
    serialPrintState();
    //Serial.println(stateJson());
  }
  void attach() {
    Serial.print( "servo.attach servo: " );
    Serial.print( _index );
    Serial.print( " pin: " );
    Serial.print( _pin );
    int res = servos[ _index ].attach( _pin, _dutyMin, _dutyMax );
    setAngle( _angles[0] );
    Serial.print( " res: " );
    Serial.println( res );
  }
  int getAngle() {
    return _angle;
  }
  void setAngle(int angle) {
    if (angle >= _angleMin && angle <= _angleMax) {
      _angle = angle;
      setServoAngle( _index, angle );
      Serial.println( "" );
      Serial.print("setAngle servo: ");
      Serial.print(_label);
      Serial.print(" angle: ");
      Serial.print(_angle);
      Serial.println("");
    }
  }
  String stateJson() {
    String tab = "  ";
    String lf = "\n";
    String json = "{" + lf;
    json += tab + "\"index\":" + _index + "," + lf;
    json += tab + "\"label\":\"" + _label + "\"," + lf;
    json += tab + "\"pin\":" + _pin + "," + lf;
    json += tab + "\"dutyMin\":" + _dutyMin + "," + lf;
    json += tab + "\"dutyMax\":" + _dutyMax + "," + lf;
    json += tab + "\"angleMin\":" + _angleMin + "," + lf;
    json += tab + "\"angleMax\":" + _angleMax + "," + lf;
    json += tab + "\"angle:\":" + _angle + "," + lf;
    json += tab + "\"angles\":[" + lf;
    for (int i = 0; i < _angles.size(); i++) {
      json += tab + tab + _angles[i];
      if (i != _angles.size() - 1) {
        json += ",";
      }
      json += lf;
    }
    json += tab + "]," + lf;
    json += tab + "\"anglesLabel\":[" + lf;
    for (int i = 0; i < _anglesLabel.size(); i++) {
      json += tab + tab + "\"" + _anglesLabel[i] + "\"";
      if (i != _anglesLabel.size() - 1) {
        json += ",";
      }
      json += lf;
    }
    json += tab + "]" + lf;
    json += "}";
    return json;
  }
  void serialPrintState() {
    Serial.print( "initServo index: ");
    Serial.print( _index);
    Serial.print( " label: ");
    Serial.print( _label);
    Serial.print( " pin: " );
    Serial.print( _pin );
    Serial.print(" minDuty: ");
    Serial.print(_dutyMin);
    Serial.print(" maxDuty: ");
    Serial.print(_dutyMax);
    Serial.print(" minAngle: ");
    Serial.print(_angleMin);
    Serial.print(" maxAngle: ");
    Serial.print(_angleMax);
    Serial.print(" angle: ");
    Serial.print(_angle);
    Serial.println("");
  }
};

ServoConfig servoConfig[8];

void servoConfigInit() {
  String n = u8"\u2191";
  String ne = u8"\u2197";
  String e = u8"\u2192";
  String se = u8"\u2198";
  String s = u8"\u2193";
  String sw = u8"\u2199";
  String w = u8"\u2190";
  String nw = u8"\u2196";

  int i = 0;

  // yes, R4 and R3 don't follow the order pattern, but that is the order in the original code
  //
  //                    index label pin   min   max    angles                   angle labels
  //
  servoConfig[i].config( i++, "R1",  12,  530, 2600, { 135, 90, 180        }, { ne, e, n        } );
  servoConfig[i].config( i++, "R2",  14,  430, 2440, { 45, 90, 0           }, { se, e, s        } );
  servoConfig[i].config( i++, "L1",  19,  550, 2510, { 45, 90, 0           }, { nw, w, n        } );
  servoConfig[i].config( i++, "L2",  17,  640, 2550, { 135, 90, 180        }, { sw, w, s        } );
  servoConfig[i].config( i++, "R4",  15,  690, 2600, { 0, 45, 90, 135, 180 }, { s, se, e, ne, n } );
  servoConfig[i].config( i++, "R3",  13,  500, 2520, { 180, 135, 90, 45, 0 }, { s, se, e, ne, n } );
  servoConfig[i].config( i++, "L3",  18,  400, 2450, { 0, 45, 90, 135, 180 }, { s, se, e, ne, n } );
  servoConfig[i].config( i++, "L4",  16,  380, 2370, { 180, 135, 90, 45, 0 }, { s, se, e, ne, n } );
}

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
//const int servoPins[8] = { 12, 14, 19, 17, 15, 13, 18, 16 };
//const int servoMins[8] = { 530, 430, 550, 640, 690, 500, 400, 380 };
//const int servoMaxs[8] = { 2600, 2440, 2510, 2550, 2600, 2520, 2450, 2370 };
//const int servoAngleMin[8] = { 90, 0, 0, 90, 0, 0, 0, 0 };
//const int servoAngleMax[8] = { 180, 90, 90, 180, 180, 180, 180, 180 };
//                                                         ft    ft    ft    ft
//                               r1    r2    l1    l2      r4    r3    l3    l4
//                     forward  180     0     0   180
//                     to side   90    90    90    90
//                     up                                 180     0     0   180
//                     down                                 0   180   180     0

// Animation constants
int frameDelay = 100;
int walkCycles = 10;
int motorCurrentDelay = 20;  // ms delay between motor movements to prevent over-current

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

void servoSetup() {

  // PWM Init
  //ESP32PWM::allocateTimer(0);
  //ESP32PWM::allocateTimer(1);
  //ESP32PWM::allocateTimer(2);
  //ESP32PWM::allocateTimer(3);

  servoConfigInit();

  delay(10);
}

void servoLoop() {

  if (currentCommand != "") {
    String cmd = currentCommand;
    if (cmd == "forward") runWalkPose();
    else if (cmd == "backward") runWalkBackward();
    else if (cmd == "left") runTurnLeft();
    else if (cmd == "right") runTurnRight();
    else if (cmd == "rest") {
      runRestPose();
      if (currentCommand == "rest") currentCommand = "";
    } else if (cmd == "stand") {
      runStandPose(1);
      if (currentCommand == "stand") currentCommand = "";
    } else if (cmd == "wave") runWavePose();
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
        if (strcmp(command_buffer, "run walk") == 0 || strcmp(command_buffer, "rn wf") == 0) {
          currentCommand = "forward";
          runWalkPose();
          currentCommand = "";
        } else if (strcmp(command_buffer, "rn wb") == 0) {
          currentCommand = "backward";
          runWalkBackward();
          currentCommand = "";
        } else if (strcmp(command_buffer, "rn tl") == 0) {
          currentCommand = "left";
          runTurnLeft();
          currentCommand = "";
        } else if (strcmp(command_buffer, "rn tr") == 0) {
          currentCommand = "right";
          runTurnRight();
          currentCommand = "";
        } else if (strcmp(command_buffer, "run rest") == 0 || strcmp(command_buffer, "rn rs") == 0) runRestPose();
        else if (strcmp(command_buffer, "run stand") == 0 || strcmp(command_buffer, "rn st") == 0) runStandPose(1);
        else if (strcmp(command_buffer, "rn wv") == 0) {
          currentCommand = "wave";
          runWavePose();
        } else if (strcmp(command_buffer, "rn dn") == 0) {
          currentCommand = "dance";
          runDancePose();
        } else if (strcmp(command_buffer, "rn sw") == 0) {
          currentCommand = "swim";
          runSwimPose();
        } else if (strcmp(command_buffer, "rn pt") == 0) {
          currentCommand = "point";
          runPointPose();
        } else if (strcmp(command_buffer, "rn pu") == 0) {
          currentCommand = "pushup";
          runPushupPose();
        } else if (strcmp(command_buffer, "rn bw") == 0) {
          currentCommand = "bow";
          runBowPose();
        } else if (strcmp(command_buffer, "rn ct") == 0) {
          currentCommand = "cute";
          runCutePose();
        } else if (strcmp(command_buffer, "rn fk") == 0) {
          currentCommand = "freaky";
          runFreakyPose();
        } else if (strcmp(command_buffer, "rn wm") == 0) {
          currentCommand = "worm";
          runWormPose();
        } else if (strcmp(command_buffer, "rn sk") == 0) {
          currentCommand = "shake";
          runShakePose();
        } else if (strcmp(command_buffer, "rn sg") == 0) {
          currentCommand = "shrug";
          runShrugPose();
        } else if (strcmp(command_buffer, "rn dd") == 0) {
          currentCommand = "dead";
          runDeadPose();
        } else if (strcmp(command_buffer, "rn cb") == 0) {
          currentCommand = "crab";
          runCrabPose();
        } else if (strncmp(command_buffer, "all ", 4) == 0) {
          if (sscanf(command_buffer + 4, "%d", &angle) == 1) {
            for (int i = 0; i < 8; i++) setServoAngle(i, angle);
            Serial.print("All servos set to ");
            Serial.println(angle);
          }
        } else if (sscanf(command_buffer, "%d %d", &motorNum, &angle) == 2) {
          if (motorNum >= 0 && motorNum < 8) {
            setServoAngle(motorNum, angle);
            Serial.print("Servo ");
            Serial.print(motorNum);
            Serial.print(" set to ");
            Serial.println(angle);
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
void setServoAngle( uint8_t channel, int angle ) {
  if ( channel >= 0 && channel <= 7 ) {
    Serial.print( "setServoAngle for servo: " );
    Serial.print( channel );
    Serial.print( " angle: " );
    Serial.println( angle );
    //if ( false ) {
      servos[ channel ].write( angle );
    //}
    delayWithFace( motorCurrentDelay );
  }
}

bool pressingCheck(String cmd, int ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    handleWebServer( "pressingCheck" );
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
