#include <SoftwareSerial.h>
#include <Adafruit_MotorShield.h>
#include <SerialCommand.h>
#include "Controller.h"

#define LEFT_MOTOR_NUMBER  1
#define RIGHT_MOTOR_NUMBER 2
#define STEPS_PER_REVOLUTION 200
#define REVOLUTIONS_PER_SECOND 1

#define FRONT_LEFT_MOTOR_NUMBER   1
#define BACK_LEFT_MOTOR_NUMBER    2
#define BACK_RIGHT_MOTOR_NUMBER   3
#define FRONT_RIGHT_MOTOR_NUMBER  4

#define PWM_MIN -255
#define PWM_MAX  255
#define MIN_SPEED 50
#define BLUETOOTH_TIMEOUT 1000

SoftwareSerial btSerial(3, 2); //RX | TX pins
SoftwareSerial imuSerial(5, 4);

SerialCommand serialCommand(imuSerial);

Controller controller;

Adafruit_MotorShield motorManager;
Adafruit_StepperMotor *motorLeft;
Adafruit_StepperMotor *motorRight;

int rightSideMotorSpeedCommand;
int leftSideMotorSpeedCommand;
long lastUpdateTimeMs = millis();

int fallThreshold = 60; // give up if robot is more than 60 degrees from vertical


void setup() {
  Serial.begin(57600);
  Serial.println("Starting setup");
  //btSerial.begin(38400); //Baud rate may vary depending on your chip's settings!
  imuSerial.begin(9600);
  imuSerial.listen();
  serialCommand.addCommand("R", rollReceived);
  serialCommand.addCommand("P", pitchReceived);
  serialCommand.addCommand("Y", yawReceived);
  serialCommand.addDefaultHandler(unrecognized);
  initMotors();
  
}
/*
* Loads serial data if available, releases motors after 1 second with data
*/
void loop() {
  serialCommand.readSerial();
//  if(readBtSerialData()){
//    lastUpdateTimeMs = millis();
//    setMotorSpeeds(rightSideMotorSpeedCommand, leftSideMotorSpeedCommand);
//  } else if(millis() - lastUpdateTimeMs > BLUETOOTH_TIMEOUT) {
//    Serial.println("Bluetooth timed out, releasing motors");
//    lastUpdateTimeMs = millis();
//    releaseMotors();
//  }
}

void rollReceived() {
  double roll = atof(serialCommand.next());
  Serial.print("Roll: ");
  Serial.println(roll);
  controller.roll = roll;
}

void pitchReceived() {
  double pitch = atof(serialCommand.next());
  Serial.print("Pitch: ");
  Serial.println(pitch);
  controller.pitch = pitch;
}

void yawReceived() {
  double yaw = atof(serialCommand.next());
  Serial.print("Yaw: ");
  Serial.println(yaw);
  controller.yaw = yaw;
}

void unrecognized() {
  char *next = serialCommand.next();
  Serial.print("Unrecognized command: ");
  if (next != NULL) {
    Serial.println(next);
  } else {
    Serial.println("No second argument");
  }
}

void initMotors(){
  motorManager = Adafruit_MotorShield();
  motorLeft = motorManager.getStepper(STEPS_PER_REVOLUTION, LEFT_MOTOR_NUMBER);
  motorRight = motorManager.getStepper(STEPS_PER_REVOLUTION, RIGHT_MOTOR_NUMBER);
  motorLeft->setSpeed(REVOLUTIONS_PER_SECOND);
  motorRight->setSpeed(REVOLUTIONS_PER_SECOND);
  motorManager.begin();
  releaseMotors();
}

/*
* Sets all motors speeds, assumes good data
*/
void setMotorSpeeds(float leftMotorSpeed, float rightMotorSpeed){
  setMotorSpeed(motorLeft, leftMotorSpeed);
  setMotorSpeed(motorRight, rightMotorSpeed);
}

void setMotorSpeed(Adafruit_StepperMotor *motor, double radiansPerSec) {
  // todo set speed here
  int steps = 10;
  if (radiansPerSec > 0) {
    motor->step(10, FORWARD, SINGLE);
  } else {
    motor->step(10, BACKWARD, SINGLE);
  }
}

void releaseMotors(){
  motorLeft->release();
  motorRight->release();
}
