#include "arduino_secrets.h"
#include "thingProperties.h"
/*
  Motor

*/
#include <string.h>

void log(String s) {
  Serial.println(s);
}

#define maxMotors 2

const int ledPin = LED_BUILTIN; // pin to use for the LED

class Motor {
  private:
    String id;
    int in1;
    int in2;
    int ena;
    byte mode;  // fwd, rev, off
    byte speed;  // 0 - 255
  public:
    void init(String name, int p1, int p2, int ena);
    void control(byte ctl, int spd);
};

void Motor::init(String name, int p1, int p2, int en) {
  id = name;
  in1 = p1;
  in2 = p2;
  ena = en;

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(ena, OUTPUT);
  digitalWrite(in1, LOW);  // turn off
  digitalWrite(in2, LOW);
  analogWrite(ena, 0);
};

void Motor:: control(byte sw, int bri) {
  int ctl = (sw) ? ((bri > 50)? 2 : 1) : 0; // 2 fwd, 1 rev, 0 off
  int spd = 5 * abs(bri - 50) + 5;  // range 55-255
  log(id + ": " + ctl + ", " + spd);

  mode = ctl;
  speed = max(50, spd);
  speed = min(255, spd);

  // stop the motor before any change
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  delay(150);

  // set motor mode/direction
  digitalWrite(in1, (mode & 0x01) ? HIGH : LOW);
  digitalWrite(in2, (mode & 0x02) ? HIGH : LOW);
  // speed
  analogWrite(ena, spd);
};

Motor motors[maxMotors];

void onLiftChange()  {
  motors[0].control(lift.getSwitch(), int(lift.getBrightness()));
}

void onMoveChange()  {
  motors[1].control(move.getSwitch(), int(move.getBrightness()));
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)

  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  motors[0].init("lift", 3, 4, 2);
  motors[1].init("move", 6, 7, 5); 
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
  */
  setDebugMessageLevel(4);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();
}
