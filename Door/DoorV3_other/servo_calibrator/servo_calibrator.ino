// Servo
#include <Servo.h>
int servopin = 3; byte servoEnable = 8;
Servo servo;

void setup() {
  // Sevo //
  pinMode(servoEnable, OUTPUT);
  servo.attach(servopin);
  delay(50);
}

void loop() {
  delay(1000);
  servo.write(180);
}
