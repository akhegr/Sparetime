#include <AFMotor.h>

AF_DCMotor motor1(1); // create motor #2, 64KHz pwm
AF_DCMotor motor2(2); // create motor #2, 64KHz pwm
int drejekransRight = 14;
int drejekransLeft = 15;
int kloOpen = 16;
int kloClose = 17;
int rotorBlink = 18;

void setup() {     // presenting pins as outputs
  Serial.begin(9600); // initialization
  
  pinMode(drejekransRight, INPUT);
  pinMode(drejekransLeft, INPUT);
  pinMode(kloOpen, INPUT);
  pinMode(kloClose, INPUT);
  pinMode(rotorBlink, OUTPUT);
  
  motor1.setSpeed(255);
  motor2.setSpeed(255);
  
  Serial.begin(9600);
}

void loop() {
  
  //Drejekrans styring
  if (digitalRead(drejekransRight)==HIGH && digitalRead(drejekransLeft)==LOW)
     {
      motor1.run(FORWARD);
      Serial.println("Drejekrans hoejre?");
     }
     
   if (digitalRead(drejekransRight)==LOW && digitalRead(drejekransLeft)==HIGH)
     {
      motor1.run(BACKWARD);
      Serial.println("Drejekrans venstre?");
     }
          
    if (digitalRead(drejekransRight)==LOW && digitalRead(drejekransLeft)==LOW) 
     {
      motor1.run(RELEASE);
      Serial.println("Drejekrans stop");
     }



  //Klo styring
  if (digitalRead(kloOpen)==HIGH && digitalRead(kloClose)==LOW)
     {
      motor2.run(FORWARD);
      Serial.println("Klo aaben");
     }
     
   if (digitalRead(kloOpen)==LOW && digitalRead(kloClose)==HIGH) 
     {
      motor2.run(BACKWARD);
      Serial.println("Klo lukket");
     }
          
    if (digitalRead(kloOpen)==LOW && digitalRead(kloClose)==LOW) 
     {
      motor2.run(RELEASE);
      Serial.println("Klo stop");
     }

  //Rotor blink
  digitalWrite(rotorBlink, HIGH);
  delay(500);
  digitalWrite(rotorBlink, LOW);
  delay(500);
}
